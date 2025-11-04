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

#ifndef STELLARLIB_ECS_RING_STORAGE_HPP
#define STELLARLIB_ECS_RING_STORAGE_HPP

#include <stellarlib/ecs/stack_vector.hpp>

#include <cstdint>
#include <memory>
#include <optional>
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
		if (_queue.size()) {
			const auto last{_queue.end() - 1};
			const auto key{last->first};
			_sparse[key] = std::move(last->second);
			_queue.pop();

			return key;
		}

		const auto key{_sparse.size()};
		_sparse.push(T{});

		return key;
	}

	[[nodiscard]]
	auto at(const std::uint32_t key) const
	{
		return key < _sparse.size() && _sparse[key] ? std::addressof(*_sparse[key]) : nullptr;
	}

	[[nodiscard]]
	auto release(const std::uint32_t key)
	{
		if (_sparse.size() <= key || !_sparse[key]) {
			return;
		}

		if constexpr (requires { _sparse[key]->reset(); }) {
			_sparse[key]->reset();
		}
		else if constexpr (requires { _sparse[key]->clear(); }) {
			_sparse[key]->clear();
		}

		_queue.push(key, std::move(*_sparse[key]));
		_sparse[key].reset();
	}

private:
	stack_vector<std::optional<T>> _sparse;
	stack_vector<std::pair<std::uint32_t, T>> _queue;
};
}

#endif
