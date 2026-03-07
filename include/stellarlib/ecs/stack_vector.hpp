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
#include <stellarlib/ext/type_traits.hpp>

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
	explicit constexpr stack_vector() noexcept = default;

	[[nodiscard]]
	constexpr stack_vector(const stack_vector &other) noexcept
		: _size{other._size}
	{
		if (_size) {
			_capacity = _size;
			ext::vector_allocator<T, SizeType>::allocate(_begin, _capacity);
			_end = _begin + _size;
			std::uninitialized_copy(other._begin, other._end, _begin);
		}
	}

	[[nodiscard]]
	constexpr stack_vector(stack_vector &&other) noexcept
		: _size{std::exchange(other._size, {})}
		, _capacity{std::exchange(other._capacity, {})}
		, _begin{std::exchange(other._begin, {})}
		, _end{std::exchange(other._end, {})}
	{}

	constexpr auto operator=(const stack_vector &other) noexcept
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

		_end = _begin + _size;
		std::uninitialized_copy(other._begin, other._end, _begin);
		return *this;
	}

	constexpr auto operator=(stack_vector &&other) noexcept
		-> stack_vector &
	{
		if (std::addressof(other) != this) {
			std::destroy_at(this);
			std::construct_at(this, std::move(other));
		}

		return *this;
	}

	constexpr ~stack_vector() noexcept
	{
		std::ranges::destroy(*this);
		ext::vector_allocator<T, SizeType>::deallocate(_begin);
	}

	template <typename ...Args>
	constexpr void push(Args &&...args) noexcept
	{
		if (_size == _capacity) {
			++_capacity;
			ext::vector_allocator<T, SizeType>::reallocate(_begin, _size, _capacity);
			_end = _begin + _size;
		}

		++_size;
		std::construct_at(_end++, std::forward<Args>(args)...);
	}

	template <typename ...Args>
	constexpr auto extend(const SizeType size, Args &&...args) noexcept
	{
		if (size <= _size) {
			return false;
		}

		if (_capacity < size) {
			_capacity = size;
			ext::vector_allocator<T, SizeType>::reallocate(_begin, _size, _capacity);
		}

		_end = _begin + size;

		if constexpr (sizeof...(Args)) {
			std::uninitialized_fill(_begin + _size, _end, T{std::forward<Args>(args)...});
		}
		else if constexpr (!std::is_trivially_copy_constructible_v<T> || !ext::is_trivially_relocatable_v<T>) {
			std::uninitialized_default_construct(_begin + _size, _end);
		}

		_size = size;
		return true;
	}

	[[nodiscard]]
	constexpr auto size() const noexcept
	{
		return _size;
	}

	[[nodiscard]]
	constexpr auto operator[](const SizeType index) const noexcept
		-> T &
	{
		return _begin[index];
	}

	[[nodiscard]]
	constexpr auto begin() const noexcept
	{
		return _begin;
	}

	[[nodiscard]]
	constexpr auto end() const noexcept
	{
		return _end;
	}

	constexpr void pop() noexcept
	{
		--_size;
		--_end;
		std::destroy_at(_end);
	}

	constexpr void clear() noexcept
	{
		std::ranges::destroy(*this);
		_end = _begin;
		_size = 0;
	}

private:
	SizeType _size{};
	SizeType _capacity{};
	[[no_unique_address]] ext::padding<T *, SizeType, SizeType> _padding{};
	T *_begin{};
	T *_end{};
};
}

#endif
