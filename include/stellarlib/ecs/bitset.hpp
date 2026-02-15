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
	explicit bitset() = default;

	[[nodiscard]]
	bitset(const bitset &other);

	[[nodiscard]]
	bitset(bitset &&) = default;

	auto operator=(const bitset &other)
		-> bitset &;

	auto operator=(bitset &&)
		-> bitset & = default;

	~bitset() = default;

	void insert(std::uintmax_t elem);

	[[nodiscard]]
	auto contains(std::uintmax_t elem) const
		-> bool;

	[[nodiscard]]
	auto operator==(const bitset &other) const
		-> bool;

	[[nodiscard]]
	auto operator<=(const bitset &other) const
		-> bool;

	[[nodiscard]]
	auto operator>=(const bitset &other) const
		-> bool;

	void erase(std::uintmax_t elem);

	void clear();

private:
	std::uintmax_t _size{};
	std::unique_ptr<std::uintmax_t, void (*)(void *)> _begin{nullptr, std::free};
	std::uintmax_t *_end{};

	[[nodiscard]]
	static auto index_of(std::uintmax_t elem)
		-> std::uintmax_t;

	[[nodiscard]]
	static auto mask_of(std::uintmax_t elem)
		-> std::uintmax_t;

	[[nodiscard]]
	auto segments() const
		-> std::ranges::subrange<std::uintmax_t *, std::uintmax_t *>;

	void realloc(std::uintmax_t size);
};
}

#endif
