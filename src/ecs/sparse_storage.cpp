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

#include <stellarlib/ecs/sparse_storage.hpp>

#include <cstdint>
#include <memory>

namespace stellarlib::ecs::internal
{
sparse_storage::sparse_storage(const sparse_storage &other) noexcept
{
	for (const auto [id, set] : other._maps.zip()) {
		_maps.insert(id, set->clone());
	}
}

auto sparse_storage::operator=(const sparse_storage &other) noexcept
	-> sparse_storage &
{
	if (std::addressof(other) == this) {
		return *this;
	}

	_maps.clear();

	for (const auto [id, set] : other._maps.zip()) {
		_maps.insert(id, set->clone());
	}

	return *this;
}

void sparse_storage::erase(const std::uint32_t key) const noexcept
{
	for (const auto &map : _maps.values()) {
		map->erase(key);
	}
}

void sparse_storage::clear() const noexcept
{
	for (const auto &map : _maps.values()) {
		map->clear();
	}
}
}
