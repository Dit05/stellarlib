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

#include <cstddef>
#include <cstdint>
#include <utility>

namespace stellarlib::ecs
{
class world final
{
public:
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

private:
	internal::sparse_storage _components;
	internal::stack_vector<std::size_t, std::uint32_t> _entities;
	internal::stack_vector<std::pair<internal::bitset, internal::sparse_set<std::uint32_t>>> _archetypes;
	internal::sparse_map<std::size_t, std::pair<internal::bitset, internal::stack_vector<std::size_t>>> _queries;
	internal::stack_vector<std::uint32_t> _queue;
	internal::bitset _bitset;
};
}

#endif
