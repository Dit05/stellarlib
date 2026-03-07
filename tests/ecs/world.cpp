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
#include <stellarlib/ecs/world.hpp>

#include <stellarlib/ext/functional.hpp>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <string>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

using namespace stellarlib;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

namespace
{
[[nodiscard]]
constexpr auto get_number(const uint32_t entity) noexcept
{
	return static_cast<std::int32_t>(entity * 5);
}

[[nodiscard]]
constexpr auto get_string(const uint32_t entity) noexcept
{
	return std::to_string(entity * 10);
}

constexpr void check_entities(ecs::world &world) noexcept
{
	for (const auto [entity, archetype] : world.query()) {
		ASSERT_TRUE(world.contains(entity));
		ASSERT_TRUE(std::get<0>(world.contains<std::int32_t>(entity)));
		ASSERT_TRUE(std::get<0>(world.contains<std::string>(entity)));
		ASSERT_TRUE(std::get<0>(world.contains<std::int32_t, std::string>(entity)));
		ASSERT_TRUE(std::get<1>(world.contains<std::int32_t, std::string>(entity)));
		ASSERT_TRUE(std::get<0>(world.contains<std::string, std::int32_t>(entity)));
		ASSERT_TRUE(std::get<1>(world.contains<std::string, std::int32_t>(entity)));
		ASSERT_EQ(archetype, (ecs::archetype::of<std::int32_t, std::string>)());
		ASSERT_EQ(archetype, (ecs::archetype::of<std::string, std::int32_t>)());
		ASSERT_TRUE(world.at(entity));
		ASSERT_EQ(*world.at(entity), (ecs::archetype::of<std::int32_t, std::string>)());
		ASSERT_EQ(*world.at(entity), (ecs::archetype::of<std::string, std::int32_t>)());
		ASSERT_TRUE(std::get<0>(world.at<std::int32_t>(entity)));
		ASSERT_EQ(*std::get<0>(world.at<std::int32_t>(entity)), get_number(entity));
		ASSERT_TRUE(std::get<0>(world.at<std::string>(entity)));
		ASSERT_EQ(*std::get<0>(world.at<std::string>(entity)), get_string(entity));
		ASSERT_TRUE(std::get<0>(world.at<std::int32_t, std::string>(entity)));
		ASSERT_EQ(*std::get<0>(world.at<std::int32_t, std::string>(entity)), get_number(entity));
		ASSERT_TRUE(std::get<1>(world.at<std::int32_t, std::string>(entity)));
		ASSERT_EQ(*std::get<1>(world.at<std::int32_t, std::string>(entity)), get_string(entity));
		ASSERT_TRUE(std::get<0>(world.at<std::string, std::int32_t>(entity)));
		ASSERT_EQ(*std::get<0>(world.at<std::string, std::int32_t>(entity)), get_string(entity));
		ASSERT_TRUE(std::get<1>(world.at<std::string, std::int32_t>(entity)));
		ASSERT_EQ(*std::get<1>(world.at<std::string, std::int32_t>(entity)), get_number(entity));
		ASSERT_EQ(world[entity], (ecs::archetype::of<std::int32_t, std::string>)());
		ASSERT_EQ(world[entity], (ecs::archetype::of<std::string, std::int32_t>)());
		ASSERT_EQ(world.operator[]<std::int32_t>(entity), std::tuple{get_number(entity)});
		ASSERT_EQ(world.operator[]<std::string>(entity), std::tuple{get_string(entity)});
		ASSERT_EQ((world.operator[]<std::int32_t, std::string>)(entity), (std::tuple{get_number(entity), get_string(entity)}));
		ASSERT_EQ((world.operator[]<std::string, std::int32_t>)(entity), (std::tuple{get_string(entity), get_number(entity)}));
	}
	ASSERT_EQ(world.query<std::int32_t>().size(), std::ranges::distance(world.query()));
	for (const auto [entity, number] : world.query<std::int32_t>()) {
		ASSERT_EQ(number, get_number(entity));
	}
	ASSERT_EQ(world.query<std::string>().size(), std::ranges::distance(world.query()));
	for (const auto [entity, string] : world.query<std::string>()) {
		ASSERT_EQ(string, get_string(entity));
	}
	ASSERT_EQ(std::ranges::distance(world.query<std::int32_t, std::string>()), std::ranges::distance(world.query()));
	for (const auto [entity, number, string] : world.query<std::int32_t, std::string>()) {
		ASSERT_EQ(number, get_number(entity));
		ASSERT_EQ(string, get_string(entity));
	}
	ASSERT_EQ(std::ranges::distance(world.query<std::string, std::int32_t>()), std::ranges::distance(world.query()));
	for (const auto [entity, string, number] : world.query<std::string, std::int32_t>()) {
		ASSERT_EQ(string, get_string(entity));
		ASSERT_EQ(number, get_number(entity));
	}
}

constexpr void check_despawned_entity(ecs::world &world, std::uint32_t entity) noexcept
{
	ASSERT_FALSE(world.contains(entity));
	ASSERT_FALSE(std::get<0>(world.contains<std::int32_t>(entity)));
	ASSERT_FALSE(std::get<0>(world.contains<std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.contains<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<1>(world.contains<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.contains<std::string, std::int32_t>(entity)));
	ASSERT_FALSE(std::get<1>(world.contains<std::string, std::int32_t>(entity)));
	ASSERT_FALSE(world.at(entity));
	ASSERT_FALSE(std::get<0>(world.at<std::int32_t>(entity)));
	ASSERT_FALSE(std::get<0>(world.at<std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<1>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_FALSE(std::get<1>(world.at<std::string, std::int32_t>(entity)));
}

constexpr void check_entity_without_components(ecs::world &world, std::uint32_t entity) noexcept
{
	ASSERT_TRUE(world.contains(entity));
	ASSERT_FALSE(std::get<0>(world.contains<std::int32_t>(entity)));
	ASSERT_FALSE(std::get<0>(world.contains<std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.contains<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<1>(world.contains<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.contains<std::string, std::int32_t>(entity)));
	ASSERT_FALSE(std::get<1>(world.contains<std::string, std::int32_t>(entity)));
	ASSERT_TRUE(world.at(entity));
	ASSERT_EQ(*world.at(entity), ecs::archetype::of());
	ASSERT_FALSE(std::get<0>(world.at<std::int32_t>(entity)));
	ASSERT_FALSE(std::get<0>(world.at<std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<1>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_FALSE(std::get<1>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_EQ(world[entity], ecs::archetype::of());
}

constexpr void check_entity_with_number_only(ecs::world &world, std::uint32_t entity) noexcept
{
	ASSERT_TRUE(world.contains(entity));
	ASSERT_TRUE(std::get<0>(world.contains<std::int32_t>(entity)));
	ASSERT_FALSE(std::get<0>(world.contains<std::string>(entity)));
	ASSERT_TRUE(std::get<0>(world.contains<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<1>(world.contains<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.contains<std::string, std::int32_t>(entity)));
	ASSERT_TRUE(std::get<1>(world.contains<std::string, std::int32_t>(entity)));
	ASSERT_TRUE(world.at(entity));
	ASSERT_EQ(*world.at(entity), ecs::archetype::of<std::int32_t>());
	ASSERT_TRUE(std::get<0>(world.at<std::int32_t>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::int32_t>(entity)), get_number(entity));
	ASSERT_FALSE(std::get<0>(world.at<std::string>(entity)));
	ASSERT_TRUE(std::get<0>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::int32_t, std::string>(entity)), get_number(entity));
	ASSERT_FALSE(std::get<1>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_TRUE(std::get<1>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_EQ(*std::get<1>(world.at<std::string, std::int32_t>(entity)), get_number(entity));
	ASSERT_EQ(world[entity], ecs::archetype::of<std::int32_t>());
	ASSERT_EQ(std::get<0>(world.operator[]<std::int32_t>(entity)), get_number(entity));
	ASSERT_NE(std::ranges::find(world.query<std::int32_t>(), std::tuple{entity, get_number(entity)}), world.query<std::int32_t>().end());
	ASSERT_EQ(std::ranges::find(world.query<std::string>(), std::tuple{entity, get_string(entity)}), world.query<std::string>().end());
	auto query1{world.query<std::int32_t, std::string>()};
	ASSERT_EQ(std::ranges::find(query1, std::tuple{entity, get_number(entity), get_string(entity)}), query1.end());
	auto query2{world.query<std::string, std::int32_t>()};
	ASSERT_EQ(std::ranges::find(query2, std::tuple{entity, get_string(entity), get_number(entity)}), query2.end());
}

constexpr void check_entity_with_string_only(ecs::world &world, std::uint32_t entity) noexcept
{
	ASSERT_TRUE(world.contains(entity));
	ASSERT_FALSE(std::get<0>(world.contains<std::int32_t>(entity)));
	ASSERT_TRUE(std::get<0>(world.contains<std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.contains<std::int32_t, std::string>(entity)));
	ASSERT_TRUE(std::get<1>(world.contains<std::int32_t, std::string>(entity)));
	ASSERT_TRUE(std::get<0>(world.contains<std::string, std::int32_t>(entity)));
	ASSERT_FALSE(std::get<1>(world.contains<std::string, std::int32_t>(entity)));
	ASSERT_TRUE(world.at(entity));
	ASSERT_EQ(*world.at(entity), ecs::archetype::of<std::string>());
	ASSERT_FALSE(std::get<0>(world.at<std::int32_t>(entity)));
	ASSERT_TRUE(std::get<0>(world.at<std::string>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::string>(entity)), get_string(entity));
	ASSERT_FALSE(std::get<0>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_TRUE(std::get<1>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_EQ(*std::get<1>(world.at<std::int32_t, std::string>(entity)), get_string(entity));
	ASSERT_TRUE(std::get<0>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::string, std::int32_t>(entity)), get_string(entity));
	ASSERT_FALSE(std::get<1>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_EQ(world[entity], ecs::archetype::of<std::string>());
	ASSERT_EQ(std::get<0>(world.operator[]<std::string>(entity)), get_string(entity));
	ASSERT_EQ(std::ranges::find(world.query<std::int32_t>(), std::tuple{entity, get_number(entity)}), world.query<std::int32_t>().end());
	ASSERT_NE(std::ranges::find(world.query<std::string>(), std::tuple{entity, get_string(entity)}), world.query<std::string>().end());
	auto query1{world.query<std::int32_t, std::string>()};
	ASSERT_EQ(std::ranges::find(query1, std::tuple{entity, get_number(entity), get_string(entity)}), query1.end());
	auto query2{world.query<std::string, std::int32_t>()};
	ASSERT_EQ(std::ranges::find(query2, std::tuple{entity, get_string(entity), get_number(entity)}), query2.end());
}

constexpr void check_entity_with_components(ecs::world &world, std::uint32_t entity) noexcept
{
	ASSERT_TRUE(world.contains(entity));
	ASSERT_TRUE(std::get<0>(world.contains<std::int32_t>(entity)));
	ASSERT_TRUE(std::get<0>(world.contains<std::string>(entity)));
	ASSERT_TRUE(std::get<0>(world.contains<std::int32_t, std::string>(entity)));
	ASSERT_TRUE(std::get<1>(world.contains<std::int32_t, std::string>(entity)));
	ASSERT_TRUE(std::get<0>(world.contains<std::string, std::int32_t>(entity)));
	ASSERT_TRUE(std::get<1>(world.contains<std::string, std::int32_t>(entity)));
	ASSERT_TRUE(world.at(entity));
	ASSERT_EQ(*world.at(entity), (ecs::archetype::of<std::int32_t, std::string>)());
	ASSERT_EQ(*world.at(entity), (ecs::archetype::of<std::string, std::int32_t>)());
	ASSERT_TRUE(std::get<0>(world.at<std::int32_t>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::int32_t>(entity)), get_number(entity));
	ASSERT_TRUE(std::get<0>(world.at<std::string>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::string>(entity)), get_string(entity));
	ASSERT_TRUE(std::get<0>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::int32_t, std::string>(entity)), get_number(entity));
	ASSERT_TRUE(std::get<1>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_EQ(*std::get<1>(world.at<std::int32_t, std::string>(entity)), get_string(entity));
	ASSERT_TRUE(std::get<0>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::string, std::int32_t>(entity)), get_string(entity));
	ASSERT_TRUE(std::get<1>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_EQ(*std::get<1>(world.at<std::string, std::int32_t>(entity)), get_number(entity));
	ASSERT_EQ(world[entity], (ecs::archetype::of<std::int32_t, std::string>)());
	ASSERT_EQ(world[entity], (ecs::archetype::of<std::string, std::int32_t>)());
	ASSERT_EQ(world.operator[]<std::int32_t>(entity), std::tuple{get_number(entity)});
	ASSERT_EQ(world.operator[]<std::string>(entity), std::tuple{get_string(entity)});
	ASSERT_EQ((world.operator[]<std::int32_t, std::string>)(entity), (std::tuple{get_number(entity), get_string(entity)}));
	ASSERT_EQ((world.operator[]<std::string, std::int32_t>)(entity), (std::tuple{get_string(entity), get_number(entity)}));
	ASSERT_NE(std::ranges::find(world.query<std::int32_t>(), std::tuple{entity, get_number(entity)}), world.query<std::int32_t>().end());
	ASSERT_NE(std::ranges::find(world.query<std::string>(), std::tuple{entity, get_string(entity)}), world.query<std::string>().end());
	auto query1{world.query<std::int32_t, std::string>()};
	ASSERT_NE(std::ranges::find(query1, std::tuple{entity, get_number(entity), get_string(entity)}), query1.end());
	auto query2{world.query<std::string, std::int32_t>()};
	ASSERT_NE(std::ranges::find(query2, std::tuple{entity, get_string(entity), get_number(entity)}), query2.end());
}
}

TEST(stellarlib_ecs_world, should_copy_via_ctor)
{
	ecs::world world1{};
	for (const auto i : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(i % 2)) {
			world1.spawn(get_number(i), get_string(i));
		}
		else {
			world1.spawn(get_string(i), get_number(i));
		}
	}
	auto world2{world1};
	check_entities(world2);
}

TEST(stellarlib_ecs_world, should_move_via_ctor)
{
	ecs::world world1{};
	for (const auto i : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(i % 2)) {
			world1.spawn(get_number(i), get_string(i));
		}
		else {
			world1.spawn(get_string(i), get_number(i));
		}
	}
	auto world2{std::move(world1)};
	check_entities(world2);
}

TEST(stellarlib_ecs_world, should_skip_self_copy_via_assignment)
{
	ecs::world world{};
	for (const auto i : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(i % 2)) {
			world.spawn(get_number(i), get_string(i));
		}
		else {
			world.spawn(get_string(i), get_number(i));
		}
	}
	world = world;
	check_entities(world);
}

TEST(stellarlib_ecs_world, should_copy_via_assignment)
{
	ecs::world world1{};
	for (const auto i : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(i % 2)) {
			world1.spawn(get_number(i), get_string(i));
		}
		else {
			world1.spawn(get_string(i), get_number(i));
		}
	}
	ecs::world world2{};
	world2 = world1;
	check_entities(world2);
}

TEST(stellarlib_ecs_world, should_skip_self_move_via_assignment)
{
	ecs::world world{};
	for (const auto i : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(i % 2)) {
			world.spawn(get_number(i), get_string(i));
		}
		else {
			world.spawn(get_string(i), get_number(i));
		}
	}
	world = std::move(world);
	check_entities(world);
}

TEST(stellarlib_ecs_world, should_move_via_assignment)
{
	ecs::world world1{};
	for (const auto i : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(i % 2)) {
			world1.spawn(get_number(i), get_string(i));
		}
		else {
			world1.spawn(get_string(i), get_number(i));
		}
	}
	ecs::world world2{};
	world2 = std::move(world1);
	check_entities(world2);
}

TEST(stellarlib_ecs_world, should_spawn_and_despawn_entities)
{
	ecs::world world{};
	for (const auto i : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(i % 2)) {
			world.spawn(get_number(i), get_string(i));
		}
		else {
			world.spawn(get_string(i), get_number(i));
		}
		ASSERT_EQ(std::ranges::distance(world.query()), i + 1);
		check_entities(world);
		world.despawn(i);
		world.despawn(i);
		check_despawned_entity(world, i);
		ASSERT_EQ(std::ranges::distance(world.query()), i);
		check_entities(world);
		if (ext::truthy(i % 2)) {
			world.spawn(get_string(i), get_number(i));
		}
		else {
			world.spawn(get_number(i), get_string(i));
		}
		ASSERT_EQ(std::ranges::distance(world.query()), i + 1);
		check_entities(world);
	}
}

TEST(stellarlib_ecs_world, should_reuse_entities)
{
	ecs::world world{};
	const auto entity1{world.spawn()};
	const auto entity2{world.spawn()};
	const auto entity3{world.spawn()};
	world.despawn(entity2);
	world.despawn(entity1);
	world.despawn(entity3);
	ASSERT_EQ(world.spawn(), entity3);
	ASSERT_EQ(world.spawn(), entity1);
	ASSERT_EQ(world.spawn(), entity2);
}

TEST(stellarlib_ecs_world, should_insert_and_erase_components)
{
	ecs::world world{};
	ASSERT_EQ(world.insert(0, get_number(0)).error(), std::tuple{get_number(0)});
	ASSERT_EQ(world.insert(0, get_string(0)).error(), std::tuple{get_string(0)});
	ASSERT_EQ(world.insert(0, get_number(0), get_string(0)).error(), (std::tuple{get_number(0), get_string(0)}));
	ASSERT_EQ(world.insert(0, get_string(0), get_number(0)).error(), (std::tuple{get_string(0), get_number(0)}));
	for (const auto i : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(i % 2)) {
			world.spawn(get_number(i));
			check_entity_with_number_only(world, i);
			*world.insert(i, get_string(i));
			*world.insert(i, get_string(i));
			check_entity_with_components(world, i);
			world.erase<std::string, std::int32_t>(i);
			world.erase<std::string, std::int32_t>(i);
			check_entity_without_components(world, i);
			*world.insert(i, get_number(i), get_string(i));
			*world.insert(i, get_number(i), get_string(i));
			check_entity_with_components(world, i);
		}
		else {
			world.spawn();
			check_entity_without_components(world, i);
			*world.insert(i, get_string(i), get_number(i));
			*world.insert(i, get_string(i), get_number(i));
			check_entity_with_components(world, i);
			world.erase<std::int32_t>(i);
			world.erase<std::int32_t>(i);
			check_entity_with_string_only(world, i);
			world.erase<std::string>(i);
			world.erase<std::string>(i);
			check_entity_without_components(world, i);
			*world.insert(i, get_string(i), get_number(i));
			*world.insert(i, get_string(i), get_number(i));
			check_entity_with_components(world, i);
		}
	}
	check_entities(world);
}

TEST(stellarlib_ecs_world, should_modify_components)
{
	ecs::world world{};
	const auto entity{world.spawn(std::int32_t{})};
	++*std::get<0>(world.at<std::int32_t>(entity));
	++std::get<0>(world.operator[]<std::int32_t>(entity));
	++std::get<1>(*world.query<std::int32_t>().begin());
	ASSERT_EQ(*std::get<0>(world.at<std::int32_t>(entity)), 3);
	ASSERT_EQ(std::get<0>(world.operator[]<std::int32_t>(entity)), 3);
	ASSERT_EQ(std::get<1>(*world.query<std::int32_t>().begin()), 3);
}

TEST(stellarlib_ecs_world, should_clear_entities)
{
	ecs::world world{};
	for (const auto i : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(i % 2)) {
			world.spawn(get_number(i), get_string(i));
		}
		else {
			world.spawn(get_string(i), get_number(i));
		}
	}
	world.clear();
	for (const auto i : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		check_despawned_entity(world, i);
	}
	ASSERT_FALSE(std::ranges::distance(world.query()));
	ASSERT_FALSE(std::ranges::distance(world.query<std::uint32_t>()));
	ASSERT_FALSE(std::ranges::distance(world.query<std::string>()));
	ASSERT_FALSE(std::ranges::distance(world.query<std::uint32_t, std::string>()));
	ASSERT_FALSE(std::ranges::distance(world.query<std::string, std::uint32_t>()));
	for (const auto i : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(i % 2)) {
			world.spawn(get_string(i), get_number(i));
		}
		else {
			world.spawn(get_number(i), get_string(i));
		}
	}
	check_entities(world);
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
