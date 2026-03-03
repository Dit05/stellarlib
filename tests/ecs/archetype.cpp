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

#include <stellarlib/ecs/archetype.hpp>

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
	std::numeric_limits<std::uintmax_t>::digits * 1 - 1,
	std::numeric_limits<std::uintmax_t>::digits * 3 - 1,
	std::numeric_limits<std::uintmax_t>::digits * 2 - 1
};

static_assert(std::ranges::find(BITS, (std::ranges::min(BITS) + std::ranges::max(BITS)) / 2) != BITS.end());
static_assert(ext::bit_index(BITS[0]) < ext::bit_index(BITS[1]));
static_assert(ext::bit_index(BITS[0]) < ext::bit_index(BITS[2]));
static_assert(ext::bit_index(BITS[2]) < ext::bit_index(BITS[1]));

namespace
{
constexpr void check_bits(const ecs::archetype &archetype)
{
	for (const auto bit : std::views::iota(std::uintmax_t{}, std::ranges::max(BITS))) {
		ASSERT_EQ(archetype.contains(bit), std::ranges::find(BITS, bit) != BITS.end());
	}
}
}

TEST(stellarlib_ecs_archetype, should_copy_via_ctor)
{
	ecs::archetype archetype1{};
	for (const auto bit : BITS) {
		archetype1.insert(bit);
	}
	const auto archetype2{archetype1};
	check_bits(archetype2);
}

TEST(stellarlib_ecs_archetype, should_move_via_ctor)
{
	ecs::archetype archetype1{};
	for (const auto bit : BITS) {
		archetype1.insert(bit);
	}
	const auto archetype2{std::move(archetype1)};
	check_bits(archetype2);
}

TEST(stellarlib_ecs_archetype, should_skip_self_copy_via_assignment)
{
	ecs::archetype archetype{};
	for (const auto bit : BITS) {
		archetype.insert(bit);
	}
	archetype = archetype;
	check_bits(archetype);
}

TEST(stellarlib_ecs_archetype, should_copy_via_assignment)
{
	ecs::archetype archetype1{};
	for (const auto bit : BITS) {
		archetype1.insert(bit);
	}
	ecs::archetype archetype2{};
	archetype2 = archetype1;
	check_bits(archetype2);
}

TEST(stellarlib_ecs_archetype, should_skip_self_move_via_assignment)
{
	ecs::archetype archetype{};
	for (const auto bit : BITS) {
		archetype.insert(bit);
	}
	archetype = std::move(archetype);
	check_bits(archetype);
}

TEST(stellarlib_ecs_archetype, should_move_via_assignment)
{
	ecs::archetype archetype1{};
	for (const auto bit : BITS) {
		archetype1.insert(bit);
	}
	ecs::archetype archetype2{};
	archetype2 = std::move(archetype1);
	check_bits(archetype2);
}

TEST(stellarlib_ecs_archetype, should_insert_and_erase_bits)
{
	ecs::archetype archetype{};
	for (const auto bit : BITS) {
		archetype.insert(bit);
		ASSERT_TRUE(archetype.contains(bit));
		archetype.erase(bit);
		archetype.erase(bit);
		ASSERT_FALSE(archetype.contains(bit));
		archetype.insert(bit);
		ASSERT_TRUE(archetype.contains(bit));
	}
	check_bits(archetype);
}

TEST(stellarlib_ecs_archetype, should_insert_archetype)
{
	ecs::archetype archetype1{};
	archetype1.insert(std::ranges::min(BITS) / 2);
	archetype1.insert((std::ranges::min(BITS) + std::ranges::max(BITS)) / 2);
	archetype1.erase((std::ranges::min(BITS) + std::ranges::max(BITS)) / 2);
	ecs::archetype archetype2{};
	for (const auto bit : BITS) {
		archetype2.insert(bit);
	}
	archetype1.insert(archetype2);
	ASSERT_TRUE(archetype1.contains(std::ranges::min(BITS) / 2));
	archetype1.erase(std::ranges::min(BITS) / 2);
	ASSERT_EQ(archetype1, archetype2);
}

TEST(stellarlib_ecs_archetype, should_evaluate_equal_archetypes)
{
	ecs::archetype archetype1{};
	archetype1.insert((std::ranges::min(BITS) + std::ranges::max(BITS)) / 2);
	archetype1.insert(std::ranges::min(BITS));
	archetype1.erase(std::ranges::min(BITS));
	ecs::archetype archetype2{};
	archetype2.insert((std::ranges::min(BITS) + std::ranges::max(BITS)) / 2);
	archetype2.insert(std::ranges::max(BITS));
	archetype2.erase(std::ranges::max(BITS));
	ASSERT_EQ(archetype1, archetype2);
	ASSERT_EQ(archetype2, archetype1);
	ASSERT_TRUE(archetype1 <= archetype2);
	ASSERT_TRUE(archetype2 <= archetype1);
	ASSERT_TRUE(archetype2 >= archetype1);
	ASSERT_TRUE(archetype1 >= archetype2);
}


TEST(stellarlib_ecs_archetype, should_evaluate_disjoint_archetypes)
{
	ecs::archetype archetype1{};
	archetype1.insert(std::ranges::min(BITS));
	ecs::archetype archetype2{};
	archetype2.insert(std::ranges::max(BITS));
	ASSERT_NE(archetype1, archetype2);
	ASSERT_NE(archetype2, archetype1);
	ASSERT_FALSE(archetype1 <= archetype2);
	ASSERT_FALSE(archetype2 <= archetype1);
	ASSERT_FALSE(archetype2 >= archetype1);
	ASSERT_FALSE(archetype1 >= archetype2);
}

TEST(stellarlib_ecs_archetype, should_evaluate_subset_and_superset)
{
	ecs::archetype subarchetype{};
	subarchetype.insert(std::ranges::min(BITS));
	subarchetype.insert(std::ranges::max(BITS));
	subarchetype.erase(std::ranges::max(BITS));
	ecs::archetype superarchetype{};
	superarchetype.insert(std::ranges::min(BITS));
	superarchetype.insert((std::ranges::min(BITS) + std::ranges::max(BITS)) / 2);
	ASSERT_TRUE(subarchetype <= superarchetype);
	ASSERT_FALSE(superarchetype <= subarchetype);
	ASSERT_TRUE(superarchetype >= subarchetype);
	ASSERT_FALSE(subarchetype >= superarchetype);
}

TEST(stellarlib_ecs_archetype, should_erase_archetype)
{
	ecs::archetype archetype1{};
	for (const auto bit : BITS) {
		archetype1.insert(bit);
	}
	const auto archetype2{archetype1};
	archetype1.insert(std::ranges::min(BITS) / 2);
	archetype1.erase(archetype2);
	archetype1.erase(archetype2);
	for (const auto bit : std::views::iota(std::uintmax_t{}, std::ranges::max(BITS))) {
		ASSERT_EQ(archetype1.contains(bit), bit == std::ranges::min(BITS) / 2);
	}
	archetype1.erase(archetype1);
	archetype1.erase(archetype1);
	ASSERT_EQ(archetype1, ecs::archetype{});
}

TEST(stellarlib_ecs_archetype, should_clear_bits)
{
	ecs::archetype archetype{};
	for (const auto bit : std::views::iota(std::uintmax_t{}, std::ranges::max(BITS))) {
		archetype.insert(bit);
	}
	archetype.clear();
	for (const auto bit : BITS) {
		archetype.insert(bit);
	}
	check_bits(archetype);
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
