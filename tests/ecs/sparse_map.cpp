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

#include <stellarlib/ecs/sparse_map.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <ranges>
#include <utility>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

constexpr std::array<std::size_t, 5> KEYS{2, 1, 0, 3, 4};
static const std::array<std::shared_ptr<std::int32_t>, KEYS.size()> VALUES{
	std::make_shared<std::int32_t>(0),
	std::make_shared<std::int32_t>(5),
	std::make_shared<std::int32_t>(10),
	std::make_shared<std::int32_t>(15),
	std::make_shared<std::int32_t>(25)
};

namespace
{
void check_ranges_mut(internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> &map)
{
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		ASSERT_TRUE(map.contains(key));
		ASSERT_EQ(*map.at(key), value);
		ASSERT_EQ(map[key], value);
	}
	ASSERT_TRUE(std::ranges::equal(map.keys(), KEYS));
	ASSERT_TRUE(std::ranges::equal(map.values(), VALUES));
	ASSERT_TRUE(std::ranges::equal(map.zip(), std::views::zip(KEYS, VALUES)));
}

void check_ranges_const(const internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> &map)
{
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		ASSERT_TRUE(map.contains(key));
		ASSERT_EQ(*map.at(key), value);
		ASSERT_EQ(map[key], value);
	}
	ASSERT_TRUE(std::ranges::equal(map.keys(), KEYS));
	ASSERT_TRUE(std::ranges::equal(map.values(), VALUES));
	ASSERT_TRUE(std::ranges::equal(map.zip(), std::views::zip(KEYS, VALUES)));
}
}

TEST(stellarlib_ecs_sparse_map, should_init_via_ctor)
{
	const internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map{};
	ASSERT_TRUE(map.keys().empty());
	ASSERT_TRUE(map.values().empty());
	ASSERT_TRUE(map.zip().empty());
}

TEST(stellarlib_ecs_sparse_map, should_copy_via_ctor)
{
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map1{};
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		map1.insert(key, value);
	}
	auto map2{map1};
	check_ranges_mut(map2);
	check_ranges_const(map2);
}

TEST(stellarlib_ecs_sparse_map, should_move_via_ctor)
{
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map1{};
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		map1.insert(key, value);
	}
	auto map2{std::move(map1)};
	check_ranges_mut(map2);
	check_ranges_const(map2);
}

TEST(stellarlib_ecs_sparse_map, should_copy_via_assignment)
{
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map1{};
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		map1.insert(key, value);
	}
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map2{};
	map2 = map1;
	check_ranges_mut(map2);
	check_ranges_const(map2);
}

TEST(stellarlib_ecs_sparse_map, should_move_via_assignment)
{
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map1{};
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		map1.insert(key, value);
	}
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map2{};
	map2 = std::move(map1);
	check_ranges_mut(map2);
	check_ranges_const(map2);
}

TEST(stellarlib_ecs_sparse_map, should_copy_via_clone)
{
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map1{};
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		map1.insert(key, value);
	}
	std::unique_ptr<internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>>> map2{map1.clone()};
	check_ranges_mut(*map2);
	check_ranges_const(*map2);
}

TEST(stellarlib_ecs_sparse_map, should_insert_and_erase_values)
{
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map{};
	for (const auto pair : std::views::zip(KEYS, VALUES)) {
		map.insert(std::get<0>(pair), std::get<1>(pair));
		ASSERT_TRUE(map.contains(std::get<0>(pair)));
		ASSERT_EQ(*map.at(std::get<0>(pair)), std::get<1>(pair));
		ASSERT_EQ(map[std::get<0>(pair)], std::get<1>(pair));
		ASSERT_EQ(*std::ranges::find(map.keys(), std::get<0>(pair)), std::get<0>(pair));
		ASSERT_EQ(*std::ranges::find(map.values(), std::get<1>(pair)), std::get<1>(pair));
		ASSERT_EQ(*std::ranges::find(map.zip(), pair), pair);
		map.erase(std::get<0>(pair));
		ASSERT_FALSE(map.contains(std::get<0>(pair)));
		ASSERT_FALSE(map.at(std::get<0>(pair)));
		ASSERT_EQ(std::ranges::find(map.keys(), std::get<0>(pair)), map.keys().end());
		ASSERT_EQ(std::ranges::find(map.values(), std::get<1>(pair)), map.values().end());
		ASSERT_EQ(std::ranges::find(map.zip(), pair), map.zip().end());
		map.insert(std::get<0>(pair), std::get<1>(pair));
		ASSERT_TRUE(map.contains(std::get<0>(pair)));
		ASSERT_EQ(*map.at(std::get<0>(pair)), std::get<1>(pair));
		ASSERT_EQ(map[std::get<0>(pair)], std::get<1>(pair));
		ASSERT_EQ(*std::ranges::find(map.keys(), std::get<0>(pair)), std::get<0>(pair));
		ASSERT_EQ(*std::ranges::find(map.values(), std::get<1>(pair)), std::get<1>(pair));
		ASSERT_EQ(*std::ranges::find(map.zip(), pair), pair);
	}
	check_ranges_mut(map);
	check_ranges_const(map);
}

TEST(stellarlib_ecs_sparse_map, should_reinsert_value)
{
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map{};
	const auto pair{std::views::zip(KEYS, VALUES).front()};
	map.insert(std::get<0>(pair), std::get<1>(pair));
	map.insert(std::get<0>(pair), std::get<1>(pair));
	ASSERT_TRUE(map.contains(std::get<0>(pair)));
	ASSERT_EQ(*map.at(std::get<0>(pair)), std::get<1>(pair));
	ASSERT_EQ(map[std::get<0>(pair)], std::get<1>(pair));
	ASSERT_EQ(map.keys().size(), 1);
	ASSERT_EQ(map.keys().front(), std::get<0>(pair));
	ASSERT_EQ(map.values().size(), 1);
	ASSERT_EQ(map.values().front(), std::get<1>(pair));
	ASSERT_EQ(map.zip().size(), 1);
	ASSERT_EQ(map.zip().front(), pair);
}

TEST(stellarlib_ecs_sparse_map, should_clear_values)
{
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map{};
	for (const auto [key, value] : std::ranges::reverse_view{std::views::zip(KEYS, VALUES)}) {
		map.insert(key, value);
	}
	map.clear();
	ASSERT_TRUE(map.keys().empty());
	ASSERT_TRUE(map.values().empty());
	ASSERT_TRUE(map.zip().empty());
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		map.insert(key, value);
	}
	check_ranges_mut(map);
	check_ranges_const(map);
}

/* NOLINTEND(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
