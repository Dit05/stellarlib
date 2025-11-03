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

#include <stellarlib/ecs/bitset.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

static const std::vector<std::uint32_t> VALUES{
	std::numeric_limits<std::uint32_t>::digits * 3 - 1,
	0,
	std::numeric_limits<std::uint32_t>::digits * 3 / 2
};

namespace
{
void check_range(const bitset &set)
{
	for (std::uint32_t i{}; i != std::ranges::max(VALUES); ++i) {
		ASSERT_EQ(set.contains(i), std::ranges::find(VALUES, i) != VALUES.end());
	}
}
}

TEST(ecs_bitset, should_init_via_default_ctor)
{
	const bitset set{};
	ASSERT_FALSE(set.contains(0));
}

TEST(ecs_bitset, should_skip_empty_copy_via_ctor)
{
	const bitset set1{};
	const auto set2{set1};
	ASSERT_FALSE(set2.contains(0));
}

TEST(ecs_bitset, should_copy_via_ctor)
{
	bitset set1{};
	for (const auto value : VALUES) {
		set1.insert(value);
	}
	auto set2{set1};
	check_range(set2);
}

TEST(ecs_bitset, should_move_via_ctor)
{
	bitset set1{};
	for (const auto value : VALUES) {
		set1.insert(value);
	}
	auto set2{std::move(set1)};
	check_range(set2);
}

TEST(ecs_bitset, should_handle_empty_copy_via_assignment_vica)
{
	const bitset set1{};
	bitset set2{};
	set2.insert(VALUES.front());
	set2 = set1;
	ASSERT_FALSE(set2.contains(VALUES.front()));
}

TEST(ecs_bitset, should_handle_empty_copy_via_assignment_versa)
{
	bitset set1{};
	set1.insert(VALUES.front());
	bitset set2{};
	set2 = set1;
	ASSERT_TRUE(set2.contains(VALUES.front()));
}

TEST(ecs_bitset, should_copy_via_assignment)
{
	bitset set1{};
	for (const auto value : VALUES) {
		set1.insert(value);
	}
	bitset set2{};
	set2.insert(VALUES.front());
	set2 = set1;
	check_range(set2);
}

TEST(ecs_bitset, should_move_via_assignment)
{
	bitset set1{};
	for (const auto value : VALUES) {
		set1.insert(value);
	}
	bitset set2{};
	set2.insert(VALUES.front());
	set2 = std::move(set1);
	check_range(set2);
}

TEST(ecs_bitset, should_insert_and_erase_values)
{
	bitset set{};
	for (const auto value : VALUES) {
		set.insert(value);
		ASSERT_TRUE(set.contains(value));
		set.erase(value);
		ASSERT_FALSE(set.contains(value));
		set.insert(value);
		ASSERT_TRUE(set.contains(value));
	}
	check_range(set);
}

TEST(ecs_bitset, should_clear_values)
{
	bitset set{};
	for (const auto value : VALUES) {
		set.insert(value);
	}
	set.clear();
	for (std::uint32_t i{}; i != std::ranges::max(VALUES); ++i) {
		ASSERT_FALSE(set.contains(i));
	}
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
