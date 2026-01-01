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

#include <stellarlib/ecs/world.hpp>

#include <stellarlib/ecs/bitset.hpp>
#include <stellarlib/ext/functional.hpp>

#include <cstdint>
#include <memory>

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
	const auto index{_entities.at(entity)};
	return ext::truthy(index) ? std::addressof(_archetypes[*index].first) : nullptr;
}

auto world::operator[](const std::uint32_t entity) const
	-> const bitset &
{
	return _archetypes[_entities[entity]].first;
}

void world::despawn(const std::uint32_t entity)
{
	const auto index{_entities.at(entity)};

	if (ext::falsy(index)) {
		return;
	}

	for (auto &set : _components.sets()) {
		set.erase(entity);
	}

	_archetypes[*index].second.erase(entity);
	_entities.erase(entity);
	_queue.push(entity);
}

thread_local bitset world::_cache{};
}
