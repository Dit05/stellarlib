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

#include <stellarlib/ecs/sparse_storage.hpp>
#include <stellarlib/ext/bit.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <memory>
#include <ranges>
#include <utility>

using namespace stellarlib;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

class foo final {};

class bar final {};

class baz final {};

constexpr std::array<std::uintmax_t, 3> IDS{
	std::numeric_limits<std::uintmax_t>::digits * 1 - 1,
	std::numeric_limits<std::uintmax_t>::digits * 3 - 1,
	std::numeric_limits<std::uintmax_t>::digits * 2 - 1
};

static_assert(std::ranges::find(IDS, (std::ranges::min(IDS) + std::ranges::max(IDS)) / 2) != IDS.end());
static_assert(ext::bit_index(IDS[0]) < ext::bit_index(IDS[1]));
static_assert(ext::bit_index(IDS[0]) < ext::bit_index(IDS[2]));
static_assert(ext::bit_index(IDS[2]) < ext::bit_index(IDS[1]));

namespace
{
constexpr void check_ids(const ecs::archetype &archetype)
{
	for (const auto id : std::views::iota(std::uintmax_t{}, std::ranges::max(IDS))) {
		ASSERT_EQ(archetype.contains(id), std::ranges::find(IDS, id) != IDS.end());
	}
}
}

TEST(stellarlib_ecs_archetype, should_cache_archetype)
{
	static_cast<void>(ecs::internal::sparse_storage::ids<foo, bar, baz>());
	ASSERT_EQ(std::addressof(ecs::archetype::of<foo, baz>()), std::addressof(ecs::archetype::of<foo, baz>()));
	ASSERT_TRUE((ecs::archetype::of<foo, baz>)().contains(ecs::internal::sparse_storage::ids<foo>().front()));
	ASSERT_FALSE((ecs::archetype::of<foo, baz>)().contains(ecs::internal::sparse_storage::ids<bar>().front()));
	ASSERT_TRUE((ecs::archetype::of<foo, baz>)().contains(ecs::internal::sparse_storage::ids<baz>().front()));
}

TEST(stellarlib_ecs_archetype, should_copy_via_ctor)
{
	ecs::archetype archetype1{};
	for (const auto id : IDS) {
		archetype1.insert(id);
	}
	const auto archetype2{archetype1};
	check_ids(archetype2);
}

TEST(stellarlib_ecs_archetype, should_move_via_ctor)
{
	ecs::archetype archetype1{};
	for (const auto id : IDS) {
		archetype1.insert(id);
	}
	const auto archetype2{std::move(archetype1)};
	check_ids(archetype2);
}

TEST(stellarlib_ecs_archetype, should_skip_self_copy_via_assignment)
{
	ecs::archetype archetype{};
	for (const auto id : IDS) {
		archetype.insert(id);
	}
	archetype = archetype;
	check_ids(archetype);
}

TEST(stellarlib_ecs_archetype, should_copy_via_assignment)
{
	ecs::archetype archetype1{};
	for (const auto id : IDS) {
		archetype1.insert(id);
	}
	ecs::archetype archetype2{};
	archetype2 = archetype1;
	check_ids(archetype2);
}

TEST(stellarlib_ecs_archetype, should_skip_self_move_via_assignment)
{
	ecs::archetype archetype{};
	for (const auto id : IDS) {
		archetype.insert(id);
	}
	archetype = std::move(archetype);
	check_ids(archetype);
}

TEST(stellarlib_ecs_archetype, should_move_via_assignment)
{
	ecs::archetype archetype1{};
	for (const auto id : IDS) {
		archetype1.insert(id);
	}
	ecs::archetype archetype2{};
	archetype2 = std::move(archetype1);
	check_ids(archetype2);
}

TEST(stellarlib_ecs_archetype, should_insert_and_erase_ids)
{
	ecs::archetype archetype{};
	for (const auto id : IDS) {
		archetype.insert(id);
		ASSERT_TRUE(archetype.contains(id));
		archetype.erase(id);
		archetype.erase(id);
		ASSERT_FALSE(archetype.contains(id));
		archetype.insert(id);
		ASSERT_TRUE(archetype.contains(id));
	}
	check_ids(archetype);
}

