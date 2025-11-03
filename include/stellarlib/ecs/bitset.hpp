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

#ifndef STELLARLIB_ECS_BITSET_HPP
#define STELLARLIB_ECS_BITSET_HPP

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <ranges>

namespace stellarlib::ecs
{
class bitset final
{
public:
	[[nodiscard]]
	explicit bitset() noexcept = default;

	[[nodiscard]]
	bitset(const bitset &other);

	[[nodiscard]]
	bitset(bitset &&) noexcept = default;

	auto operator=(const bitset &other)
		-> bitset &;

	auto operator=(bitset &&) noexcept
		-> bitset & = default;

	~bitset() noexcept = default;

	void insert(std::uint32_t key);

	[[nodiscard]]
	auto contains(std::uint32_t key) const noexcept
		-> bool;

	[[nodiscard]]
	auto operator==(const bitset &other) const noexcept
		-> bool;

	[[nodiscard]]
	auto operator!=(const bitset &other) const noexcept
		-> bool;

	[[nodiscard]]
	auto operator<=(const bitset &other) const noexcept
		-> bool;

	[[nodiscard]]
	auto operator>=(const bitset &other) const noexcept
		-> bool;

	void erase(std::uint32_t key) noexcept;

	void clear() noexcept;

private:
	std::uint32_t                                     _size{};
	[[maybe_unused]] std::uint32_t                    _padding{};
	std::unique_ptr<std::uint32_t, void (*)(void *)>  _begin{nullptr, std::free};
	std::uint32_t                                    *_end{};

	[[nodiscard]]
	static auto index_of(std::uint32_t key) noexcept
		-> std::uint32_t;

	[[nodiscard]]
	static auto mask_of(std::uint32_t key) noexcept
		-> std::uint32_t;

	void realloc(std::uint32_t size);

	[[nodiscard]]
	auto range() const noexcept
		-> std::ranges::subrange<std::uint32_t *, std::uint32_t *>;
};
}

#endif
