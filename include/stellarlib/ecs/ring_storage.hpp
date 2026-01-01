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

#ifndef STELLARLIB_ECS_RING_STORAGE_HPP
#define STELLARLIB_ECS_RING_STORAGE_HPP

#include <stellarlib/ecs/sparse_map.hpp>
#include <stellarlib/ecs/stack_vector.hpp>

#include <cstdint>
#include <utility>

namespace stellarlib::ecs
{
template <typename T>
class ring_storage final
{
public:
	[[nodiscard]]
	auto acquire()
	{
		std::uint32_t key{};

		if (_stack.size()) {
			const auto last{_stack.end() - 1};
			key = last->first;
			_set.insert(key, std::move(last->second));
			_stack.pop();
		}
		else {
			key = _key++;
			_set.insert(key);
		}

		return key;
	}

	[[nodiscard]]
	auto contains(const std::uint32_t key) const
	{
		return _set.contains(key);
	}

	[[nodiscard]]
	auto at(const std::uint32_t key) const
	{
		return _set.at(key);
	}

	[[nodiscard]]
	auto operator[](const std::uint32_t key) const
		-> T &
	{
		return _set[key];
	}

	[[nodiscard]]
	auto keys() const
	{
		return _set.keys();
	}

	[[nodiscard]]
	auto values() const
	{
		return _set.values();
	}

	[[nodiscard]]
	auto zip() const
	{
		return _set.zip();
	}

	[[nodiscard]]
	auto release(const std::uint32_t key)
	{
		if (!contains(key)) {
			return;
		}

		if constexpr (requires { _set[key].reset(); }) {
			_set[key].reset();
		}
		else if constexpr (requires { _set[key].clear(); }) {
			_set[key].clear();
		}

		_stack.push(key, std::move(_set[key]));
		_set.erase(key);
	}

private:
	uint32_t _key{};
	uint32_t _padding{};
	sparse_map<std::uint32_t, T> _set;
	internal::stack_vector<std::pair<std::uint32_t, T>, std::uint32_t> _stack;
};
}

#endif
