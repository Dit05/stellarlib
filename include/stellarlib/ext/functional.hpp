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

#ifndef STELLARLIB_EXT_FUNCTIONAL_HPP
#define STELLARLIB_EXT_FUNCTIONAL_HPP

#include <utility>

namespace stellarlib::ext
{
template <typename T>
[[nodiscard]]
constexpr auto truthy(const T &arg)
{
	return static_cast<bool>(arg);
}

template <typename T>
[[nodiscard]]
constexpr auto falsy(const T &arg)
{
	return !truthy(arg);
}

template <typename T>
[[nodiscard]]
constexpr auto subset(const T &lhs, const T &rhs)
{
	return (lhs & rhs) == lhs;
}

template <typename T>
[[nodiscard]]
constexpr auto superset(const T &lhs, const T &rhs)
{
	return subset(rhs, lhs);
}

namespace zip
{
template <typename T>
[[nodiscard]]
constexpr auto subset(const std::pair<T, T> &arg)
{
	return ext::subset(arg.first, arg.second);
}

template <typename T>
[[nodiscard]]
constexpr auto superset(const std::pair<T, T> &arg)
{
	return ext::superset(arg.first, arg.second);
}
}
}

#endif
