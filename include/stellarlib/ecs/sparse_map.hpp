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

#ifndef STELLARLIB_ECS_SPARSE_MAP_HPP
#define STELLARLIB_ECS_SPARSE_MAP_HPP

#include <stellarlib/ecs/any_set.hpp>
#include <stellarlib/ecs/stack_vector.hpp>

#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace stellarlib::ecs::internal
{
template <typename Key, typename T>
class sparse_map final : public any_set<Key>
{
public:
	[[nodiscard]]
	explicit constexpr sparse_map() = default;

	[[nodiscard]]
	constexpr sparse_map(const sparse_map &) = default;

	[[nodiscard]]
	constexpr sparse_map(sparse_map &&) = default;

	constexpr auto operator=(const sparse_map &)
		-> sparse_map & = default;

	constexpr auto operator=(sparse_map &&)
		-> sparse_map & = default;

	constexpr ~sparse_map() final = default;

	auto clone() const
		-> sparse_map * final
	{
		if constexpr (std::is_copy_constructible_v<T>) {
			return new sparse_map{*this};
		}

		throw std::runtime_error{__FILE_NAME__":" + std::to_string(__LINE__) + ' ' + typeid(T).name() + " is not copy constructible"};
	}

	template <typename ...Args>
	void insert(const Key key, Args &&...args)
	{
		if (_sparse.extend(key + 1, static_cast<Key>(-1)) || _sparse[key] == static_cast<Key>(-1)) {
			_sparse[key] = _keys.size();
			_keys.push(key);
			_values.push(std::forward<Args>(args)...);
		}
		else if constexpr (sizeof...(Args) == 1 && (std::is_same_v<std::remove_cvref_t<Args>, T> && ...)) {
			(*this)[key] = (std::forward<Args>(args), ...);
		}
		else {
			const auto ptr{_values.begin() + _sparse[key]};
			std::destroy_at(ptr);
			std::construct_at(ptr, std::forward<Args>(args)...);
		}
	}

	[[nodiscard]]
	auto size() const
	{
		return _keys.size();
	}

	[[nodiscard]]
	auto contains(const Key key) const
	{
		return key < _sparse.size() && _sparse[key] != static_cast<Key>(-1);
	}

	[[nodiscard]]
	auto at(const Key key) const
	{
		return contains(key) ? _values.begin() + _sparse[key] : nullptr;
	}

	[[nodiscard]]
	auto operator[](const Key key) const
		-> T &
	{
		return _values[_sparse[key]];
	}

	[[nodiscard]]
	auto keys() const
	{
		return std::ranges::subrange{reinterpret_cast<const Key *>(_keys.begin()), reinterpret_cast<const Key *>(_keys.end())};
	}

	[[nodiscard]]
	auto values() const
	{
		return _values | std::views::all;
	}

	[[nodiscard]]
	auto zip() const
	{
		return std::views::zip(keys(), values());
	}

	void erase(const Key key) final
	{
		if (!contains(key)) {
			return;
		}

		const auto index{_sparse[key]};
		_sparse[key] = static_cast<Key>(-1);

		if (index != _keys.size() - 1) {
			std::swap(_keys[index], *(_keys.end() - 1));
			_sparse[_keys[index]] = index;
			std::swap(_values[index], *(_values.end() - 1));
		}

		_keys.pop();
		_values.pop();
	}

	void clear()
	{
		_sparse.clear();
		_keys.clear();
		_values.clear();
	}

private:
	stack_vector<Key, Key> _sparse;
	stack_vector<Key, Key> _keys;
	stack_vector<T, Key> _values;
};
}

#endif
