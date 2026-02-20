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

#ifndef STELLARLIB_ECS_ANY_SET_HPP
#define STELLARLIB_ECS_ANY_SET_HPP

#include <memory>

namespace stellarlib::ecs::internal
{
template <typename Key>
class any_set
{
public:
	virtual constexpr ~any_set() noexcept = default;

	[[nodiscard]]
	virtual constexpr auto clone() const noexcept
		-> std::unique_ptr<any_set> = 0;

	virtual constexpr void erase(Key) noexcept = 0;

	virtual constexpr void clear() noexcept = 0;

protected:
	[[nodiscard]]
	explicit constexpr any_set() noexcept = default;

	[[nodiscard]]
	constexpr any_set(const any_set &) noexcept = default;

	[[nodiscard]]
	constexpr any_set(any_set &&) noexcept = default;

	constexpr auto operator=(const any_set &) noexcept
		-> any_set & = default;

	constexpr auto operator=(any_set &&) noexcept
		-> any_set & = default;
};
}

#endif
