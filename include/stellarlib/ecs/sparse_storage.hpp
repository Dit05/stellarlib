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

#ifndef STELLARLIB_ECS_SPARSE_STORAGE_HPP
#define STELLARLIB_ECS_SPARSE_STORAGE_HPP

#include <stellarlib/ecs/any_set.hpp>
#include <stellarlib/ecs/sparse_map.hpp>
#include <stellarlib/ecs/stack_vector.hpp>
#include <stellarlib/ext/utility.hpp>

#include <cstdint>
#include <memory>

namespace stellarlib::ecs::internal
{
template <typename Scope>
class sparse_storage final
{
public:
	template <typename T>
	[[nodiscard]]
	static constexpr auto id() noexcept
	{
		return ext::scoped_typeid<Scope, T>();
	}

	[[nodiscard]]
	explicit constexpr sparse_storage() noexcept = default;

	[[nodiscard]]
	constexpr sparse_storage(const sparse_storage &other) noexcept
	{
		for (const auto &set : other._maps) {
			_maps.push(set ? set->clone() : nullptr);
		}
	}

	[[nodiscard]]
	constexpr sparse_storage(sparse_storage &&) noexcept = default;

	constexpr auto operator=(const sparse_storage &other) noexcept
		-> sparse_storage &
	{
		if (std::addressof(other) == this) {
			return *this;
		}

		_maps.clear();

		for (const auto &set : other._maps) {
			_maps.push(set ? set->clone() : nullptr);
		}

		return *this;
	}

	constexpr auto operator=(sparse_storage &&) noexcept
		-> sparse_storage & = default;

	constexpr ~sparse_storage() noexcept = default;

	template <typename T>
	[[nodiscard]]
	constexpr auto get() noexcept
		-> sparse_map<std::uint32_t, T> &
	{
		if (_maps.extend(id<T>() + 1) || !_maps[id<T>()]) {
			_maps[id<T>()] = std::make_unique<sparse_map<std::uint32_t, T>>();
		}

		return static_cast<sparse_map<std::uint32_t, T> &>(*_maps[id<T>()]);
	}

	constexpr void erase(const std::uint32_t key) const noexcept
	{
		for (const auto &map : _maps) {
			map->erase(key);
		}
	}

	constexpr void clear() const noexcept
	{
		for (const auto &map : _maps) {
			map->clear();
		}
	}

private:
	stack_vector<std::unique_ptr<any_set<std::uint32_t>>> _maps;
};
}

#endif
