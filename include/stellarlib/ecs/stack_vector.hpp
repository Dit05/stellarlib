/* clang-format off */

/*
  stellarlib
  Copyright (C) 2026 Domán Zana

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

#ifndef STELLARLIB_ECS_STACK_VECTOR_HPP
#define STELLARLIB_ECS_STACK_VECTOR_HPP

#include <stellarlib/ext/functional.hpp>
#include <stellarlib/ext/memory.hpp>

#include <cstddef>
#include <memory>
#include <utility>

namespace stellarlib::ecs::internal
{
template <typename T, typename size_type = std::size_t>
class stack_vector final : ext::vector_allocator<T, size_type>
{
public:
	[[nodiscard]]
	explicit constexpr stack_vector() = default;

	[[nodiscard]]
	constexpr stack_vector(const stack_vector<T, size_type> &other)
		: _size{other._size}
	{
		if (_size != 0) {
			_capacity = _size;
			ext::vector_allocator<T, size_type>::allocate(_begin, _capacity);
			std::uninitialized_copy(other._begin, other._end, _begin);
			_end = _begin + _size;
		}
	}

	[[nodiscard]]
	constexpr stack_vector(stack_vector<T, size_type> &&other)
		: _capacity{other._capacity}
		, _size{other._size}
		, _begin{other._begin}
		, _end{other._end}
	{
		other._end = nullptr;
		other._begin = nullptr;
		other._capacity = 0;
	}

	constexpr auto operator=(const stack_vector<T, size_type> &other)
		-> stack_vector<T, size_type> &
	{
		if (std::addressof(other) == this) {
			return *this;
		}

		std::ranges::destroy(*this);
		_size = other._size;

		if (_capacity < _size) {
			ext::vector_allocator<T, size_type>::deallocate(_begin);
			_capacity = _size;
			ext::vector_allocator<T, size_type>::allocate(_begin, _capacity);
		}

		std::uninitialized_copy(other._begin, other._end, _begin);
		_end = _begin + _size;
		return *this;
	}

	constexpr auto operator=(stack_vector<T, size_type> &&other)
		-> stack_vector<T, size_type> &
	{
		if (std::addressof(other) != this) {
			std::destroy_at(this);
			std::construct_at(this, std::move(other));
		}

		return *this;
	}

	constexpr ~stack_vector()
	{
		std::ranges::destroy(*this);
		ext::vector_allocator<T, size_type>::deallocate(_begin);
	}

	template <typename ...Args>
	constexpr auto extend(const size_type size, Args &&...args)
	{
		if (size <= _size) {
			return false;
		}

		if (_capacity < size) {
			_capacity = size;
			ext::vector_allocator<T, size_type>::reallocate(_begin, _size, _capacity);
		}

		_end = _begin + size;
		 std::uninitialized_fill(_begin + _size, _end, T{std::forward<Args>(args)...});
		_size = size;
		return true;
	}

	template <typename ...Args>
	constexpr void push(Args &&...args)
	{
		if (_size == _capacity) {
			++_capacity;
			ext::vector_allocator<T, size_type>::reallocate(_begin, _size, _capacity);
			_end = _begin + _size;
		}

		std::construct_at(_end, std::forward<Args>(args)...);
		++_end;
		++_size;
	}

	[[nodiscard]]
	constexpr auto size() const
	{
		return _size;
	}

	[[nodiscard]]
	constexpr auto operator[](const size_type i) const
		-> T &
	{
		return _begin[i];
	}

	[[nodiscard]]
	constexpr auto begin() const
	{
		return _begin;
	}

	[[nodiscard]]
	constexpr auto end() const
	{
		return _end;
	}

	constexpr void pop()
	{
		--_size;
		--_end;
		std::destroy_at(_end);
	}

	constexpr void clear()
	{
		_size = 0;
		std::ranges::destroy(*this);
		_end = _begin;
	}

private:
	size_type _capacity{};
	size_type _size{};
	T *_begin{};
	T *_end{};
};
}

#endif
