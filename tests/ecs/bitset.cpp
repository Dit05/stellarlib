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

#include <stellarlib/ecs/bitset.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <utility>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

constexpr std::array<std::size_t, 3> ELEMS{
	std::numeric_limits<std::size_t>::digits * 3 - 1,
	0,
	(std::numeric_limits<std::size_t>::digits * 3 - 1) / 2
};

namespace
{
void check_elems(const bitset &set)
{
	for (std::size_t elem{}; elem != std::ranges::max(ELEMS); ++elem) {
		ASSERT_EQ(set.contains(elem), std::ranges::find(ELEMS, elem) != ELEMS.end());
	}
}
}

TEST(stellarlib_ecs_bitset, should_init_via_ctor)
{
	const bitset set{};
	ASSERT_FALSE(set.contains(0));
}

TEST(stellarlib_ecs_bitset, should_skip_empty_copy_via_ctor)
{
	const bitset set1{};
	const auto set2{set1};
	ASSERT_FALSE(set2.contains(0));
}

TEST(stellarlib_ecs_bitset, should_copy_via_ctor)
{
	bitset set1{};
	for (const auto elem : ELEMS) {
		set1.insert(elem);
	}
	const auto set2{set1};
	check_elems(set2);
}

TEST(stellarlib_ecs_bitset, should_move_via_ctor)
{
	bitset set1{};
	for (const auto elem : ELEMS) {
		set1.insert(elem);
	}
	const auto set2{std::move(set1)};
	check_elems(set2);
}

TEST(stellarlib_ecs_bitset, should_handle_empty_copy_via_assignment_vica)
{
	const bitset set1{};
	bitset set2{};
	set2.insert(ELEMS.front());
	set2 = set1;
	ASSERT_FALSE(set2.contains(ELEMS.front()));
}

TEST(stellarlib_ecs_bitset, should_handle_empty_copy_via_assignment_versa)
{
	bitset set1{};
	set1.insert(ELEMS.front());
	bitset set2{};
	set2 = set1;
	ASSERT_TRUE(set2.contains(ELEMS.front()));
}

TEST(stellarlib_ecs_bitset, should_copy_via_assignment)
{
	bitset set1{};
	for (const auto elem : ELEMS) {
		set1.insert(elem);
	}
	bitset set2{};
	set2.insert(ELEMS.front());
	set2 = set1;
	check_elems(set2);
}

TEST(stellarlib_ecs_bitset, should_move_via_assignment)
{
	bitset set1{};
	for (const auto elem : ELEMS) {
		set1.insert(elem);
	}
	bitset set2{};
	set2.insert(ELEMS.front());
	set2 = std::move(set1);
	check_elems(set2);
}

TEST(stellarlib_ecs_bitset, should_insert_and_erase_elems)
{
	bitset set{};
	for (const auto elem : ELEMS) {
		set.insert(elem);
		ASSERT_TRUE(set.contains(elem));
		set.erase(elem);
		ASSERT_FALSE(set.contains(elem));
		set.insert(elem);
		ASSERT_TRUE(set.contains(elem));
	}
	check_elems(set);
}

TEST(stellarlib_ecs_bitset, should_evaluate_equal_sets)
{
	bitset set1{};
	set1.insert(std::ranges::min(ELEMS));
	bitset set2{};
	set2.insert(std::ranges::min(ELEMS));
	set2.insert(std::ranges::max(ELEMS));
	set2.erase(std::ranges::max(ELEMS));
	ASSERT_TRUE(set1 == set2);
	ASSERT_TRUE(set2 == set1);
	ASSERT_FALSE(set1 != set2);
	ASSERT_FALSE(set2 != set1);
	ASSERT_TRUE(set1 <= set2);
	ASSERT_TRUE(set2 <= set1);
	ASSERT_TRUE(set2 >= set1);
	ASSERT_TRUE(set1 >= set2);
}


TEST(stellarlib_ecs_bitset, should_evaluate_disjoint_sets)
{
	bitset set1{};
	set1.insert(std::ranges::min(ELEMS));
	bitset set2{};
	set2.insert(std::ranges::max(ELEMS));
	ASSERT_FALSE(set1 == set2);
	ASSERT_FALSE(set2 == set1);
	ASSERT_TRUE(set1 != set2);
	ASSERT_TRUE(set2 != set1);
	ASSERT_FALSE(set1 <= set2);
	ASSERT_FALSE(set2 <= set1);
	ASSERT_FALSE(set2 >= set1);
	ASSERT_FALSE(set1 >= set2);
}

TEST(stellarlib_ecs_bitset, should_evaluate_tight_subset)
{
	bitset subset{};
	subset.insert(std::ranges::min(ELEMS));
	bitset superset{};
	for (const auto elem : ELEMS) {
		superset.insert(elem);
	}
	ASSERT_TRUE(subset <= superset);
	ASSERT_FALSE(superset <= subset);
	ASSERT_TRUE(superset >= subset);
	ASSERT_FALSE(subset >= superset);
}

TEST(stellarlib_ecs_bitset, should_evaluate_loose_subset)
{
	bitset subset{};
	subset.insert(std::ranges::min(ELEMS));
	subset.insert(std::ranges::max(ELEMS));
	subset.erase(std::ranges::max(ELEMS));
	bitset superset{};
	superset.insert(std::ranges::min(ELEMS));
	superset.insert((std::ranges::min(ELEMS) + std::ranges::max(ELEMS)) / 2);
	ASSERT_TRUE(subset <= superset);
	ASSERT_FALSE(superset <= subset);
	ASSERT_TRUE(superset >= subset);
	ASSERT_FALSE(subset >= superset);
}

TEST(stellarlib_ecs_bitset, should_clear_elems)
{
	bitset set{};
	for (const auto elem : ELEMS) {
		set.insert(elem);
	}
	set.clear();
	for (std::size_t elem{}; elem != std::ranges::max(ELEMS); ++elem) {
		ASSERT_FALSE(set.contains(elem));
	}
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
