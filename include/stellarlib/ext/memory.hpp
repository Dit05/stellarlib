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

#ifndef STELLARLIB_EXT_MEMORY_HPP
#define STELLARLIB_EXT_MEMORY_HPP

#include <stellarlib/ext/functional.hpp>
#include <stellarlib/ext/type_traits.hpp>

#include <cstdlib>
#include <memory>
#include <new>

namespace stellarlib::ext
{
template <typename T, typename SizeType>
class arena_allocator : std::allocator<T>
{
public:
	using value_type = std::allocator<T>::value_type;
	using size_type = SizeType;
	using difference_type = std::allocator<value_type>::difference_type;
	using propagate_on_container_move_assignment = std::allocator<value_type>::propagate_on_container_move_assignment;

	constexpr void allocate(value_type *&begin, size_type &capacity)
	{
		capacity = grow(capacity);

		if constexpr (is_trivially_relocatable_v<value_type>) {
			begin = static_cast<value_type *>(std::malloc(sizeof(*begin) * capacity));

			if (falsy(begin)) {
				throw std::bad_alloc{};
			}
		}
		else {
			begin = std::allocator<value_type>::allocate(capacity);
		}
	}

	constexpr void reallocate(size_type size, size_type required, value_type *&begin, size_type &capacity)
	{
		if constexpr (is_trivially_relocatable_v<value_type>) {
			capacity = grow(required);
			begin = static_cast<value_type *>(std::realloc(begin, sizeof(*begin) * capacity));

			if (falsy(begin)) {
				throw std::bad_alloc{};
			}
		}
		else {
			required = grow(required);
			const auto tmp{std::allocator<value_type>::allocate(required)};
			std::uninitialized_move_n(begin, size, tmp);
			std::destroy_n(begin, size);
			std::allocator<value_type>::deallocate(begin, capacity);
			capacity = required;
			begin = tmp;
		}
	}

	constexpr void deallocate(value_type *begin, const size_type capacity)
	{
		if constexpr (is_trivially_relocatable_v<value_type>) {
			std::free(begin);
		}
		else {
			std::allocator<value_type>::deallocate(begin, capacity);
		}
	}

	[[nodiscard]]
	constexpr auto operator==(const arena_allocator<value_type, size_type> &other) const
	{
		return std::allocator<value_type>::operator==(other);
	}

private:
	[[nodiscard]]
	static constexpr auto grow(const size_type required)
	{
		return required + required / 2;
	}
};
}

#endif
