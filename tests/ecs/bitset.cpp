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

#include <stellarlib/ext/bit.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <ranges>
#include <utility>

using namespace stellarlib;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

constexpr std::array<std::uintmax_t, 3> BITS{
	std::numeric_limits<std::uintmax_t>::digits * 3 - 1,
	std::numeric_limits<std::uintmax_t>::digits - 1,
	std::numeric_limits<std::uintmax_t>::digits * 2 - 1
};

static_assert(ext::bit_index(BITS[0]) > ext::bit_index(BITS[1]));
static_assert(ext::bit_index(BITS[1]) < ext::bit_index(BITS[2]));
static_assert(ext::bit_index(BITS[2]) < ext::bit_index(BITS[0]));
static_assert(std::ranges::find(BITS, (std::ranges::min(BITS) + std::ranges::max(BITS)) / 2) != BITS.end());

namespace
{
constexpr void check_bits(const ecs::internal::bitset &set)
{
	for (const auto bit : std::views::iota(std::uintmax_t{}, std::ranges::max(BITS))) {
		ASSERT_EQ(set.contains(bit), std::ranges::find(BITS, bit) != BITS.end());
	}
}
}

TEST(stellarlib_ecs_bitset, should_copy_via_ctor)
{
	ecs::internal::bitset set1{};
	for (const auto bit : BITS) {
		set1.insert(bit);
	}
	const auto set2{set1};
	check_bits(set2);
}

TEST(stellarlib_ecs_bitset, should_move_via_ctor)
{
	ecs::internal::bitset set1{};
	for (const auto bit : BITS) {
		set1.insert(bit);
	}
	const auto set2{std::move(set1)};
	check_bits(set2);
}

TEST(stellarlib_ecs_bitset, should_skip_self_copy_via_assignment)
{
	ecs::internal::bitset set{};
	for (const auto bit : BITS) {
		set.insert(bit);
	}
	set = set;
	check_bits(set);
}

TEST(stellarlib_ecs_bitset, should_copy_via_assignment)
{
	ecs::internal::bitset set1{};
	for (const auto bit : BITS) {
		set1.insert(bit);
	}
	ecs::internal::bitset set2{};
	set2 = set1;
	check_bits(set2);
}

TEST(stellarlib_ecs_bitset, should_skip_self_move_via_assignment)
{
	ecs::internal::bitset set{};
	for (const auto bit : BITS) {
		set.insert(bit);
	}
	set = std::move(set);
	check_bits(set);
}

TEST(stellarlib_ecs_bitset, should_move_via_assignment)
{
	ecs::internal::bitset set1{};
	for (const auto bit : BITS) {
		set1.insert(bit);
	}
	ecs::internal::bitset set2{};
	set2 = std::move(set1);
	check_bits(set2);
}

TEST(stellarlib_ecs_bitset, should_insert_and_erase_bits)
{
	ecs::internal::bitset set{};
	for (const auto bit : BITS) {
		set.insert(bit);
		ASSERT_TRUE(set.contains(bit));
		set.erase(bit);
		ASSERT_FALSE(set.contains(bit));
		set.insert(bit);
		ASSERT_TRUE(set.contains(bit));
	}
	check_bits(set);
}

TEST(stellarlib_ecs_bitset, should_evaluate_equal_sets)
{
	ecs::internal::bitset set1{};
	set1.insert((std::ranges::min(BITS) + std::ranges::max(BITS)) / 2);
	set1.insert(std::ranges::min(BITS));
	set1.erase(std::ranges::min(BITS));
	ecs::internal::bitset set2{};
	set2.insert((std::ranges::min(BITS) + std::ranges::max(BITS)) / 2);
	set2.insert(std::ranges::max(BITS));
	set2.erase(std::ranges::max(BITS));
	ASSERT_EQ(set1, set2);
	ASSERT_EQ(set2, set1);
	ASSERT_TRUE(set1 <= set2);
	ASSERT_TRUE(set2 <= set1);
	ASSERT_TRUE(set2 >= set1);
	ASSERT_TRUE(set1 >= set2);
}


TEST(stellarlib_ecs_bitset, should_evaluate_disjoint_sets)
{
	ecs::internal::bitset set1{};
	set1.insert(std::ranges::min(BITS));
	ecs::internal::bitset set2{};
	set2.insert(std::ranges::max(BITS));
	ASSERT_NE(set1, set2);
	ASSERT_NE(set2, set1);
	ASSERT_FALSE(set1 <= set2);
	ASSERT_FALSE(set2 <= set1);
	ASSERT_FALSE(set2 >= set1);
	ASSERT_FALSE(set1 >= set2);
}

TEST(stellarlib_ecs_bitset, should_evaluate_subset_and_superset)
{
	ecs::internal::bitset subset{};
	subset.insert(std::ranges::min(BITS));
	subset.insert(std::ranges::max(BITS));
	subset.erase(std::ranges::max(BITS));
	ecs::internal::bitset superset{};
	superset.insert(std::ranges::min(BITS));
	superset.insert((std::ranges::min(BITS) + std::ranges::max(BITS)) / 2);
	ASSERT_TRUE(subset <= superset);
	ASSERT_FALSE(superset <= subset);
	ASSERT_TRUE(superset >= subset);
	ASSERT_FALSE(subset >= superset);
}

TEST(stellarlib_ecs_bitset, should_clear_bits)
{
	ecs::internal::bitset set{};
	for (const auto bit : std::views::iota(std::uintmax_t{}, std::ranges::max(BITS))) {
		set.insert(bit);
	}
	set.clear();
	for (const auto bit : BITS) {
		set.insert(bit);
	}
	check_bits(set);
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
