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
#include <stellarlib/ext/utility.hpp>

#include <array>
#include <cstdint>
#include <memory>
#include <type_traits>

namespace stellarlib::ecs::internal
{
class sparse_storage final
{
public:
	template <typename ...T>
	[[nodiscard]]
	static constexpr auto ids() noexcept
		-> std::conditional_t<1 < sizeof...(T), const std::array<std::uint16_t, sizeof...(T)> &, std::array<std::uint16_t, sizeof...(T)>>
	{
		if constexpr (1 < sizeof...(T)) {
			static const std::array<std::uint16_t, sizeof...(T)> cache{ext::scoped_typeid<sparse_storage, T, std::uint16_t>()...};
			return cache;
		}
		else {
			return {ext::scoped_typeid<sparse_storage, T, std::uint16_t>()...};
		}
	}

	[[nodiscard]]
	explicit constexpr sparse_storage() noexcept = default;

	[[nodiscard]]
	sparse_storage(const sparse_storage &other) noexcept;

	[[nodiscard]]
	constexpr sparse_storage(sparse_storage &&) noexcept = default;

	auto operator=(const sparse_storage &other) noexcept
		-> sparse_storage &;

	constexpr auto operator=(sparse_storage &&) noexcept
		-> sparse_storage & = default;

	constexpr ~sparse_storage() noexcept = default;

	template <typename T>
	[[nodiscard]]
	constexpr auto at(const std::uint16_t id) noexcept
		-> sparse_map<std::uint32_t, T> &
	{
		if (const auto map{_maps.at(id)}) {
			return static_cast<sparse_map<std::uint32_t, T> &>(**map);
		}

		_maps.insert(id, std::make_unique<sparse_map<std::uint32_t, T>>());
		return static_cast<sparse_map<std::uint32_t, T> &>(*_maps.values().back());
	}

	template <typename T>
	[[nodiscard]]
	constexpr auto operator[](const std::uint16_t id) const noexcept
		-> sparse_map<std::uint32_t, T> &
	{
		return static_cast<sparse_map<std::uint32_t, T> &>(*_maps[id]);
	}

	void erase(std::uint32_t key) const noexcept;

	void clear() const noexcept;

private:
	sparse_map<std::uint16_t, std::unique_ptr<any_set<std::uint32_t>>> _maps;
};
}

#endif
