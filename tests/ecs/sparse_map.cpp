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
#include <stdexcept>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

constexpr std::array<std::size_t, 5> KEYS{2, 1, 0, 3, 4};
const std::array<std::shared_ptr<std::int32_t>, KEYS.size()> VALUES{
	std::make_shared<std::int32_t>(0),
	std::make_shared<std::int32_t>(5),
	std::make_shared<std::int32_t>(10),
	std::make_shared<std::int32_t>(15),
	std::make_shared<std::int32_t>(25)
};

namespace
{
constexpr void check_pairs(const internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> &map)
{
	ASSERT_EQ(map.size(), KEYS.size());
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		ASSERT_TRUE(map.contains(key));
		ASSERT_TRUE(map.at(key));
		ASSERT_EQ(*map.at(key), value);
		ASSERT_EQ(map[key], value);
	}
	ASSERT_TRUE(std::ranges::equal(map.keys(), KEYS));
	ASSERT_TRUE(std::ranges::equal(map.values(), VALUES));
	ASSERT_TRUE(std::ranges::equal(map.zip(), std::views::zip(KEYS, VALUES)));
}
}

TEST(stellarlib_ecs_sparse_map, should_throw_on_non_copy_constructible_clone)
{
	ASSERT_THROW((internal::sparse_map<std::size_t, std::unique_ptr<std::int32_t>>{}).clone(), std::runtime_error);
}

TEST(stellarlib_ecs_sparse_map, should_copy_via_clone)
{
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map1{};
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		map1.insert(key, value);
	}
	const std::unique_ptr<internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>>> map2{map1.clone()};
	check_pairs(*map2);
}

TEST(stellarlib_ecs_sparse_map, should_insert_and_erase_pairs)
{
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map{};
	for (const auto i : std::views::iota(std::size_t{}, KEYS.size())) {
		map.insert(KEYS[i], VALUES[i]);
		map.insert(KEYS[i], new int32_t{});
		map.insert(KEYS[i], VALUES[i]);
		ASSERT_EQ(map.size(), i + 1);
		ASSERT_TRUE(map.contains(KEYS[i]));
		ASSERT_TRUE(map.at(KEYS[i]));
		ASSERT_EQ(*map.at(KEYS[i]), VALUES[i]);
		ASSERT_EQ(map[KEYS[i]], VALUES[i]);
		ASSERT_NE(std::ranges::find(map.keys(), KEYS[i]), map.keys().end());
		ASSERT_NE(std::ranges::find(map.values(), VALUES[i]), map.values().end());
		ASSERT_NE(std::ranges::find(map.zip(), {KEYS[i], VALUES[i]}), map.zip().end());
		map.erase(KEYS[i / 2]);
		map.erase(KEYS[i / 2]);
		ASSERT_EQ(map.size(), i);
		ASSERT_FALSE(map.contains(KEYS[i / 2]));
		ASSERT_FALSE(map.at(KEYS[i / 2]));
		ASSERT_EQ(std::ranges::find(map.keys(), KEYS[i / 2]), map.keys().end());
		ASSERT_EQ(std::ranges::find(map.values(), VALUES[i / 2]), map.values().end());
		ASSERT_EQ(std::ranges::find(map.zip(), {KEYS[i / 2], VALUES[i / 2]}), map.zip().end());
		map.insert(KEYS[i / 2], VALUES[i / 2]);
		map.insert(KEYS[i / 2], new int32_t{});
		map.insert(KEYS[i / 2], VALUES[i / 2]);
		ASSERT_EQ(map.size(), i + 1);
		ASSERT_TRUE(map.contains(KEYS[i / 2]));
		ASSERT_TRUE(map.at(KEYS[i / 2]));
		ASSERT_EQ(*map.at(KEYS[i / 2]), VALUES[i / 2]);
		ASSERT_EQ(map[KEYS[i / 2]], VALUES[i / 2]);
		ASSERT_NE(std::ranges::find(map.keys(), KEYS[i / 2]), map.keys().end());
		ASSERT_NE(std::ranges::find(map.values(), VALUES[i / 2]), map.values().end());
		ASSERT_NE(std::ranges::find(map.zip(), {KEYS[i / 2], VALUES[i / 2]}), map.zip().end());
		map.erase(KEYS[i]);
		map.erase(KEYS[i]);
		ASSERT_EQ(map.size(), i);
		ASSERT_FALSE(map.contains(KEYS[i]));
		ASSERT_FALSE(map.at(KEYS[i]));
		ASSERT_EQ(std::ranges::find(map.keys(), KEYS[i]), map.keys().end());
		ASSERT_EQ(std::ranges::find(map.values(), VALUES[i]), map.values().end());
		ASSERT_EQ(std::ranges::find(map.zip(), {KEYS[i], VALUES[i]}), map.zip().end());
		map.insert(KEYS[i], VALUES[i]);
		map.insert(KEYS[i], new int32_t{});
		map.insert(KEYS[i], VALUES[i]);
		ASSERT_EQ(map.size(), i + 1);
		ASSERT_TRUE(map.contains(KEYS[i]));
		ASSERT_TRUE(map.at(KEYS[i]));
		ASSERT_EQ(*map.at(KEYS[i]), VALUES[i]);
		ASSERT_EQ(map[KEYS[i]], VALUES[i]);
		ASSERT_NE(std::ranges::find(map.keys(), KEYS[i]), map.keys().end());
		ASSERT_NE(std::ranges::find(map.values(), VALUES[i]), map.values().end());
		ASSERT_NE(std::ranges::find(map.zip(), {KEYS[i], VALUES[i]}), map.zip().end());
	}
	check_pairs(map);
}

TEST(stellarlib_ecs_sparse_map, should_clear_pairs)
{
	internal::sparse_map<std::size_t, std::shared_ptr<std::int32_t>> map{};
	for (const auto [key, value] : std::views::zip(KEYS, VALUES) | std::views::reverse) {
		map.insert(key, value);
	}
	map.clear();
	ASSERT_FALSE(map.size());
	for (const auto key : KEYS) {
		ASSERT_FALSE(map.contains(key));
		ASSERT_FALSE(map.at(key));
	}
	ASSERT_TRUE(map.keys().empty());
	ASSERT_TRUE(map.values().empty());
	ASSERT_TRUE(map.zip().empty());
	for (const auto [key, value] : std::views::zip(KEYS, VALUES)) {
		map.insert(key, value);
	}
	check_pairs(map);
}

/* NOLINTEND(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
