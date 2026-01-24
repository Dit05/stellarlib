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

#include <stellarlib/ecs/sparse_set.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <ranges>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

constexpr std::array<std::uint32_t, 5> VALUES{0, 5, 10, 15, 20};

TEST(stellarlib_ecs_sparse_set, should_insert_and_erase_values)
{
	internal::sparse_set<std::uint32_t> set{};
	for (const auto i : std::views::iota(std::size_t{}, VALUES.size())) {
		set.insert(VALUES[i]);
		ASSERT_NE(std::ranges::find(set, VALUES[i]), set.end());
		set.erase(VALUES[i / 2]);
		ASSERT_EQ(std::ranges::find(set, VALUES[i / 2]), set.end());
		set.insert(VALUES[i / 2]);
		ASSERT_NE(std::ranges::find(set, VALUES[i / 2]), set.end());
		set.erase(VALUES[i]);
		ASSERT_EQ(std::ranges::find(set, VALUES[i]), set.end());
		set.insert(VALUES[i]);
		ASSERT_NE(std::ranges::find(set, VALUES[i]), set.end());
	}
	ASSERT_TRUE(std::ranges::equal(set, VALUES));
}

/* NOLINTEND(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
