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

#ifndef STELLARLIB_ECS_SPARSE_SET_HPP
#define STELLARLIB_ECS_SPARSE_SET_HPP

#include "any_set.hpp"
#include "stack_vector.hpp"

#include <cstddef>
#include <optional>
#include <ranges>
#include <utility>

namespace stellarlib::ecs
{
template <typename T>
class sparse_set final : public any_set
{
public:
	[[nodiscard]]
	explicit sparse_set() noexcept = default;

	[[nodiscard]]
	sparse_set(const sparse_set<T> &) = default;

	[[nodiscard]]
	sparse_set(sparse_set<T> &&) = default;

	auto operator=(const sparse_set<T> &)
		-> sparse_set<T> & = default;

	auto operator=(sparse_set<T> &&)
		-> sparse_set<T> & = default;

	~sparse_set() final = default;

	void insert(const std::size_t key, const T &value)
	{
		if (!_sparse.extend(key + 1) && _sparse[key]) {
			_values[*_sparse[key]] = value;
		}
		else {
			_values.push(value);
			_sparse[key] = _keys.size();
			_keys.push(key);
		}
	}

	void insert(const std::size_t key, T &&value)
	{
		if (!_sparse.extend(key + 1) && _sparse[key]) {
			_values[*_sparse[key]] = std::move(value);
		}
		else {
			_values.push(std::move(value));
			_sparse[key] = _keys.size();
			_keys.push(key);
		}
	}

	template <typename ...Args>
	void emplace(const std::size_t key, Args &&...args)
	{
		if (!_sparse.extend(key + 1) && _sparse[key]) {
			auto ptr{_values.begin() + *_sparse[key]};
			ptr->~T();
			new (ptr) T{std::forward<Args>(args)...};
		}
		else {
			_values.emplace(std::forward<Args>(args)...);
			_sparse[key] = _keys.size();
			_keys.push(key);
		}
	}

	[[nodiscard]]
	auto contains(const std::size_t key) const noexcept
	{
		return key < _sparse.size() && _sparse[key];
	}

	[[nodiscard]]
	auto operator[](const std::size_t key) noexcept
		-> T &
	{
		return _values[*_sparse[key]];
	}

	[[nodiscard]]
	auto operator[](const std::size_t key) const noexcept
		-> const T &
	{
		return _values[*_sparse[key]];
	}

	[[nodiscard]]
	auto at(const std::size_t key) noexcept
	{
		return contains(key) ? _values.begin() + *_sparse[key] : nullptr;
	}

	[[nodiscard]]
	auto at(const std::size_t key) const noexcept
	{
		return contains(key) ? _values.begin() + *_sparse[key] : nullptr;
	}

	[[nodiscard]]
	auto keys() const noexcept
	{
		return std::ranges::views::all(_keys);
	}

	[[nodiscard]]
	auto values() noexcept
	{
		return std::ranges::views::all(_values);
	}

	[[nodiscard]]
	auto values() const noexcept
	{
		return std::ranges::views::all(_values);
	}

	[[nodiscard]]
	auto zip() noexcept
	{
		return std::ranges::views::zip(std::as_const(_keys), _values);
	}

	[[nodiscard]]
	auto zip() const noexcept
	{
		return std::ranges::views::zip(_keys, _values);
	}

	void erase(const std::size_t key) override
	{
		if (!contains(key)) {
			return;
		}

		const auto index{*_sparse[key]};
		_sparse[key].reset();

		if (index != _keys.size() - 1) {
			std::swap(_keys[index], *(_keys.end() - 1));
			_sparse[_keys[index]] = index;
			std::swap(_values[index], *(_values.end() - 1));
		}

		_keys.pop();
		_values.pop();
	}

private:
	stack_vector<T>                          _values;
	stack_vector<std::size_t>                _keys;
	stack_vector<std::optional<std::size_t>> _sparse;
};
}

#endif