TEST(stellarlib_ecs_archetype, should_insert_archetype)
{
	ecs::archetype archetype1{};
	archetype1.insert(std::ranges::min(IDS) / 2);
	archetype1.insert((std::ranges::min(IDS) + std::ranges::max(IDS)) / 2);
	archetype1.erase((std::ranges::min(IDS) + std::ranges::max(IDS)) / 2);
	ecs::archetype archetype2{};
	for (const auto id : IDS) {
		archetype2.insert(id);
	}
	archetype1.insert(archetype2);
	ASSERT_TRUE(archetype1.contains(std::ranges::min(IDS) / 2));
	archetype1.erase(std::ranges::min(IDS) / 2);
	ASSERT_EQ(archetype1, archetype2);
}

TEST(stellarlib_ecs_archetype, should_evaluate_equal_archetypes)
{
	ecs::archetype archetype1{};
	archetype1.insert((std::ranges::min(IDS) + std::ranges::max(IDS)) / 2);
	archetype1.insert(std::ranges::min(IDS));
	archetype1.erase(std::ranges::min(IDS));
	ecs::archetype archetype2{};
	archetype2.insert((std::ranges::min(IDS) + std::ranges::max(IDS)) / 2);
	archetype2.insert(std::ranges::max(IDS));
	archetype2.erase(std::ranges::max(IDS));
	ASSERT_EQ(archetype1, archetype2);
	ASSERT_EQ(archetype2, archetype1);
	ASSERT_TRUE(archetype1 <= archetype2);
	ASSERT_TRUE(archetype2 <= archetype1);
	ASSERT_TRUE(archetype2 >= archetype1);
	ASSERT_TRUE(archetype1 >= archetype2);
}


TEST(stellarlib_ecs_archetype, should_evaluate_unequal_archetypes)
{
	ecs::archetype archetype1{};
	archetype1.insert(std::ranges::min(IDS));
	ecs::archetype archetype2{};
	archetype2.insert(std::ranges::max(IDS));
	ASSERT_NE(archetype1, archetype2);
	ASSERT_NE(archetype2, archetype1);
	ASSERT_FALSE(archetype1 <= archetype2);
	ASSERT_FALSE(archetype2 <= archetype1);
	ASSERT_FALSE(archetype2 >= archetype1);
	ASSERT_FALSE(archetype1 >= archetype2);
}

TEST(stellarlib_ecs_archetype, should_evaluate_subtype_and_supertype)
{
	ecs::archetype subarchetype{};
	subarchetype.insert(std::ranges::min(IDS));
	subarchetype.insert(std::ranges::max(IDS));
	subarchetype.erase(std::ranges::max(IDS));
	ecs::archetype superarchetype{};
	superarchetype.insert(std::ranges::min(IDS));
	superarchetype.insert((std::ranges::min(IDS) + std::ranges::max(IDS)) / 2);
	ASSERT_TRUE(subarchetype <= superarchetype);
	ASSERT_FALSE(superarchetype <= subarchetype);
	ASSERT_TRUE(superarchetype >= subarchetype);
	ASSERT_FALSE(subarchetype >= superarchetype);
}

TEST(stellarlib_ecs_archetype, should_erase_archetype)
{
	ecs::archetype archetype1{};
	for (const auto id : IDS) {
		archetype1.insert(id);
	}
	const auto archetype2{archetype1};
	archetype1.insert(std::ranges::min(IDS) / 2);
	archetype1.erase(archetype2);
	archetype1.erase(archetype2);
	for (const auto id : std::views::iota(std::uintmax_t{}, std::ranges::max(IDS))) {
		ASSERT_EQ(archetype1.contains(id), id == std::ranges::min(IDS) / 2);
	}
	archetype1.erase(archetype1);
	archetype1.erase(archetype1);
	ASSERT_EQ(archetype1, ecs::archetype{});
}

TEST(stellarlib_ecs_archetype, should_clear_ids)
{
	ecs::archetype archetype{};
	for (const auto id : std::views::iota(std::uintmax_t{}, std::ranges::max(IDS))) {
		archetype.insert(id);
	}
	archetype.clear();
	for (const auto id : IDS) {
		archetype.insert(id);
	}
	check_ids(archetype);
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
