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
#include <memory>
#include <ranges>
#include <utility>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

constexpr std::array<std::uint32_t, 5>                              KEYS{2, 1, 0, 3, 4};
static const std::array<std::shared_ptr<std::int32_t>, KEYS.size()> VALUES{
	std::make_shared<std::int32_t>(0),
	std::make_shared<std::int32_t>(5),
	std::make_shared<std::int32_t>(10),
	std::make_shared<std::int32_t>(15),
	std::make_shared<std::int32_t>(25)
};

namespace
{
void check_ranges_mut(sparse_set<std::shared_ptr<std::int32_t>> &set)
{
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		ASSERT_TRUE(set.contains(key));
		ASSERT_EQ(*set.at(key), value);
		ASSERT_EQ(set[key], value);
	}
	ASSERT_TRUE(std::ranges::equal(set.keys(), KEYS));
	ASSERT_TRUE(std::ranges::equal(set.values(), VALUES));
	ASSERT_TRUE(std::ranges::equal(set.zip(), std::views::zip(KEYS, VALUES)));
}

void check_ranges_const(const sparse_set<std::shared_ptr<std::int32_t>> &set)
{
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		ASSERT_TRUE(set.contains(key));
		ASSERT_EQ(*set.at(key), value);
		ASSERT_EQ(set[key], value);
	}
	ASSERT_TRUE(std::ranges::equal(set.keys(), KEYS));
	ASSERT_TRUE(std::ranges::equal(set.values(), VALUES));
	ASSERT_TRUE(std::ranges::equal(set.zip(), std::views::zip(KEYS, VALUES)));
}
}

TEST(ecs_sparse_set, should_init_via_default_ctor)
{
	const sparse_set<std::shared_ptr<std::int32_t>> set{};
	ASSERT_TRUE(set.keys().empty());
	ASSERT_TRUE(set.values().empty());
	ASSERT_TRUE(set.zip().empty());
}

TEST(ecs_sparse_set, should_copy_via_ctor)
{
	sparse_set<std::shared_ptr<std::int32_t>> set1{};
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		set1.insert(key, value);
	}
	auto set2{set1};
	check_ranges_mut(set2);
	check_ranges_const(set2);
}

TEST(ecs_sparse_set, should_move_via_ctor)
{
	sparse_set<std::shared_ptr<std::int32_t>> set1{};
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		set1.insert(key, value);
	}
	auto set2{std::move(set1)};
	check_ranges_mut(set2);
	check_ranges_const(set2);
}

TEST(ecs_sparse_set, should_copy_via_assignment)
{
	sparse_set<std::shared_ptr<std::int32_t>> set1{};
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		set1.insert(key, value);
	}
	sparse_set<std::shared_ptr<std::int32_t>> set2{};
	set2 = set1;
	check_ranges_mut(set2);
	check_ranges_const(set2);
}

TEST(ecs_sparse_set, should_move_via_assignment)
{
	sparse_set<std::shared_ptr<std::int32_t>> set1{};
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		set1.insert(key, value);
	}
	sparse_set<std::shared_ptr<std::int32_t>> set2{};
	set2 = std::move(set1);
	check_ranges_mut(set2);
	check_ranges_const(set2);
}

TEST(ecs_sparse_set, should_insert_and_erase_values)
{
	sparse_set<std::shared_ptr<std::int32_t>> set{};
	for (const auto pair : std::views::zip(KEYS, VALUES)) {
		set.insert(std::get<0>(pair), std::get<1>(pair));
		ASSERT_TRUE(set.contains(std::get<0>(pair)));
		ASSERT_EQ(*set.at(std::get<0>(pair)), std::get<1>(pair));
		ASSERT_EQ(set[std::get<0>(pair)], std::get<1>(pair));
		ASSERT_EQ(*std::ranges::find(set.keys(), std::get<0>(pair)), std::get<0>(pair));
		ASSERT_EQ(*std::ranges::find(set.values(), std::get<1>(pair)), std::get<1>(pair));
		ASSERT_EQ(*std::ranges::find(set.zip(), pair), pair);
		set.erase(std::get<0>(pair));
		ASSERT_FALSE(set.contains(std::get<0>(pair)));
		ASSERT_FALSE(set.at(std::get<0>(pair)));
		ASSERT_EQ(std::ranges::find(set.keys(), std::get<0>(pair)), set.keys().end());
		ASSERT_EQ(std::ranges::find(set.values(), std::get<1>(pair)), set.values().end());
		ASSERT_EQ(std::ranges::find(set.zip(), pair), set.zip().end());
		set.insert(std::get<0>(pair), std::get<1>(pair));
		ASSERT_TRUE(set.contains(std::get<0>(pair)));
		ASSERT_EQ(*set.at(std::get<0>(pair)), std::get<1>(pair));
		ASSERT_EQ(set[std::get<0>(pair)], std::get<1>(pair));
		ASSERT_EQ(*std::ranges::find(set.keys(), std::get<0>(pair)), std::get<0>(pair));
		ASSERT_EQ(*std::ranges::find(set.values(), std::get<1>(pair)), std::get<1>(pair));
		ASSERT_EQ(*std::ranges::find(set.zip(), pair), pair);
	}
	check_ranges_mut(set);
	check_ranges_const(set);
}

TEST(ecs_sparse_set, should_reinsert_value)
{
	sparse_set<std::shared_ptr<std::int32_t>> set{};
	const auto pair{std::views::zip(KEYS, VALUES).front()};
	set.insert(std::get<0>(pair), std::get<1>(pair));
	set.insert(std::get<0>(pair), std::get<1>(pair));
	ASSERT_TRUE(set.contains(std::get<0>(pair)));
	ASSERT_EQ(*set.at(std::get<0>(pair)), std::get<1>(pair));
	ASSERT_EQ(set[std::get<0>(pair)], std::get<1>(pair));
	ASSERT_EQ(set.keys().size(), 1);
	ASSERT_EQ(set.keys().front(), std::get<0>(pair));
	ASSERT_EQ(set.values().size(), 1);
	ASSERT_EQ(set.values().front(), std::get<1>(pair));
	ASSERT_EQ(set.zip().size(), 1);
	ASSERT_EQ(set.zip().front(), pair);
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
