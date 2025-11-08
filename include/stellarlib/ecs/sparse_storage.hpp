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

#ifndef STELLARLIB_ECS_SPARSE_STORAGE_HPP
#define STELLARLIB_ECS_SPARSE_STORAGE_HPP

#include <stellarlib/ecs/any_set.hpp>
#include <stellarlib/ecs/sparse_set.hpp>

#include <cstddef>
#include <memory>

namespace stellarlib::ecs
{
class sparse_storage final
{
public:
	[[nodiscard]]
	explicit sparse_storage() = default;

	[[nodiscard]]
	sparse_storage(const sparse_storage &other);

	[[nodiscard]]
	sparse_storage(sparse_storage &&) = default;

	auto operator=(const sparse_storage &other)
		-> sparse_storage &;

	auto operator=(sparse_storage &&)
		-> sparse_storage & = default;

	~sparse_storage() = default;

	template <typename T>
	[[nodiscard]]
	static auto id_of()
	{
		static auto id{next_id()};
		return id;
	}

private:
	sparse_set<std::shared_ptr<any_set<std::size_t>>> _sets;

	[[nodiscard]]
	static auto next_id()
		-> std::size_t;
};
}

#endif
