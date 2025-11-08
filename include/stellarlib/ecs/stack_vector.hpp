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

#ifndef STELLARLIB_ECS_STACK_VECTOR_HPP
#define STELLARLIB_ECS_STACK_VECTOR_HPP

#include <cstddef>
#include <memory>
#include <ranges>
#include <utility>

namespace stellarlib::ecs
{
template <typename T, typename size_type = std::size_t>
class stack_vector final : std::allocator<T>
{
public:
	[[nodiscard]]
	explicit constexpr stack_vector() = default;

	[[nodiscard]]
	constexpr stack_vector(const stack_vector<T, size_type> &other)
		: _capacity{other._capacity}
	{
		if (_capacity == 0) {
			return;
		}

		_begin = std::allocator<T>::allocate(_capacity);
		_size = other._size;
		_end = _begin + _size;

		for (auto [dst, src] : std::views::zip(*this, other)) {
			new (std::addressof(dst)) T{src};
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

		for (auto &value : *this) {
			value.~T();
		}

		if (_capacity < other._size) {
			std::allocator<T>::deallocate(_begin, _capacity);
			_capacity = other._capacity;
			_begin = std::allocator<T>::allocate(_capacity);
		}

		_size = other._size;
		_end = _begin + _size;

		for (auto [dst, src] : std::views::zip(*this, other)) {
			new (std::addressof(dst)) T{src};
		}

		return *this;
	}

	constexpr auto operator=(stack_vector<T, size_type> &&other)
		-> stack_vector<T, size_type> &
	{
		if (std::addressof(other) != this) {
			this->~stack_vector();
			new (this) stack_vector<T>{std::move(other)};
		}

		return *this;
	}

	constexpr ~stack_vector()
	{
		for (auto &value : *this) {
			value.~T();
		}

		std::allocator<T>::deallocate(_begin, _capacity);
	}

	constexpr auto extend(const size_type size)
	{
		if (size <= _size) {
			return false;
		}

		if (_capacity < size) {
			realloc(size);
		}

		for (; _size != size; ++_size) {
			new (_begin + _size) T{};
		}

		_end = _begin + _size;
		return true;
	}

	template <typename ...Args>
	constexpr void push(Args &&...args)
	{
		if (_size < _capacity) {
			new (_end) T{std::forward<Args>(args)...};
			++_size;
			++_end;
		}
		else {
			realloc(_capacity + 1);
			new (_begin + _size) T{std::forward<Args>(args)...};
			_size = _capacity;
			_end = _begin + _size;
		}
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
		--_end;
		_end->~T();
		--_size;
	}

private:
	size_type _capacity{};
	size_type _size{};
	T *_begin{};
	T *_end{};

	constexpr void realloc(const size_type capacity)
	{
		auto tmp{std::allocator<T>::allocate(capacity)};

		for (auto dst{tmp}, src{_begin}; src != _end; ++dst, ++src) {
			new (dst) T{std::move(*src)};
			src->~T();
		}

		std::allocator<T>::deallocate(_begin, _capacity);
		_capacity = capacity;
		_begin = tmp;
	}
};
}

#endif
