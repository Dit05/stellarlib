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
#include <cstdlib>
#include <memory>
#include <ranges>
#include <utility>

namespace stellarlib::ecs
{
template <typename T, typename Allocator = std::allocator<T>>
class stack_vector final
{
public:
	[[nodiscard]]
	explicit stack_vector() noexcept = default;

	[[nodiscard]]
	explicit stack_vector(const Allocator &allocator)
		: _allocator{allocator} { }

	[[nodiscard]]
	stack_vector(const stack_vector<T, Allocator> &other)
		: _capacity{other._capacity}
	{
		if (!_capacity) {
			return;
		}

		_begin = _allocator.allocate(_capacity);

		_size = other._size;
		_end  = _begin + _size;

		for (auto [dst, src] : std::ranges::views::zip(*this, other)) {
			new (&dst) T{src};
		}
	}

	[[nodiscard]]
	stack_vector(stack_vector<T, Allocator> &&other)
		: _allocator{std::move(other._allocator)}
		, _capacity{other._capacity}
		, _begin{other._begin}
		, _size{other._size}
		, _end{other._end}
	{
		other._end      = nullptr;
		other._size     = 0;
		other._begin    = nullptr;
		other._capacity = 0;
	}

	auto operator=(const stack_vector<T, Allocator> &other)
		-> stack_vector<T, Allocator> &
	{
		if (&other == this) {
			return *this;
		}

		for (auto &value : *this) {
			value.~T();
		}

		if (_capacity < other._size) {
			_allocator.deallocate(_begin, _capacity);
			_capacity = other._capacity;
			_begin    = _allocator.allocate(_capacity);
		}

		_size = other._size;
		_end  = _begin + _size;

		for (auto [dst, src] : std::ranges::views::zip(*this, other)) {
			new (&dst) T{src};
		}

		return *this;
	}

	auto operator=(stack_vector<T, Allocator> &&other)
		-> stack_vector<T, Allocator> &
	{
		if (&other == this) {
			return *this;
		}

		for (auto &value : *this) {
			value.~T();
		}

		_allocator.deallocate(_begin, _capacity);
		_allocator.~Allocator();
		new (this) stack_vector<T>{std::move(other)};

		return *this;
	}

	~stack_vector()
	{
		for (auto &value : *this) {
			value.~T();
		}

		_allocator.deallocate(_begin, _capacity);
	}

	auto extend(const std::size_t size)
	{
		if (size <= _size) {
			return false;
		}

		if (_capacity < size) {
			realloc(size);
		}

		for (auto i{_size}; i != size; ++i) {
			new (_begin + i) T{};
		}

		_size = size;
		_end  = _begin + _size;

		return true;
	}

	void push(const T &value)
	{
		emplace(value);
	}

	void push(T &&value)
	{
		emplace(std::move(value));
	}

	template <typename ...Args>
	void emplace(Args &&...args)
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
			_end  = _begin + _size;
		}
	}

	[[nodiscard]]
	auto size() const noexcept
	{
		return _size;
	}

	[[nodiscard]]
	auto operator[](const std::size_t i) noexcept
		-> T &
	{
		return _begin[i];
	}

	[[nodiscard]]
	auto operator[](const std::size_t i) const noexcept
		-> const T &
	{
		return _begin[i];
	}

	[[nodiscard]]
	auto begin() noexcept
	{
		return _begin;
	}

	[[nodiscard]]
	auto begin() const noexcept
	{
		return _begin;
	}

	[[nodiscard]]
	auto end() noexcept
	{
		return _end;
	}

	[[nodiscard]]
	auto end() const noexcept
	{
		return _end;
	}

	void pop()
	{
		--_end;
		_end->~T();
		--_size;
	}

private:
	Allocator            _allocator{};
	std::size_t          _capacity{};
	T                   *_begin{};
	decltype(_capacity)  _size{};
	decltype(_begin)     _end{};

	void realloc(const std::size_t capacity)
	{
		auto tmp{_allocator.allocate(capacity)};

		for (auto dst{tmp}, src{_begin}; src != _end; ++dst, ++src) {
			new (dst) T{std::move(*src)};
			src->~T();
		}

		_allocator.deallocate(_begin, _capacity);
		_capacity = capacity;
		_begin    = tmp;
	}
};
}

#endif
