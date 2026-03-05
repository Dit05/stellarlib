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

#include <stellarlib/ecs/archetype.hpp>

#include <cstdint>
#include <memory>

namespace stellarlib::ecs
{
auto world::contains(const std::uint32_t entity) const noexcept
	-> bool
{
	return _entities.contains(entity);
}

auto world::at(const std::uint32_t entity) const noexcept
	-> const archetype *
{
	if (const auto id{_entities.at(entity)}) {
		return std::addressof(_archetypes[*id].first);
	}

	return nullptr;
}

auto world::operator[](const std::uint32_t entity) const noexcept
	-> const archetype &
{
	return _archetypes[_entities[entity]].first;
}

void world::despawn(const std::uint32_t entity) noexcept
{
	if (const auto id{_entities.at(entity)}) {
		_entities.erase(entity);
		_archetypes[*id].second.erase(entity);
		_components.erase(entity);
		_queue.push(entity);
	}
}

void world::clear() noexcept
{
	_queue.clear();
	_entities.clear();

	for (auto &pair : _archetypes) {
		pair.second.clear();
	}

	_components.clear();
}

thread_local archetype world::cache{};
}
