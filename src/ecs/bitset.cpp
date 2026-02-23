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

#include <stellarlib/ext/bit.hpp>
#include <stellarlib/ext/functional.hpp>
#include <stellarlib/ext/memory.hpp>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <ranges>
#include <utility>

namespace stellarlib::ecs::internal
{
bitset::bitset(const bitset &other) noexcept
	: _size{other._size}
{
	if (ext::truthy(_size)) {
		_capacity = _size;
		ext::vector_allocator<std::uintmax_t>::allocate(_begin, _capacity);
		_end = _begin + _size;
		std::copy(other._begin, other._end, _begin);
	}
}

bitset::bitset(bitset &&other) noexcept
	: _size{other._size}
	, _capacity{other._capacity}
	, _begin{other._begin}
	, _end{other._end}
{
	other._begin = nullptr;
}

auto bitset::operator=(const bitset &other) noexcept
	-> bitset &
{
	if (std::addressof(other) == this) {
		return *this;
	}

	_size = other._size;

	if (_capacity < _size) {
		_capacity = _size;
		ext::vector_allocator<std::uintmax_t>::reallocate(_begin, _capacity);
	}

	_end = _begin + _size;
	std::copy(other._begin, other._end, _begin);
	return *this;
}

auto bitset::operator=(bitset &&other) noexcept
	-> bitset &
{
	if (std::addressof(other) != this) {
		std::destroy_at(this);
		std::construct_at(this, std::move(other));
	}

	return *this;
}

bitset::~bitset() noexcept
{
	ext::vector_allocator<std::uintmax_t>::deallocate(_begin);
}

void bitset::insert(const std::uintmax_t bit) noexcept
{
	if (ext::bit_index(bit) < _size) {
		_begin[ext::bit_index(bit)] |= ext::bit_mask(bit);
		return;
	}

	if (_capacity <= ext::bit_index(bit)) {
		_capacity = ext::bit_index(bit) + 1;
		ext::vector_allocator<std::uintmax_t>::reallocate(_begin, _capacity);
		std::fill(_begin + _size, _begin + ext::bit_index(bit), 0);
	}

	_size = ext::bit_index(bit) + 1;
	_end = _begin + _size;
	_begin[ext::bit_index(bit)] = ext::bit_mask(bit);
}

auto bitset::contains(const std::uintmax_t bit) const noexcept
	-> bool
{
	return ext::bit_index(bit) < _size && ext::truthy(_begin[ext::bit_index(bit)] & ext::bit_mask(bit));
}

auto bitset::operator==(const bitset &other) const noexcept
	-> bool
{
	return std::equal(_begin, _end, other._begin, other._end);
}

auto bitset::operator<=(const bitset &other) const noexcept
	-> bool
{
	return _size <= other._size && std::ranges::all_of(std::views::zip(std::ranges::subrange{_begin, _end}, std::ranges::subrange{other._begin, other._end}), ext::zip::subset<std::uintmax_t>);
}

auto bitset::operator>=(const bitset &other) const noexcept
	-> bool
{
	return other <= *this;
}

void bitset::erase(const std::uintmax_t bit) noexcept
{
	_begin[ext::bit_index(bit)] &= ~ext::bit_mask(bit);

	for (; ext::falsy(_begin[_size - 1]) && 0 < _size; --_size) {}

	_end = _begin + _size;
}

void bitset::clear() noexcept
{
	std::fill(_begin, _end, 0);
	_end = _begin;
	_size = 0;
}
}
