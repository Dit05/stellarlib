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
constexpr auto number_of(const std::uint32_t entity) noexcept
{
	return static_cast<std::int32_t>(entity * 5);
}

[[nodiscard]]
constexpr auto string_of(const std::uint32_t entity) noexcept
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
		ASSERT_EQ(*std::get<0>(world.at<std::int32_t>(entity)), number_of(entity));
		ASSERT_TRUE(std::get<0>(world.at<std::string>(entity)));
		ASSERT_EQ(*std::get<0>(world.at<std::string>(entity)), string_of(entity));
		ASSERT_TRUE(std::get<0>(world.at<std::int32_t, std::string>(entity)));
		ASSERT_EQ(*std::get<0>(world.at<std::int32_t, std::string>(entity)), number_of(entity));
		ASSERT_TRUE(std::get<1>(world.at<std::int32_t, std::string>(entity)));
		ASSERT_EQ(*std::get<1>(world.at<std::int32_t, std::string>(entity)), string_of(entity));
		ASSERT_TRUE(std::get<0>(world.at<std::string, std::int32_t>(entity)));
		ASSERT_EQ(*std::get<0>(world.at<std::string, std::int32_t>(entity)), string_of(entity));
		ASSERT_TRUE(std::get<1>(world.at<std::string, std::int32_t>(entity)));
		ASSERT_EQ(*std::get<1>(world.at<std::string, std::int32_t>(entity)), number_of(entity));
		ASSERT_EQ(world[entity], (ecs::archetype::of<std::int32_t, std::string>)());
		ASSERT_EQ(world[entity], (ecs::archetype::of<std::string, std::int32_t>)());
		ASSERT_EQ(world.operator[]<std::int32_t>(entity), std::tuple{number_of(entity)});
		ASSERT_EQ(world.operator[]<std::string>(entity), std::tuple{string_of(entity)});
		ASSERT_EQ((world.operator[]<std::int32_t, std::string>)(entity), (std::tuple{number_of(entity), string_of(entity)}));
		ASSERT_EQ((world.operator[]<std::string, std::int32_t>)(entity), (std::tuple{string_of(entity), number_of(entity)}));
	}
	ASSERT_EQ(world.query<std::int32_t>().size(), std::ranges::distance(world.query()));
	for (const auto [entity, number] : world.query<std::int32_t>()) {
		ASSERT_EQ(number, number_of(entity));
	}
	ASSERT_EQ(world.query<std::string>().size(), std::ranges::distance(world.query()));
	for (const auto [entity, string] : world.query<std::string>()) {
		ASSERT_EQ(string, string_of(entity));
	}
	ASSERT_EQ(std::ranges::distance(world.query<std::int32_t, std::string>()), std::ranges::distance(world.query()));
	for (const auto [entity, number, string] : world.query<std::int32_t, std::string>()) {
		ASSERT_EQ(number, number_of(entity));
		ASSERT_EQ(string, string_of(entity));
	}
	ASSERT_EQ(std::ranges::distance(world.query<std::string, std::int32_t>()), std::ranges::distance(world.query()));
	for (const auto [entity, string, number] : world.query<std::string, std::int32_t>()) {
		ASSERT_EQ(string, string_of(entity));
		ASSERT_EQ(number, number_of(entity));
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
	ASSERT_EQ(*std::get<0>(world.at<std::int32_t>(entity)), number_of(entity));
	ASSERT_FALSE(std::get<0>(world.at<std::string>(entity)));
	ASSERT_TRUE(std::get<0>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::int32_t, std::string>(entity)), number_of(entity));
	ASSERT_FALSE(std::get<1>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_FALSE(std::get<0>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_TRUE(std::get<1>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_EQ(*std::get<1>(world.at<std::string, std::int32_t>(entity)), number_of(entity));
	ASSERT_EQ(world[entity], ecs::archetype::of<std::int32_t>());
	ASSERT_EQ(std::get<0>(world.operator[]<std::int32_t>(entity)), number_of(entity));
	ASSERT_NE(std::ranges::find(world.query<std::int32_t>(), std::tuple{entity, number_of(entity)}), world.query<std::int32_t>().end());
	ASSERT_EQ(std::ranges::find(world.query<std::string>(), std::tuple{entity, string_of(entity)}), world.query<std::string>().end());
	auto query1{world.query<std::int32_t, std::string>()};
	ASSERT_EQ(std::ranges::find(query1, std::tuple{entity, number_of(entity), string_of(entity)}), query1.end());
	auto query2{world.query<std::string, std::int32_t>()};
	ASSERT_EQ(std::ranges::find(query2, std::tuple{entity, string_of(entity), number_of(entity)}), query2.end());
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
	ASSERT_EQ(*std::get<0>(world.at<std::string>(entity)), string_of(entity));
	ASSERT_FALSE(std::get<0>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_TRUE(std::get<1>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_EQ(*std::get<1>(world.at<std::int32_t, std::string>(entity)), string_of(entity));
	ASSERT_TRUE(std::get<0>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::string, std::int32_t>(entity)), string_of(entity));
	ASSERT_FALSE(std::get<1>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_EQ(world[entity], ecs::archetype::of<std::string>());
	ASSERT_EQ(std::get<0>(world.operator[]<std::string>(entity)), string_of(entity));
	ASSERT_EQ(std::ranges::find(world.query<std::int32_t>(), std::tuple{entity, number_of(entity)}), world.query<std::int32_t>().end());
	ASSERT_NE(std::ranges::find(world.query<std::string>(), std::tuple{entity, string_of(entity)}), world.query<std::string>().end());
	auto query1{world.query<std::int32_t, std::string>()};
	ASSERT_EQ(std::ranges::find(query1, std::tuple{entity, number_of(entity), string_of(entity)}), query1.end());
	auto query2{world.query<std::string, std::int32_t>()};
	ASSERT_EQ(std::ranges::find(query2, std::tuple{entity, string_of(entity), number_of(entity)}), query2.end());
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
	ASSERT_EQ(*std::get<0>(world.at<std::int32_t>(entity)), number_of(entity));
	ASSERT_TRUE(std::get<0>(world.at<std::string>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::string>(entity)), string_of(entity));
	ASSERT_TRUE(std::get<0>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::int32_t, std::string>(entity)), number_of(entity));
	ASSERT_TRUE(std::get<1>(world.at<std::int32_t, std::string>(entity)));
	ASSERT_EQ(*std::get<1>(world.at<std::int32_t, std::string>(entity)), string_of(entity));
	ASSERT_TRUE(std::get<0>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_EQ(*std::get<0>(world.at<std::string, std::int32_t>(entity)), string_of(entity));
	ASSERT_TRUE(std::get<1>(world.at<std::string, std::int32_t>(entity)));
	ASSERT_EQ(*std::get<1>(world.at<std::string, std::int32_t>(entity)), number_of(entity));
	ASSERT_EQ(world[entity], (ecs::archetype::of<std::int32_t, std::string>)());
	ASSERT_EQ(world[entity], (ecs::archetype::of<std::string, std::int32_t>)());
	ASSERT_EQ(world.operator[]<std::int32_t>(entity), std::tuple{number_of(entity)});
	ASSERT_EQ(world.operator[]<std::string>(entity), std::tuple{string_of(entity)});
	ASSERT_EQ((world.operator[]<std::int32_t, std::string>)(entity), (std::tuple{number_of(entity), string_of(entity)}));
	ASSERT_EQ((world.operator[]<std::string, std::int32_t>)(entity), (std::tuple{string_of(entity), number_of(entity)}));
	ASSERT_NE(std::ranges::find(world.query<std::int32_t>(), std::tuple{entity, number_of(entity)}), world.query<std::int32_t>().end());
	ASSERT_NE(std::ranges::find(world.query<std::string>(), std::tuple{entity, string_of(entity)}), world.query<std::string>().end());
	auto query1{world.query<std::int32_t, std::string>()};
	ASSERT_NE(std::ranges::find(query1, std::tuple{entity, number_of(entity), string_of(entity)}), query1.end());
	auto query2{world.query<std::string, std::int32_t>()};
	ASSERT_NE(std::ranges::find(query2, std::tuple{entity, string_of(entity), number_of(entity)}), query2.end());
}
}

TEST(stellarlib_ecs_world, should_copy_via_ctor)
{
	ecs::world world1{};
	for (const auto entity : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(entity % 2)) {
			world1.spawn(number_of(entity), string_of(entity));
		}
		else {
			world1.spawn(string_of(entity), number_of(entity));
		}
	}
	auto world2{world1};
	check_entities(world2);
}

TEST(stellarlib_ecs_world, should_move_via_ctor)
{
	ecs::world world1{};
	for (const auto entity : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(entity % 2)) {
			world1.spawn(number_of(entity), string_of(entity));
		}
		else {
			world1.spawn(string_of(entity), number_of(entity));
		}
	}
	auto world2{std::move(world1)};
	check_entities(world2);
}

TEST(stellarlib_ecs_world, should_skip_self_copy_via_assignment)
{
	ecs::world world{};
	for (const auto entity : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(entity % 2)) {
			world.spawn(number_of(entity), string_of(entity));
		}
		else {
			world.spawn(string_of(entity), number_of(entity));
		}
	}
	world = world;
	check_entities(world);
}

