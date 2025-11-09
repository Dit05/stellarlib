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

#include <stellarlib/ecs/world.hpp>

#include <stellarlib/ecs/bitset.hpp>

#include <cstdint>

namespace stellarlib::ecs
{
auto world::contains(const std::uint32_t entity) const
	-> bool
{
	return _entities.contains(entity);
}

auto world::at(const std::uint32_t entity) const
	-> const bitset *
{
	return _entities.at(entity);
}

auto world::operator[](const std::uint32_t entity) const
	-> const bitset &
{
	return _entities[entity];
}

void world::despawn(const std::uint32_t entity)
{
	if (!_entities.contains(entity)) {
		return;
	}

	_entities.release(entity);

	for (auto &set : _components.sets()) {
		set.erase(entity);
	}
}
}
