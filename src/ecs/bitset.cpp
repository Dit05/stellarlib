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

#include <stellarlib/ecs/bitset.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <new>
#include <ranges>

namespace stellarlib::ecs
{
bitset::bitset(const bitset &other)
	: _size{other._size}
{
	if (_size == 0) {
		return;
	}

	_begin.reset(static_cast<std::uint32_t *>(std::malloc(_size * sizeof(*_begin))));

	if (!_begin) {
		throw std::bad_alloc{};
	}

	std::copy(other._begin.get(), other._end, _begin.get());
	_end = _begin.get() + _size;
}

auto bitset::operator=(const bitset &other)
	-> bitset &
{
	if (std::addressof(other) == this) {
		return *this;
	}

	if (_size < other._size) {
		_size = other._size;
		realloc(_size);
		_end = _begin.get() + _size;
	}

	std::copy(other._begin.get(), other._end, _begin.get());

	for (auto &segment : std::ranges::subrange(_begin.get() + other._size, _end)) {
		segment = 0;
	}

	return *this;
}

void bitset::insert(const std::uint32_t key)
{
	const auto index{index_of(key)};

	if (index < _size) {
		_begin.get()[index] |= mask_of(key);
	}
	else {
		realloc(index + 1);

		for (; _size != index; ++_size) {
			_begin.get()[_size] = 0;
		}

		++_size;
		_begin.get()[index] = mask_of(key);
		_end = _begin.get() + _size;
	}
}

auto bitset::contains(const std::uint32_t key) const noexcept
	-> bool
{
	const auto index{index_of(key)};

	return index < _size && (_begin.get()[index] & mask_of(key)) != 0;
}

void bitset::erase(const std::uint32_t key) noexcept
{
	const auto index{index_of(key)};

	if (index < _size) {
		_begin.get()[index] &= ~mask_of(key);
	}
}

void bitset::clear() noexcept
{
	for (auto &segment : range()) {
		segment = 0;
	}
}

auto bitset::index_of(const std::uint32_t key) noexcept
	-> std::uint32_t
{
	return key / std::numeric_limits<std::uint32_t>::digits;
}

auto bitset::mask_of(const std::uint32_t key) noexcept
	-> std::uint32_t
{
	return std::uint32_t{1} << key % std::numeric_limits<std::uint32_t>::digits;
}

void bitset::realloc(const std::uint32_t size)
{
	_begin.reset(static_cast<std::uint32_t *>(std::realloc(_begin.release(), size * sizeof(*_begin))));

	if (!_begin) {
		throw std::bad_alloc{};
	}
}

auto bitset::range() const noexcept
	-> std::ranges::subrange<std::uint32_t *, std::uint32_t *>
{
	return std::ranges::subrange{_begin.get(), _end};
}
}
