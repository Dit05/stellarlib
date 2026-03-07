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

#include <stellarlib/ecs/archetype.hpp>

#include <stellarlib/ext/bit.hpp>
#include <stellarlib/ext/functional.hpp>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <ranges>
#include <utility>

namespace stellarlib::ecs
{
archetype::archetype() noexcept = default;

archetype::archetype(const archetype &other) noexcept
	: _size{other._size}
{
	if (ext::truthy(_size)) {
		_capacity = _size;
		allocate(_begin, _capacity);
		_end = _begin + _size;
		std::copy(other._begin, other._end, _begin);
	}
}

archetype::archetype(archetype &&other) noexcept
	: _size{std::exchange(other._size, {})}
	, _capacity{std::exchange(other._capacity, {})}
	, _begin{std::exchange(other._begin, {})}
	, _end{std::exchange(other._end, {})}
{}

auto archetype::operator=(const archetype &other) noexcept
	-> archetype &
{
	if (std::addressof(other) == this) {
		return *this;
	}

	_size = other._size;

	if (_capacity < _size) {
		_capacity = _size;
		reallocate(_begin, _capacity);
	}

	_end = _begin + _size;
	std::fill(_end, _begin + _capacity, 0);
	std::copy(other._begin, other._end, _begin);
	return *this;
}

auto archetype::operator=(archetype &&other) noexcept
	-> archetype &
{
	if (std::addressof(other) != this) {
		std::destroy_at(this);
		std::construct_at(this, std::move(other));
	}

	return *this;
}

archetype::~archetype() noexcept
{
	deallocate(_begin);
}

void archetype::insert(const std::uintmax_t id) noexcept
{
	if (ext::bit_index(id) < _size) {
		_begin[ext::bit_index(id)] |= ext::bit_mask(id);
		return;
	}

	if (_capacity <= ext::bit_index(id)) {
		_capacity = ext::bit_index(id) + 1;
		reallocate(_begin, _capacity);
		_end = _begin + _size;
	}

	_begin[ext::bit_index(id)] = ext::bit_mask(id);
	std::fill(_end, _begin + ext::bit_index(id), 0);
	_size = ext::bit_index(id) + 1;
	_end = _begin + _size;
}

void archetype::insert(const archetype &other) noexcept
{
	for (const auto [lhs, rhs] : std::views::zip(std::ranges::subrange{_begin, _end}, std::ranges::subrange{other._begin, other._end})) {
		lhs |= rhs;
	}

	if (_capacity < other._size) {
		_capacity = other._size;
		reallocate(_begin, _capacity);
	}

	while (_size < other._size) {
		_begin[_size++] = other._begin[_size];
	}

	_end = _begin + _size;
}

auto archetype::contains(const std::uintmax_t id) const noexcept
	-> bool
{
	return ext::bit_index(id) < _size && ext::truthy(_begin[ext::bit_index(id)] & ext::bit_mask(id));
}

auto archetype::operator==(const archetype &other) const noexcept
	-> bool
{
	return std::equal(_begin, _end, other._begin, other._end);
}

auto archetype::operator<=(const archetype &other) const noexcept
	-> bool
{
	return _size <= other._size && std::ranges::all_of(std::views::zip(std::ranges::subrange{_begin, _end}, std::ranges::subrange{other._begin, other._end}), ext::zip::subset<std::uintmax_t>);
}

auto archetype::operator>=(const archetype &other) const noexcept
	-> bool
{
	return other <= *this;
}

void archetype::erase(const std::uintmax_t id) noexcept
{
	if (_size <= ext::bit_index(id)) {
		return;
	}

	_begin[ext::bit_index(id)] &= ~ext::bit_mask(id);

	while (ext::falsy(_begin[_size - 1])) {
		if (ext::falsy(--_size)) {
			break;
		}
	}

	_end = _begin + _size;
}

void archetype::erase(const archetype &other) noexcept
{
	for (const auto [lhs, rhs] : std::views::zip(std::ranges::subrange{_begin, _end}, std::ranges::subrange{other._begin, other._end})) {
		lhs &= ~rhs;
	}

	while (ext::truthy(_size) && ext::falsy(_begin[_size - 1])) {
		--_size;
	}

	_end = _begin + _size;
}

void archetype::clear() noexcept
{
	std::fill(_begin, _end, 0);
	_end = _begin;
	_size = 0;
}
}
