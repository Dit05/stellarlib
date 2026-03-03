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

#include <stellarlib/ecs/world.hpp>

#include <stellarlib/ext/functional.hpp>

#include <cstdint>
#include <iterator>
#include <ranges>
#include <string>
#include <tuple>

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
		ASSERT_EQ(archetype, (ecs::world::get_archetype<std::int32_t, std::string>)());
		ASSERT_EQ(archetype, (ecs::world::get_archetype<std::string, std::int32_t>)());
		ASSERT_EQ(*world.at(entity), (ecs::world::get_archetype<std::int32_t, std::string>)());
		ASSERT_EQ(*world.at(entity), (ecs::world::get_archetype<std::string, std::int32_t>)());
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
		ASSERT_EQ(world[entity], (ecs::world::get_archetype<std::int32_t, std::string>)());
		ASSERT_EQ(world[entity], (ecs::world::get_archetype<std::string, std::int32_t>)());
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
		ASSERT_EQ(std::ranges::distance(world.query()), i);
		ASSERT_FALSE(world.contains(i));
		ASSERT_FALSE(world.at(i));
		ASSERT_FALSE(std::get<0>(world.at<std::int32_t>(i)));
		ASSERT_FALSE(std::get<0>(world.at<std::string>(i)));
		ASSERT_FALSE(std::get<0>(world.at<std::int32_t, std::string>(i)));
		ASSERT_FALSE(std::get<1>(world.at<std::int32_t, std::string>(i)));
		ASSERT_FALSE(std::get<0>(world.at<std::string, std::int32_t>(i)));
		ASSERT_FALSE(std::get<1>(world.at<std::string, std::int32_t>(i)));
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

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
