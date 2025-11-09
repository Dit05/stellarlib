/* clang-format off */

/*
  stellarlib
  Copyright (C) 2025 Domán Zana

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
#include <stellarlib/ecs/sparse_set.hpp>
#include <stellarlib/ecs/sparse_storage.hpp>

#include <cstdint>
#include <tuple>

namespace stellarlib::ecs
{
class world final
{
public:
	template <typename ...T>
	[[nodiscard]]
	auto spawn(T ...components)
		-> std::uint32_t
	{
		const auto entity{_entities.acquire()};

		(
			[&] -> void
			{
				const auto id{_components.id_of<T>()};
				_entities[entity].insert(id);
				_components.by_type<T>().insert(entity, components);
			}(),
			...
		);

		return entity;
	}

	template <typename ...T>
	void insert(const std::uint32_t entity, T ...components)
	{
		const auto data{_entities.at(entity)};

		if (!data) {
			return;
		}

		(
			[&] -> void
			{
				data->insert(_components.id_of<T>());
				_components.by_type<T>().insert(entity, components);
			}(),
			...
		);
	}

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

	template <typename T>
	[[nodiscard]]
	auto get(const std::uint32_t entity) const
	{
		const auto set{_components.by_type<T>()};
		return set ? set->at(entity) : nullptr;
	}

	template <typename ...T>
	[[nodiscard]]
	auto get(const std::uint32_t entity) const
		requires (1 < sizeof...(T))
	{
		return std::make_tuple(get<T>(entity)...);
	}

	template <typename ...T>
	void erase(const std::uint32_t entity)
	{
		const auto data{_entities.at(entity)};

		if (!data) {
			return;
		}

		(
			[&] -> void
			{
				const auto id{_components.id_of<T>()};

				if (data->contains(id)) {
					data->erase(id);
					_components.by_type<T>().erase(entity);
				}
			}(),
			...
		);
	}

	void despawn(const std::uint32_t entity)
	{
		if (!_entities.contains(entity)) {
			return;
		}

		_entities.release(entity);

		for (auto &set : _components.sets()) {
			set.erase(entity);
		}
	}

private:
	ring_storage<bitset> _entities;
	sparse_storage _components;
};
}

#endif
