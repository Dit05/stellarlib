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

#include <bit>
#include <cstdlib>
#include <memory>
#include <new>
#include <type_traits>

namespace stellarlib::ext
{
template <typename T, typename size_type>
class arena_allocator : std::allocator<T>
{
public:
	static constexpr auto is_pod{std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>};

	constexpr void allocate(T *&begin, size_type &capacity)
	{
		capacity = std::bit_ceil(capacity);

		if constexpr (is_pod) {
			begin = static_cast<T *>(std::malloc(sizeof(T) * capacity));

			if (ext::falsy(begin)) {
				throw std::bad_alloc{};
			}
		} else {
			begin = std::allocator<T>::allocate(capacity);
		}
	}

	constexpr void reallocate(size_type size, size_type required, T *&begin, size_type &capacity)
	{

		if constexpr (is_pod) {
			capacity = std::bit_ceil(required);
			begin = static_cast<T *>(std::realloc(begin, sizeof(T) * capacity));

			if (ext::falsy(begin)) {
				throw std::bad_alloc{};
			}
		}
		else {
			required = std::bit_ceil(required);
			const auto tmp{std::allocator<T>::allocate(required)};
			std::uninitialized_move_n(begin, size, tmp);
			std::destroy_n(begin, size);
			std::allocator<T>::deallocate(begin, capacity);
			capacity = required;
			begin = tmp;
		}
	}

	constexpr void deallocate(T *begin, const size_type capacity)
	{
		if constexpr (is_pod) {
			std::free(begin);
		}
		else {
			std::allocator<T>::deallocate(begin, capacity);
		}
	}
};
}

#endif
