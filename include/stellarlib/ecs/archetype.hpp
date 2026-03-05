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

#ifndef STELLARLIB_ECS_ARCHETYPE_HPP
#define STELLARLIB_ECS_ARCHETYPE_HPP

#include <stellarlib/ecs/sparse_storage.hpp>
#include <stellarlib/ext/bit.hpp>
#include <stellarlib/ext/memory.hpp>

#include <cstdint>
#include <type_traits>

namespace stellarlib::ecs
{
class archetype final : ext::vector_allocator<std::uintmax_t>
{
public:
	template <typename ...T>
	[[nodiscard]]
	static constexpr auto of() noexcept
		-> std::conditional_t<0 < sizeof...(T), const archetype &, archetype>
	{
		if constexpr (0 < sizeof...(T)) {
			static const auto cache{[] -> archetype {
				archetype archetype{};

				for (const auto id : internal::sparse_storage::ids<T...>()) {
					archetype.insert(ext::bit_index(id), ext::bit_mask(id));
				}

				archetype._end = archetype._begin + archetype._size;
				return archetype;
			}()};

			return cache;
		}
		else {
			return archetype{};
		}
	}

	[[nodiscard]]
	explicit constexpr archetype() noexcept = default;

	[[nodiscard]]
	archetype(const archetype &other) noexcept;

	[[nodiscard]]
	archetype(archetype &&other) noexcept;

	auto operator=(const archetype &other) noexcept
		-> archetype &;

	auto operator=(archetype &&other) noexcept
		-> archetype &;

	~archetype() noexcept;

	void insert(std::uintmax_t id) noexcept;

	void insert(const archetype &other) noexcept;

	[[nodiscard]]
	auto contains(std::uintmax_t id) const noexcept
		-> bool;

	[[nodiscard]]
	auto operator==(const archetype &other) const noexcept
		-> bool;

	[[nodiscard]]
	auto operator<=(const archetype &other) const noexcept
		-> bool;

	[[nodiscard]]
	auto operator>=(const archetype &other) const noexcept
		-> bool;

	void erase(std::uintmax_t id) noexcept;

	void erase(const archetype &other) noexcept;

	void clear() noexcept;

private:
	std::uintmax_t _size{};
	std::uintmax_t _capacity{};
	std::uintmax_t *_begin{};
	std::uintmax_t *_end{};

	void insert(std::uintmax_t index, std::uintmax_t mask) noexcept;
};
}

#endif
