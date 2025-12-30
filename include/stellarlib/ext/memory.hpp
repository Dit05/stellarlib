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

#include <stellarlib/ext/type_traits.hpp>

#include <cstddef>
#include <cstdlib>
#include <memory>

namespace stellarlib::ext
{
template <typename T, typename SizeType = std::size_t>
class arena_allocator : std::allocator<T>
{
public:
	using value_type = std::allocator<T>::value_type;
	using size_type = SizeType;
	using difference_type = std::allocator<value_type>::difference_type;
	using propagate_on_container_move_assignment = std::allocator<value_type>::propagate_on_container_move_assignment;

	constexpr void allocate(value_type *&begin, const size_type capacity) const
	{
		begin = reinterpret_cast<value_type *>(std::malloc(capacity * sizeof(value_type)));
	}

	constexpr void reallocate(value_type *&begin, const size_type size, size_type &capacity) const
	{
		if constexpr (is_trivially_relocatable_v<value_type>) {
			capacity += capacity / 4;
			begin = reinterpret_cast<value_type *>(std::realloc(begin, capacity * sizeof(value_type)));
		}
		else {
			capacity *= 2;
			const auto dst{reinterpret_cast<value_type *>(std::malloc(capacity * sizeof(value_type)))};

			for (size_type i{}; i != size; ++i) {
				const auto src{begin + i};
				std::construct_at(dst + i, std::move(*src));
				std::destroy_at(src);
			}

			std::free(begin);
			begin = dst;
		}
	}

	constexpr void deallocate(value_type *begin) const
	{
		std::free(begin);
	}

	[[nodiscard]]
	constexpr auto operator==(const arena_allocator<value_type, size_type> &) const
		-> bool = default;
};
}

#endif
