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

#ifndef STELLARLIB_ECS_STACK_VECTOR_HPP
#define STELLARLIB_ECS_STACK_VECTOR_HPP

#include <stellarlib/ext/memory.hpp>

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace stellarlib::ecs::internal
{
template <typename T, typename SizeType = std::size_t>
class stack_vector final : ext::vector_allocator<T, SizeType>
{
public:
	[[nodiscard]]
	explicit constexpr stack_vector() = default;

	[[nodiscard]]
	constexpr stack_vector(const stack_vector &other)
		: _size{other._size}
	{
		if (_size) {
			_capacity = _size;
			ext::vector_allocator<T, SizeType>::allocate(_begin, _capacity);
			std::uninitialized_copy(other._begin.get(), other._end, _begin.get());
			_end = _begin.get() + _size;
		}
	}

	[[nodiscard]]
	constexpr stack_vector(stack_vector &&other)
		: _size{other._size}
		, _capacity{other._capacity}
		, _begin{std::move(other._begin)}
		, _end{other._end}
	{
		other._end = nullptr;
	}

	constexpr auto operator=(const stack_vector &other)
		-> stack_vector &
	{
		if (std::addressof(other) == this) {
			return *this;
		}

		std::ranges::destroy(*this);
		_size = other._size;

		if (_capacity < _size) {
			_capacity = _size;
			ext::vector_allocator<T, SizeType>::reallocate(_begin, _capacity);
		}

		std::uninitialized_copy(other._begin.get(), other._end, _begin.get());
		_end = _begin.get() + _size;
		return *this;
	}

	constexpr auto operator=(stack_vector &&other)
		-> stack_vector &
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
	}

	template <typename ...Args>
	constexpr void push(Args &&...args)
	{
		if (_size == _capacity) {
			++_capacity;
			ext::vector_allocator<T, SizeType>::reallocate(_begin, _size, _capacity);
			_end = _begin.get() + _size;
		}

		std::construct_at(_end, std::forward<Args>(args)...);
		++_end;
		++_size;
	}

	template <typename ...Args>
	constexpr auto extend(const SizeType size, Args &&...args)
	{
		if (size <= _size) {
			return false;
		}

		if (_capacity < size) {
			_capacity = size;
			ext::vector_allocator<T, SizeType>::reallocate(_begin, _size, _capacity);
		}

		_end = _begin.get() + size;

		if constexpr (!std::is_scalar_v<T> || sizeof...(Args)) {
			std::uninitialized_fill(_begin.get() + _size, _end, T{std::forward<Args>(args)...});
		}

		_size = size;
		return true;
	}

	[[nodiscard]]
	constexpr auto size() const
	{
		return _size;
	}

	[[nodiscard]]
	constexpr auto operator[](const SizeType index) const
		-> T &
	{
		return _begin.get()[index];
	}

	[[nodiscard]]
	constexpr auto begin() const
	{
		return _begin.get();
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
		_end = _begin.get();
	}

private:
	SizeType _size{};
	SizeType _capacity{};
	std::unique_ptr<T, void (*)(T *)> _begin{nullptr, ext::vector_allocator<T, SizeType>::deallocate};
	T *_end{};
};
}

#endif
