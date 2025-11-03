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

#include <stellarlib/ecs/any_set.hpp>
#include <stellarlib/ecs/stack_vector.hpp>

#include <cstdint>
#include <optional>
#include <ranges>
#include <type_traits>
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
	sparse_set(const sparse_set<T> &) noexcept(false) = default;

	[[nodiscard]]
	sparse_set(sparse_set<T> &&) noexcept = default;

	auto operator=(const sparse_set<T> &) noexcept(false)
		-> sparse_set<T> & = default;

	auto operator=(sparse_set<T> &&) noexcept(false)
		-> sparse_set<T> & = default;

	~sparse_set() noexcept(false) final = default;

	template <typename ...Args>
	void insert(const std::uint32_t key, Args &&...args)
	{
		if (!_sparse.extend(key + 1) && _sparse[key]) {
			if constexpr (sizeof...(Args) == 1 && (std::is_same_v<std::remove_cvref_t<Args>, T> && ...)) {
				(*this)[key].operator=(std::forward<Args>(args)...);
			}
			else {
				auto ptr{_values.begin() + *_sparse[key]};
				ptr->~T();
				new (ptr) T{std::forward<Args>(args)...};
			}
		}
		else {
			_values.push(std::forward<Args>(args)...);
			_sparse[key] = _keys.size();
			_keys.push(key);
		}
	}

	[[nodiscard]]
	auto contains(const std::uint32_t key) const noexcept
	{
		return key < _sparse.size() && _sparse[key];
	}

	[[nodiscard]]
	auto at(const std::uint32_t key) const noexcept
	{
		return contains(key) ? _values.begin() + *_sparse[key] : nullptr;
	}

	[[nodiscard]]
	auto operator[](const std::uint32_t key) const noexcept
		-> T &
	{
		return _values[*_sparse[key]];
	}

	[[nodiscard]]
	auto keys() const noexcept
	{
		return std::views::as_const(_keys);
	}

	[[nodiscard]]
	auto values() const noexcept
	{
		return std::views::all(_values);
	}

	[[nodiscard]]
	auto zip() const noexcept
	{
		return std::views::zip(keys(), values());
	}

	void erase(const std::uint32_t key) final
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
	stack_vector<T>                            _values;
	stack_vector<std::uint32_t>                _keys;
	stack_vector<std::optional<std::uint32_t>> _sparse;
};
}

#endif
