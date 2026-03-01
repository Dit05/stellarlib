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

#include <stellarlib/ecs/bitset.hpp>
#include <stellarlib/ecs/sparse_map.hpp>
#include <stellarlib/ecs/sparse_set.hpp>
#include <stellarlib/ecs/sparse_storage.hpp>
#include <stellarlib/ecs/stack_vector.hpp>
#include <stellarlib/ext/utility.hpp>

#include <algorithm>
#include <array>
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
	template <typename ...T>
	[[nodiscard]]
	static constexpr auto ids() noexcept
		-> const std::array<std::uint16_t, sizeof...(T)> &
	{
		static const std::array<std::uint16_t, sizeof...(T)> ids{internal::sparse_storage::id<T>()...};
		return ids;
	}

	template <typename ...T>
	[[nodiscard]]
	static constexpr auto archetype() noexcept
		-> const internal::bitset &
	{
		static const auto archetype{[] -> const internal::bitset & {
			bitset.clear();
			const auto &ids{world::ids<T...>()};

			[ids]<std::size_t ...I>(std::index_sequence<I...>) -> void {
				(bitset.insert(ids[I]), ...);
			}(std::index_sequence_for<T...>{});

			return bitset;
		}()};
		return archetype;
	}

	[[nodiscard]]
	explicit constexpr world() noexcept = default;

	[[nodiscard]]
	constexpr world(const world &) noexcept = default;

	[[nodiscard]]
	constexpr world(world &&) noexcept = default;

	constexpr auto operator=(const world &) noexcept
		-> world & = default;

	constexpr auto operator=(world &&) noexcept
		-> world & = default;

	constexpr ~world() noexcept = default;

	template <typename ...T>
	constexpr auto spawn(T &&...components) noexcept
	{
		auto entity{_entities.size()};

		if (_queue.size()) {
			entity = *(_queue.end() - 1);
			_queue.pop();
		}

		const auto &ids{world::ids<T...>()};

		[this, &ids, &entity, &components...]<std::size_t ...I>(std::index_sequence<I...>) -> void {
			(_components.at<T>(ids[I]).insert(entity, std::forward<T>(components)), ...);
		}(std::index_sequence_for<T...>{});

		const auto &archetype{world::archetype<T...>()};
		const auto it{std::ranges::find_if(_archetypes, [&archetype](const auto &pair) -> bool {
			return pair.first == archetype;
		})};

		if (it == _archetypes.end()) {
			_entities.insert(entity, _archetypes.size());
			_archetypes.push(std::pair{archetype, internal::sparse_set<std::uint32_t>{}});
			(_archetypes.end() - 1)->second.insert(entity);

			for (auto &index : _indices) {
				if (index.first <= archetype) {
					index.second.push(_archetypes.size() - 1);
				}
			}
		}
		else {
			it->second.insert(entity);
			_entities.insert(entity, static_cast<std::uint16_t>(it - _archetypes.begin()));
		}

		return entity;
	}

	template <typename ...T>
	[[nodiscard]]
	constexpr auto insert(const std::uint32_t entity, T &&...components) noexcept
		-> std::expected<void, std::tuple<T...>>
	{
		const auto i{_entities.at(entity)};

		if (!i) {
			return std::unexpected(std::forward<T>(components)...);
		}

		const auto &ids{world::ids<T...>()};

		[this, &ids, &entity, &components...]<std::size_t ...I>(std::index_sequence<I...>) -> void {
			(_components.at<T>(ids[I]).insert(entity, std::forward<T>(components)), ...);
		}(std::index_sequence_for<T...>{});

		const auto &extension{archetype<T...>()};
		bitset = _archetypes[*i].first;
		bitset.insert(extension);

		const auto it{std::ranges::find_if(_archetypes, [](const auto &pair) -> bool {
			return pair.first == bitset;
		})};

		if (it == _archetypes.end()) {
			_archetypes[*i].second.erase(entity);
			_entities[entity] = _archetypes.size();
			_archetypes.push(std::pair{bitset, internal::sparse_set<std::uint32_t>{}});
			(_archetypes.end() - 1)->second.insert(entity);

			for (auto &index : _indices) {
				if (index.first <= bitset) {
					index.second.push(_archetypes.size() - 1);
				}
			}
		}
		else {
			_archetypes[*i].second.erase(entity);
			_entities[entity] = static_cast<std::uint16_t>(it - _archetypes.begin());
			it->second.insert(entity);
		}

		return {};
	}

	[[nodiscard]]
	constexpr auto contains(const std::uint32_t entity) const noexcept
	{
		return _entities.contains(entity);
	}

	[[nodiscard]]
	constexpr auto at(const std::uint32_t entity) const noexcept
		-> const internal::bitset *
	{
		if (const auto index{_entities.at(entity)}) {
			return std::addressof(_archetypes[*index].first);
		}

		return nullptr;
	}

	template <typename ...T>
	[[nodiscard]]
	constexpr auto at(const std::uint32_t entity) noexcept
		requires (0 < sizeof...(T))
	{
		const auto &ids{world::ids<T...>()};
		return [&entity, this, &ids]<std::size_t ...I>(std::index_sequence<I...>) -> std::tuple<T *...> {
			return {_components.at<T>(std::get<I>(ids)).at(entity)...};
		}(std::index_sequence_for<T...>{});
	}

	[[nodiscard]]
	constexpr auto operator[](const std::uint32_t entity) const noexcept
		-> const internal::bitset &
	{
		return _archetypes[_entities[entity]].first;
	}

	template <typename ...T>
	[[nodiscard]]
	constexpr auto operator[](const std::uint32_t entity) const noexcept
		requires (0 < sizeof...(T))
	{
		const auto &ids{world::ids<T...>()};
		return [&entity, this, &ids]<std::size_t ...I>(std::index_sequence<I...>) -> std::tuple<T &...> {
			return {_components.operator[]<T>(std::get<I>(ids))[entity]...};
		}(std::index_sequence_for<T...>{});
	}

	[[nodiscard]]
	constexpr auto query() const noexcept
	{
		return _archetypes | std::views::transform([](const auto &pair) -> auto {
				return pair.second | std::views::transform([&pair](const auto entity) -> std::tuple<std::uint32_t, const internal::bitset &> {
					return {entity, pair.first};
				});
			}) | std::views::join;
	}

	template <typename T>
	[[nodiscard]]
	constexpr auto query() noexcept
	{
		return _components.at<T>(ids<T>().front()).zip();
	}

	template <typename ...T>
	[[nodiscard]]
	constexpr auto query() noexcept
		requires (1 < sizeof...(T))
	{
		const auto id{ext::scoped_typeid<world, std::tuple<T...>, std::uint16_t>()};

		if (_queries.extend(id + 1, static_cast<std::uint16_t>(-1)) || _queries[id] == static_cast<std::uint16_t>(-1)) {
			const auto &archetype{world::archetype<T...>()};
			const auto it{std::ranges::find_if(_indices, [&archetype](const auto &pair) -> bool {
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

		const auto &ids{world::ids<T...>()};
		const auto components{[this, &ids]<std::size_t ...I>(std::index_sequence<I...>) -> std::tuple<const internal::sparse_map<std::uint32_t, T> &...> {
			return {_components.operator[]<T>(ids[I])...};
		}(std::index_sequence_for<T...>{})};
		return _indices[_queries[id]].second | std::views::transform([this](const auto index) -> const internal::sparse_set<std::uint32_t> & {
				return _archetypes[index].second;
			}) | std::views::join | std::views::transform([components](const auto entity) -> std::tuple<std::uint32_t, T &...> {
				return [&entity, &components]<std::size_t ...I>(std::index_sequence<I...>) -> std::tuple<std::uint32_t, T &...> {
					return {entity, std::get<I>(components)[entity]...};
				}(std::index_sequence_for<T...>{});
			});
	}

	template <typename ...T>
	constexpr void erase(const std::uint32_t entity) noexcept
	{
		const auto i{_entities.at(entity)};

		if (!i) {
			return;
		}

		const auto &ids{world::ids<T...>()};

		[this, &ids, &entity]<std::size_t ...I>(std::index_sequence<I...>) -> void {
			(_components.at<T>(ids[I]).erase(entity), ...);
		}(std::index_sequence_for<T...>{});

		const auto &extension{archetype<T...>()};
		bitset = _archetypes[*i].first;
		bitset.erase(extension);

		const auto it{std::ranges::find_if(_archetypes, [](const auto &pair) -> bool {
			return pair.first == bitset;
		})};

		if (it == _archetypes.end()) {
			_archetypes[*i].second.erase(entity);
			_entities[entity] = _archetypes.size();
			_archetypes.push(std::pair{bitset, internal::sparse_set<std::uint32_t>{}});
			(_archetypes.end() - 1)->second.insert(entity);

			for (auto &index : _indices) {
				if (index.first <= bitset) {
					index.second.push(_archetypes.size() - 1);
				}
			}
		}
		else {
			_archetypes[*i].second.erase(entity);
			_entities[entity] = static_cast<std::uint16_t>(it - _archetypes.begin());
			it->second.insert(entity);
		}
	}

	constexpr auto despawn(const std::uint32_t entity) noexcept
	{
		if (const auto index{_entities.at(entity)}) {
			_entities.erase(entity);
			_archetypes[*index].second.erase(entity);
			_components.erase(entity);
			_queue.push(entity);
		}
	}

	constexpr auto clear() noexcept {
		_queue.clear();
		_entities.clear();
		_components.clear();

		for (auto &pair : _archetypes) {
			pair.second.clear();
		}
	}

private:
	static thread_local internal::bitset bitset;
	internal::stack_vector<std::uint32_t, std::uint32_t> _queue;
	internal::sparse_map<std::uint32_t, std::uint16_t> _entities;
	internal::stack_vector<std::pair<internal::bitset, internal::sparse_set<std::uint32_t>>, std::uint16_t> _archetypes;
	internal::sparse_storage _components;
	internal::stack_vector<std::uint16_t, std::uint16_t> _queries;
	internal::stack_vector<std::pair<internal::bitset, internal::stack_vector<std::uint16_t>>, std::uint16_t> _indices;
};
}

#endif
