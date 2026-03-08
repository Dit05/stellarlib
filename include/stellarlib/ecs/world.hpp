/* clang-format off */

/*
  stellarlib
  Copyright (C) 2025-2026 Domán Zana

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef STELLARLIB_ECS_WORLD_HPP
#define STELLARLIB_ECS_WORLD_HPP

#include <stellarlib/ecs/archetype.hpp>
#include <stellarlib/ecs/sparse_map.hpp>
#include <stellarlib/ecs/sparse_set.hpp>
#include <stellarlib/ecs/sparse_storage.hpp>
#include <stellarlib/ecs/stack_vector.hpp>
#include <stellarlib/ext/type_traits.hpp>
#include <stellarlib/ext/utility.hpp>

#include <algorithm>
#include <expected>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <tuple>
#include <ranges>

namespace stellarlib::ecs
{
class world final
{
public:
	[[nodiscard]]
	explicit world() noexcept;

	[[nodiscard]]
	world(const world &) noexcept;

	[[nodiscard]]
	world(world &&) noexcept;

	auto operator=(const world &) noexcept
		-> world &;

	auto operator=(world &&) noexcept
		-> world &;

	~world() noexcept;

	template <typename ...T>
	constexpr auto spawn(T &&...components) noexcept
	{
		auto entity{_entities.size()};

		if (_queue.size()) {
			entity = *(_queue.end() - 1);
			_queue.pop();
		}

		[&]<std::size_t ...I>(std::index_sequence<I...>) -> void {
			const auto &ids{internal::sparse_storage::ids<T...>()};
			(_components.at<T>(ids[I]).insert(entity, std::forward<T>(components)), ...);
		}(std::index_sequence_for<T...>{});

		relocate(entity, archetype::of<T...>());
		return entity;
	}

	template <typename ...T>
	[[nodiscard]]
	constexpr auto insert(const std::uint32_t entity, T &&...components) noexcept
		-> std::expected<void, std::tuple<T...>>
		requires (0 < sizeof...(T))
	{
		const auto it{_entities.at(entity)};

		if (!it) {
			return std::unexpected(std::tuple{std::forward<T>(components)...});
		}

		[&]<std::size_t ...I>(std::index_sequence<I...>) -> void {
			const auto &ids{internal::sparse_storage::ids<T...>()};
			(_components.at<T>(ids[I]).insert(entity, std::forward<T>(components)), ...);
		}(std::index_sequence_for<T...>{});

		cache = _archetypes[*it].first;

		if constexpr (1 < sizeof...(T)) {
			cache.insert(archetype::of<T...>());
		}
		else {
			cache.insert(internal::sparse_storage::ids<T...>().front());
		}

		relocate(entity, it);
		return {};
	}

	[[nodiscard]]
	auto contains(std::uint32_t entity) const noexcept
		-> bool;

	template <typename ...T>
	[[nodiscard]]
	constexpr auto contains(const std::uint32_t entity) const noexcept
		requires (0 < sizeof...(T))
	{
		return [&]<std::size_t ...I>(std::index_sequence<I...>) -> auto {
			if (const auto it{_entities.at(entity)}) {
				const auto &ids{internal::sparse_storage::ids<T...>()};
				return std::tuple{_archetypes[*it].first.contains(std::get<I>(ids))...};
			}

			return std::tuple{ext::expand_as_v<T, false>...};
		}(std::index_sequence_for<T...>{});
	}

	[[nodiscard]]
	auto at(std::uint32_t entity) const noexcept
		-> const archetype *;

	template <typename ...T>
	[[nodiscard]]
	constexpr auto at(const std::uint32_t entity) noexcept
		requires (0 < sizeof...(T))
	{
		return [&]<std::size_t ...I>(std::index_sequence<I...>) -> auto {
			const auto &ids{internal::sparse_storage::ids<T...>()};
			return std::tuple{_components.at<T>(std::get<I>(ids)).at(entity)...};
		}(std::index_sequence_for<T...>{});
	}

	[[nodiscard]]
	auto operator[](std::uint32_t entity) const noexcept
		-> const archetype &;

	template <typename ...T>
	[[nodiscard]]
	constexpr auto operator[](const std::uint32_t entity) const noexcept
		requires (0 < sizeof...(T))
	{
		return [&]<std::size_t ...I>(std::index_sequence<I...>) -> std::tuple<T &...> {
			const auto &ids{internal::sparse_storage::ids<T...>()};
			return {_components.operator[]<T>(std::get<I>(ids))[entity]...};
		}(std::index_sequence_for<T...>{});
	}

	[[nodiscard]]
	constexpr auto query() const noexcept
	{
		return _archetypes | std::views::transform([](const auto &pair) -> auto {
			return pair.second | std::views::transform([&](const auto entity) -> std::tuple<std::uint32_t, const archetype &> {
				return {entity, pair.first};
			});
		}) | std::views::join;
	}

	template <typename T>
	[[nodiscard]]
	constexpr auto query() noexcept
	{
		return _components.at<T>(internal::sparse_storage::ids<T>().front()).zip();
	}

	template <typename ...T>
	[[nodiscard]]
	constexpr auto query() noexcept
		requires (1 < sizeof...(T))
	{
		const auto id{ext::scoped_typeid<world, std::tuple<T...>, std::uint16_t>()};

		if (_queries.extend(id + 1, static_cast<std::uint16_t>(-1)) || _queries[id] == static_cast<std::uint16_t>(-1)) {
			const auto &archetype{archetype::of<T...>()};

			const auto it{std::ranges::find_if(_indices, [&](const auto &pair) -> auto {
				return pair.first == archetype;
			})};

			if (it == _indices.end()) {
				_queries[id] = _indices.size();
				_indices.push(std::pair{archetype, internal::stack_vector<std::uint16_t>{}});

				for (const auto i : std::views::iota(std::uint16_t{}, _archetypes.size())) {
					if (archetype <= _archetypes[i].first) {
						(_indices.end() - 1)->second.push(i);
					}
				}
			}
			else {
				_queries[id] = static_cast<std::uint16_t>(it - _indices.begin());
			}
		}

		const auto components{[this]<std::size_t ...I>(std::index_sequence<I...>) -> std::tuple<const internal::sparse_map<std::uint32_t, T> &...> {
			const auto &ids{internal::sparse_storage::ids<T...>()};
			return {_components.operator[]<T>(ids[I])...};
		}(std::index_sequence_for<T...>{})};

		return _indices[_queries[id]].second | std::views::transform([this](const auto index) -> const internal::sparse_set<std::uint32_t> & {
			return _archetypes[index].second;
		}) | std::views::join | std::views::transform([=](const auto entity) -> std::tuple<std::uint32_t, T &...> {
			return [&]<std::size_t ...I>(std::index_sequence<I...>) -> std::tuple<std::uint32_t, T &...> {
				return {entity, std::get<I>(components)[entity]...};
			}(std::index_sequence_for<T...>{});
		});
	}

	template <typename ...T>
	constexpr void erase(const std::uint32_t entity) noexcept
		requires (0 < sizeof...(T))
	{
		const auto it{_entities.at(entity)};

		if (!it) {
			return;
		}

		[&]<std::size_t ...I>(std::index_sequence<I...>) -> void {
			const auto &ids{internal::sparse_storage::ids<T...>()};
			(_components.at<T>(ids[I]).erase(entity), ...);
		}(std::index_sequence_for<T...>{});

		cache = _archetypes[*it].first;

		if constexpr (1 < sizeof...(T)) {
			cache.erase(archetype::of<T...>());
		}
		else {
			cache.erase(internal::sparse_storage::ids<T...>().front());
		}

		relocate(entity, it);
	}

	void despawn(std::uint32_t entity) noexcept;

	void clear() noexcept;

private:
	static thread_local archetype cache;
	internal::stack_vector<std::uint32_t, std::uint32_t> _queue;
	internal::sparse_map<std::uint32_t, std::uint16_t> _entities;
	internal::stack_vector<std::pair<archetype, internal::sparse_set<std::uint32_t>>, std::uint16_t> _archetypes;
	internal::sparse_storage _components;
	internal::stack_vector<std::pair<archetype, internal::stack_vector<std::uint16_t>>, std::uint16_t> _indices;
	internal::stack_vector<std::uint16_t, std::uint16_t> _queries;

	template <typename T>
	constexpr void relocate(const std::uint32_t entity, const T &arg) noexcept
	{
		const auto it{std::ranges::find_if(_archetypes, [&](const auto &pair) -> auto {
			if constexpr (std::is_same_v<T, archetype>) {
				return pair.first == arg;
			}
			else if constexpr (std::is_same_v<T, std::uint16_t *>) {
				return pair.first == cache;
			}
		})};

		if constexpr (std::is_same_v<T, std::uint16_t *>) {
			_archetypes[*arg].second.erase(entity);
		}

		if (it == _archetypes.end()) {
			if constexpr (std::is_same_v<T, archetype>) {
				_entities.insert(entity, _archetypes.size());
				_archetypes.push(std::pair{arg, internal::sparse_set<std::uint32_t>{}});
			}
			else if constexpr (std::is_same_v<T, std::uint16_t *>) {
				_entities[entity] = _archetypes.size();
				_archetypes.push(std::pair{cache, internal::sparse_set<std::uint32_t>{}});
			}

			(_archetypes.end() - 1)->second.insert(entity);

			for (auto &index : _indices) {
				if (index.first <= (_archetypes.end() - 1)->first) {
					index.second.push(_archetypes.size() - 1);
				}
			}
		}
		else {
			if constexpr (std::is_same_v<T, archetype>) {
				_entities.insert(entity, static_cast<std::uint16_t>(it - _archetypes.begin()));
			}
			else if constexpr (std::is_same_v<T, std::uint16_t *>) {
				_entities[entity] = static_cast<std::uint16_t>(it - _archetypes.begin());
			}

			it->second.insert(entity);
		}
	}
};
}

#endif
