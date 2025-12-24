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

#include <stellarlib/ecs/sparse_set.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <ranges>
#include <utility>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

constexpr std::array<std::uint32_t, 5> VALUES{0, 5, 10, 15, 20};

namespace
{
void check_range(const sparse_set<std::uint32_t> &set)
{
	ASSERT_EQ(set.size(), VALUES.size());
	for (const auto value : VALUES) {
		ASSERT_TRUE(set.contains(value));
	}
	ASSERT_TRUE(std::ranges::equal(set, VALUES));
}
}

TEST(stellarlib_ecs_sparse_set, should_init_via_ctor)
{
	const sparse_set<std::uint32_t> set{};
	ASSERT_FALSE(set.size());
}

TEST(stellarlib_ecs_sparse_set, should_copy_via_ctor)
{
	sparse_set<std::uint32_t> set1{};
	for (const auto value : VALUES) {
		set1.insert(value);
	}
	auto set2{set1};
	check_range(set2);
}

TEST(stellarlib_ecs_sparse_set, should_move_via_ctor)
{
	sparse_set<std::uint32_t> set1{};
	for (const auto value : VALUES) {
		set1.insert(value);
	}
	auto set2{std::move(set1)};
	check_range(set2);
}

TEST(stellarlib_ecs_sparse_set, should_copy_via_assignment)
{
	sparse_set<std::uint32_t> set1{};
	for (const auto value : VALUES) {
		set1.insert(value);
	}
	sparse_set<std::uint32_t> set2{};
	set2 = set1;
	check_range(set2);
}

TEST(stellarlib_ecs_sparse_set, should_move_via_assignment)
{
	sparse_set<std::uint32_t> set1{};
	for (const auto value : VALUES) {
		set1.insert(value);
	}
	sparse_set<std::uint32_t> set2{};
	set2 = std::move(set1);
	check_range(set2);
}

TEST(stellarlib_ecs_sparse_set, should_insert_and_erase_values)
{
	sparse_set<std::uint32_t> set{};
	for (const auto [i, value] : std::views::zip(std::views::iota(0), VALUES)) {
		set.insert(value);
		ASSERT_EQ(set.size(), i + 1);
		ASSERT_TRUE(set.contains(value));
		ASSERT_NE(std::ranges::find(set, value), set.end());
		set.erase(value);
		ASSERT_EQ(set.size(), i);
		ASSERT_FALSE(set.contains(value));
		ASSERT_EQ(std::ranges::find(set, value), set.end());
		set.insert(value);
		ASSERT_EQ(set.size(), i + 1);
		ASSERT_TRUE(set.contains(value));
		ASSERT_NE(std::ranges::find(set, value), set.end());
	}
	check_range(set);
}

TEST(stellarlib_ecs_sparse_set, should_reinsert_value)
{
	sparse_set<std::uint32_t> set{};
	const auto value{VALUES.front()};
	set.insert(value);
	set.insert(value);
	ASSERT_EQ(set.size(), 1);
	ASSERT_TRUE(set.contains(value));
	ASSERT_NE(std::ranges::find(set, value), set.end());
}

TEST(stellarlib_ecs_sparse_set, should_clear_values)
{
	sparse_set<std::uint32_t> set{};
	for (const auto value : VALUES) {
		set.insert(value);
	}
	set.clear();
	ASSERT_FALSE(set.size());
	for (const auto value : VALUES) {
		ASSERT_FALSE(set.contains(value));
	}
	ASSERT_EQ(set.begin(), set.end());
	for (const auto value : VALUES) {
		set.insert(value);
	}
	check_range(set);
}

/* NOLINTEND(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
