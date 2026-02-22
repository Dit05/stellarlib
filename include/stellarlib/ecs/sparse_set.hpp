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

#ifndef STELLARLIB_ECS_SPARSE_SET_HPP
#define STELLARLIB_ECS_SPARSE_SET_HPP

#include <stellarlib/ecs/stack_vector.hpp>

namespace stellarlib::ecs::internal
{
template <typename T>
class sparse_set final
{
public:
	[[nodiscard]]
	explicit constexpr sparse_set() noexcept = default;

	[[nodiscard]]
	constexpr sparse_set(const sparse_set &) noexcept = default;

	[[nodiscard]]
	constexpr sparse_set(sparse_set &&) noexcept = default;

	constexpr auto operator=(const sparse_set &) noexcept
		-> sparse_set & = default;

	constexpr auto operator=(sparse_set &&) noexcept
		-> sparse_set & = default;

	constexpr ~sparse_set() noexcept = default;

	constexpr void insert(const T key) noexcept
	{
		_sparse.extend(key + 1);
		_sparse[key] = _keys.size();
		_keys.push(key);
	}

	[[nodiscard]]
	constexpr auto begin() const noexcept
	{
		return static_cast<const T *>(_keys.begin());
	}

	[[nodiscard]]
	constexpr auto end() const noexcept
	{
		return static_cast<const T *>(_keys.end());
	}

	constexpr void erase(const T key) noexcept
	{
		if (_sparse[key] != _keys.size() - 1) {
			_keys[_sparse[key]] = *(_keys.end() - 1);
			_sparse[_keys[_sparse[key]]] = _sparse[key];
		}

		_keys.pop();
	}

private:
	stack_vector<T, T> _sparse;
	stack_vector<T, T> _keys;
};
}

#endif
