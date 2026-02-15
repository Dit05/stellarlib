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

#include <stellarlib/ecs/bitset.hpp>

#include <stellarlib/ext/functional.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <ranges>

namespace stellarlib::ecs
{
bitset::bitset(const bitset &other)
	: _size{other._size}
{
	if (ext::truthy(_size)) {
		_begin.reset(reinterpret_cast<std::uintmax_t *>(std::malloc(_size * sizeof(std::uintmax_t))));
		std::ranges::copy(other.segments(), _begin.get());
		_end = _begin.get() + _size;
	}
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

	std::ranges::copy(other.segments(), _begin.get());
	std::fill(_begin.get() + other._size, _end, 0);
	return *this;
}

void bitset::insert(const std::uintmax_t elem)
{
	const auto index{index_of(elem)};

	if (index < _size) {
		_begin.get()[index] |= mask_of(elem);
		return;
	}

	realloc(index + 1);

	for (; _size != index; ++_size) {
		_begin.get()[_size] = 0;
	}

	++_size;
	_begin.get()[index] = mask_of(elem);
	_end = _begin.get() + _size;
}

auto bitset::contains(const std::uintmax_t elem) const
	-> bool
{
	const auto index{index_of(elem)};
	return index < _size && ext::truthy((_begin.get()[index] & mask_of(elem)));
}

auto bitset::operator==(const bitset &other) const
	-> bool
{
	if (_size < other._size) {
		return std::equal(_begin.get(), _end, other._begin.get()) && std::none_of(other._begin.get() + _size, other._end, ext::truthy<std::uintmax_t>);
	}

	if (other._size < _size) {
		return std::equal(other._begin.get(), other._end, _begin.get()) && std::none_of(_begin.get() + other._size, _end, ext::truthy<std::uintmax_t>);
	}

	return std::equal(_begin.get(), _end, other._begin.get());
}

auto bitset::operator<=(const bitset &other) const
	-> bool
{
	return (_size <= other._size || std::none_of(_begin.get() + other._size, _end, ext::truthy<std::uintmax_t>))
		&& std::ranges::all_of(std::views::zip(segments(), other.segments()), ext::zip::subset<std::uintmax_t>);
}

auto bitset::operator>=(const bitset &other) const
	-> bool
{
	return other <= *this;
}

void bitset::erase(const std::uintmax_t elem)
{
	const auto index{index_of(elem)};

	if (index < _size) {
		_begin.get()[index] &= ~mask_of(elem);
	}
}

void bitset::clear()
{
	std::ranges::fill(segments(), 0);
}

auto bitset::index_of(const std::uintmax_t elem)
	-> std::uintmax_t
{
	return elem / std::numeric_limits<std::uintmax_t>::digits;
}

auto bitset::mask_of(const std::uintmax_t elem)
	-> std::uintmax_t
{
	return std::uintmax_t{1} << elem % std::numeric_limits<std::uintmax_t>::digits;
}

auto bitset::segments() const
	-> std::ranges::subrange<std::uintmax_t *, std::uintmax_t *>
{
	return std::ranges::subrange{_begin.get(), _end};
}

void bitset::realloc(const std::uintmax_t size)
{
	_begin.reset(reinterpret_cast<std::uintmax_t *>(std::realloc(_begin.release(), size * sizeof(std::uintmax_t))));
}
}
