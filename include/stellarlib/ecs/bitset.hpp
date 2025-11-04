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

#include <cstddef>
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

	void insert(std::size_t key);

	[[nodiscard]]
	auto contains(std::size_t key) const
		-> bool;

	[[nodiscard]]
	auto operator==(const bitset &other) const
		-> bool;

	[[nodiscard]]
	auto operator!=(const bitset &other) const
		-> bool;

	[[nodiscard]]
	auto operator<=(const bitset &other) const
		-> bool;

	[[nodiscard]]
	auto operator>=(const bitset &other) const
		-> bool;

	void erase(std::size_t key);

	void reset();

private:
	std::size_t                                     _size{};
	[[maybe_unused]] std::size_t                    _padding{};
	std::unique_ptr<std::size_t, void (*)(void *)>  _begin{nullptr, std::free};
	std::size_t                                    *_end{};

	[[nodiscard]]
	static auto index_of(std::size_t key)
		-> std::size_t;

	[[nodiscard]]
	static auto mask_of(std::size_t key)
		-> std::size_t;

	void realloc(std::size_t size);

	[[nodiscard]]
	auto range() const
		-> std::ranges::subrange<std::size_t *, std::size_t *>;
};
}

#endif