TEST(stellarlib_ecs_world, should_copy_via_assignment)
{
	ecs::world world1{};
	for (const auto entity : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(entity % 2)) {
			world1.spawn(number_of(entity), string_of(entity));
		}
		else {
			world1.spawn(string_of(entity), number_of(entity));
		}
	}
	ecs::world world2{};
	world2 = world1;
	check_entities(world2);
}

TEST(stellarlib_ecs_world, should_skip_self_move_via_assignment)
{
	ecs::world world{};
	for (const auto entity : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(entity % 2)) {
			world.spawn(number_of(entity), string_of(entity));
		}
		else {
			world.spawn(string_of(entity), number_of(entity));
		}
	}
	world = std::move(world);
	check_entities(world);
}

TEST(stellarlib_ecs_world, should_move_via_assignment)
{
	ecs::world world1{};
	for (const auto entity : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(entity % 2)) {
			world1.spawn(number_of(entity), string_of(entity));
		}
		else {
			world1.spawn(string_of(entity), number_of(entity));
		}
	}
	ecs::world world2{};
	world2 = std::move(world1);
	check_entities(world2);
}

TEST(stellarlib_ecs_world, should_spawn_and_despawn_entities)
{
	ecs::world world{};
	for (const auto entity : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(entity % 2)) {
			world.spawn(number_of(entity), string_of(entity));
		}
		else {
			world.spawn(string_of(entity), number_of(entity));
		}
		ASSERT_EQ(std::ranges::distance(world.query()), entity + 1);
		check_entities(world);
		world.despawn(entity);
		world.despawn(entity);
		check_despawned_entity(world, entity);
		ASSERT_EQ(std::ranges::distance(world.query()), entity);
		check_entities(world);
		if (ext::truthy(entity % 2)) {
			world.spawn(string_of(entity), number_of(entity));
		}
		else {
			world.spawn(number_of(entity), string_of(entity));
		}
		ASSERT_EQ(std::ranges::distance(world.query()), entity + 1);
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
	ASSERT_EQ(world.insert({}, number_of({})).error(), std::tuple{number_of({})});
	ASSERT_EQ(world.insert({}, string_of({})).error(), std::tuple{string_of({})});
	ASSERT_EQ(world.insert({}, number_of({}), string_of({})).error(), (std::tuple{number_of({}), string_of({})}));
	ASSERT_EQ(world.insert({}, string_of({}), number_of({})).error(), (std::tuple{string_of({}), number_of({})}));
	for (const auto entity : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(entity % 2)) {
			world.spawn(number_of(entity));
			check_entity_with_number_only(world, entity);
			*world.insert(entity, string_of(entity));
			*world.insert(entity, string_of(entity));
			check_entity_with_components(world, entity);
			world.erase<std::string, std::int32_t>(entity);
			world.erase<std::string, std::int32_t>(entity);
			check_entity_without_components(world, entity);
			*world.insert(entity, number_of(entity), string_of(entity));
			*world.insert(entity, number_of(entity), string_of(entity));
			check_entity_with_components(world, entity);
		}
		else {
			world.spawn();
			check_entity_without_components(world, entity);
			*world.insert(entity, string_of(entity), number_of(entity));
			*world.insert(entity, string_of(entity), number_of(entity));
			check_entity_with_components(world, entity);
			world.erase<std::int32_t>(entity);
			world.erase<std::int32_t>(entity);
			check_entity_with_string_only(world, entity);
			world.erase<std::string>(entity);
			world.erase<std::string>(entity);
			check_entity_without_components(world, entity);
			*world.insert(entity, string_of(entity), number_of(entity));
			*world.insert(entity, string_of(entity), number_of(entity));
			check_entity_with_components(world, entity);
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
	for (const auto enttiy : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(enttiy % 2)) {
			world.spawn(number_of(enttiy), string_of(enttiy));
		}
		else {
			world.spawn(string_of(enttiy), number_of(enttiy));
		}
	}
	world.clear();
	for (const auto entity : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		check_despawned_entity(world, entity);
	}
	ASSERT_FALSE(std::ranges::distance(world.query()));
	ASSERT_FALSE(std::ranges::distance(world.query<std::uint32_t>()));
	ASSERT_FALSE(std::ranges::distance(world.query<std::string>()));
	ASSERT_FALSE(std::ranges::distance(world.query<std::uint32_t, std::string>()));
	ASSERT_FALSE(std::ranges::distance(world.query<std::string, std::uint32_t>()));
	for (const auto entity : std::views::iota(std::uint32_t{}, std::uint32_t{10})) {
		if (ext::truthy(entity % 2)) {
			world.spawn(string_of(entity), number_of(entity));
		}
		else {
			world.spawn(number_of(entity), string_of(entity));
		}
	}
	check_entities(world);
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
