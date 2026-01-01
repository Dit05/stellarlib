/* clang-format off */

/*
  stellarlib
  Copyright (C) 2026 Domán Zana

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
#include <stellarlib/ecs/ring_storage.hpp>
#include <stellarlib/ecs/sparse_map.hpp>
#include <stellarlib/ecs/sparse_set.hpp>
#include <stellarlib/ecs/sparse_storage.hpp>

#include <cstdint>
#include <memory>
#include <tuple>

namespace stellarlib::ecs
{
class world final
{
public:
	template <typename T>
	[[nodiscard]]
	static auto id_of()
	{
		return sparse_storage<world>::id_of<T>();
	}

	template <typename ...T>
	auto spawn(T ...components)
		-> std::uint32_t
	{
		std::uint32_t entity{};

		if (_queue.size()) {
			entity = *(_queue.end() - 1);
			_queue.pop();
		}
		else {
			entity = _entities.size();
		}

		bitset archetype{};

		(
			[&] -> void
			{
				_components.by_type<T>().insert(entity, components);
				archetype.insert(_components.id_of<T>());
			}(),
			...
		);

		const auto it{std::ranges::find_if(_archetypes, [&](const auto &pair) -> bool {
			return pair.first == archetype;
		})};

		_entities.insert(entity, static_cast<std::uint32_t>(it - _archetypes.begin()));

		if (it == _archetypes.end()) {
			sparse_set<std::uint32_t> set{};
			set.insert(entity);
			_archetypes.push(std::tuple<bitset, sparse_set<std::uint32_t>>{std::move(archetype), std::move(set)});
		}
		else {
			it->second.insert(entity);
		}

		return entity;
	}

	template <typename ...T>
	void insert(const std::uint32_t entity, T ...components)
	{
		const auto index{_entities.at(entity)};

		if (!index) {
			return;
		}

		_cache = _archetypes[*index].first;

		(
			[&] -> void
			{
				_components.by_type<T>().insert(entity, components);
				_cache.insert(_components.id_of<T>());
			}(),
			...
		);

		_archetypes[*index].second.erase(entity);

		for (std::uint32_t i{}; i != _archetypes.size(); ++i) {
			if (_archetypes[i].first == _cache) {
				*index = i;
				_archetypes[i].second.insert(entity);
				break;
			}
		}
	}

	[[nodiscard]]
	auto contains(std::uint32_t entity) const
		-> bool;

	template <typename T>
	[[nodiscard]]
	auto contains(const std::uint32_t entity) const
	{
		const auto set{_components.by_type<T>()};
		return set ? set->contains(entity) : false;
	}

	template <typename ...T>
	[[nodiscard]]
	auto contains(const std::uint32_t entity) const
		requires (1 < sizeof...(T))
	{
		return std::make_tuple(contains<T>(entity)...);
	}

	[[nodiscard]]
	auto at(std::uint32_t entity) const
		-> const bitset *;

	template <typename T>
	[[nodiscard]]
	auto at(const std::uint32_t entity) const
	{
		const auto set{_components.by_type<T>()};
		return set ? set->at(entity) : nullptr;
	}

	template <typename ...T>
	[[nodiscard]]
	auto at(const std::uint32_t entity) const
		requires (1 < sizeof...(T))
	{
		return std::make_tuple(get<T>(entity)...);
	}

	[[nodiscard]]
	auto operator[](std::uint32_t entity) const
		-> const bitset &;

	template <typename T>
	[[nodiscard]]
	auto operator[](const std::uint32_t entity) const
		-> T &
	{
		return (*_components.by_type<T>())[entity];
	}

	template <typename ...T>
	[[nodiscard]]
	auto operator[](const std::uint32_t entity) const
		-> std::tuple<T &...>
		requires (1 < sizeof...(T))
	{
		return std::make_tuple(operator[]<T>(entity)...);
	}

	template <typename ...T>
	void erase([[maybe_unused]] const std::uint32_t entity)
	{
		/* const auto data{_entities.at(entity)}; */
		/**/
		/* if (!data) { */
		/* 	return; */
		/* } */
		/**/
		/* ( */
		/* 	[&] -> void */
		/* 	{ */
		/* 		const auto id{_components.id_of<T>()}; */
		/**/
		/* 		if (data->contains(id)) { */
		/* 			data->erase(id); */
		/* 			_components.by_type<T>().erase(entity); */
		/* 		} */
		/* 	}(), */
		/* 	... */
		/* ); */
		/**/
		/* for (auto [archetype, set] : _caches) { */
		/* 	if (set.contains(entity) && (archetype.contains(id_of<T>()) || ...)) { */
		/* 		set.erase(entity); */
		/* 	} */
		/* } */
	}

	void despawn(std::uint32_t entity);

	[[nodiscard]]
	auto query() const
	{
		return _entities.keys();
	}

	template <typename T>
	[[nodiscard]]
	auto query()
	{
		return _components.by_type<T>().zip();
	}

	template <typename ...T>
	[[nodiscard]]
	auto query()
		requires (1 < sizeof...(T))
	{
		const auto id = ext::scoped_typeid<world, std::tuple<T...>>();
		const auto query{_queries.at(id)};

		if (!query) {
			bitset archetype{};
			(archetype.insert(id_of<T>()), ...);
			internal::stack_vector<std::size_t> indices{};

			for (std::size_t i{}; i != _archetypes.size(); ++i) {
				if (archetype <= _archetypes[i].first) {
					indices.push(i);
				}
			}

			_queries.insert(id, {archetype, indices});
			query = std::addressof(_queries[id]);
		}

		return query->second
			| std::views::transform([this](const auto index) -> internal::stack_vector<std::uint32_t> & {
				return _archetypes[index].second.keys();
			})
			| std::views::join
			| std::views::transform([this](const auto entity) -> std::tuple<std::uint32_t, T *...> {
				return {entity, std::addressof(_components.by_type<T>()[entity])...};
			});
	}

private:
	sparse_storage<world> _components;
	sparse_map<std::uint32_t, std::uint32_t> _entities;
	internal::stack_vector<std::uint32_t> _queue;
	internal::stack_vector<std::pair<bitset, sparse_set<std::uint32_t>>> _archetypes;
	sparse_map<std::size_t, std::pair<bitset, internal::stack_vector<std::size_t>>> _queries;
	static thread_local bitset _cache;
};
}

#endif
