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

#ifndef STELLARLIB_ECS_SPARSE_STORAGE_HPP
#define STELLARLIB_ECS_SPARSE_STORAGE_HPP

#include <stellarlib/ecs/any_set.hpp>
#include <stellarlib/ecs/sparse_map.hpp>
#include <stellarlib/ext/utility.hpp>

#include <cstddef>
#include <memory>
#include <ranges>

namespace stellarlib::ecs
{
template <typename scope>
class sparse_storage final
{
public:
	[[nodiscard]]
	explicit sparse_storage() = default;

	[[nodiscard]]
	sparse_storage(const sparse_storage &other)
	{
		for (const auto [id, set] : other._sets.zip()) {
			_sets.insert(id, set->clone());
		}
	}

	[[nodiscard]]
	sparse_storage(sparse_storage &&) = default;

	auto operator=(const sparse_storage &other)
		-> sparse_storage &
	{
		if (std::addressof(other) == this) {
			return *this;
		}

		_sets.clear();

		for (const auto [id, set] : other._sets.zip()) {
			_sets.insert(id, set->clone());
		}

		return *this;
	}

	auto operator=(sparse_storage &&)
		-> sparse_storage & = default;

	~sparse_storage() = default;

	template <typename T>
	[[nodiscard]]
	static auto id_of()
	{
		return ext::scoped_typeid<scope, T>();
	}

	template <typename T>
	[[nodiscard]]
	auto by_type() const
	{
		const auto set{_sets.at(id_of<T>())};
		return set ? reinterpret_cast<sparse_map<std::size_t, T> *>(set->get()) : nullptr;
	}

	template <typename T>
	[[nodiscard]]
	auto by_type()
		-> sparse_map<std::size_t, T> &
	{
		const auto id{id_of<T>()};

		if (const auto set{_sets.at(id)}) {
			return static_cast<sparse_map<std::size_t, T> &>(*set->get());
		}

		const auto set{new sparse_map<std::size_t, T>{}};
		_sets.insert(id, set);
		return *set;
	}

	[[nodiscard]]
	auto sets() const
	{
		return std::views::transform(_sets.values(), [](const auto &set) -> internal::any_set<std::size_t> &
		{
			return *set.get();
		});
	}

private:
	sparse_map<std::size_t, std::unique_ptr<internal::any_set<std::size_t>>> _sets;
};
}

#endif
