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

#include <utility>

namespace stellarlib::ecs
{
template <typename T>
class sparse_set final
{
public:
	void insert(const T value)
	{
		if (_sparse.extend(value + 1, static_cast<T>(-1)) || _sparse[value] == static_cast<T>(-1)) {
			_sparse[value] = _values.size();
			_values.push(value);
		}
	}

	[[nodiscard]]
	auto size() const
	{
		return _values.size();
	}

	[[nodiscard]]
	auto contains(const T value) const
	{
		return value < _sparse.size() && _sparse[value] != static_cast<T>(-1);
	}

	[[nodiscard]]
	auto begin() const
		-> const T *
	{
		return _values.begin();
	}

	[[nodiscard]]
	auto end() const
		-> const T *
	{
		return _values.end();
	}

	void erase(const T value)
	{
		if (!contains(value)) {
			return;
		}

		const auto index{_sparse[value]};
		_sparse[value] = static_cast<T>(-1);

		if (index != _values.size() - 1) {
			std::swap(_values[index], *(_values.end() - 1));
			_sparse[_values[index]] = index;
		}

		_values.pop();
	}

	void clear()
	{
		_sparse.clear();
		_values.clear();
	}

private:
	internal::stack_vector<T, T> _values;
	internal::stack_vector<T, T> _sparse;
};
}

#endif
