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

#ifndef STELLARLIB_EXT_BIT_HPP
#define STELLARLIB_EXT_BIT_HPP

#include <limits>

namespace stellarlib::ext
{
template <typename T>
[[nodiscard]]
constexpr auto bit_index(const T arg)
{
	return static_cast<T>(arg / std::numeric_limits<T>::digits);
}

template <typename T>
[[nodiscard]]
constexpr auto bit_mask(const T arg)
{
	return static_cast<T>(static_cast<T>(1) << static_cast<T>(arg % std::numeric_limits<T>::digits));
}
}

#endif
