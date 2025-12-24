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

#include <stellarlib/ecs/world.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

namespace
{
template <typename T>
void check_insert(const world &world, std::uint32_t entity, const T &component)
{
	ASSERT_TRUE(world.contains(entity));
	ASSERT_TRUE(world.contains<T>(entity));
	ASSERT_TRUE(world.at(entity));
	ASSERT_TRUE(world.at(entity)->contains(world::id_of<T>()));
	ASSERT_TRUE(world.at<T>(entity));
	ASSERT_EQ(*world.at<T>(entity), component);
	ASSERT_TRUE(world[entity].contains(world::id_of<T>()));
	ASSERT_EQ(world.operator[]<T>(entity), component);
}

template <typename T>
void check_erase(const world &world, std::uint32_t entity)
{
	ASSERT_TRUE(world.contains(entity));
	ASSERT_FALSE(world.contains<T>(entity));
	ASSERT_TRUE(world.at(entity));
	ASSERT_FALSE(world.at(entity)->contains(world::id_of<T>()));
	ASSERT_FALSE(world.at<T>(entity));
}

template <typename T>
void check_despawn(const world &world, std::uint32_t entity)
{
	ASSERT_FALSE(world.contains(entity));
	ASSERT_FALSE(world.contains<T>(entity));
	ASSERT_FALSE(world.at(entity));
	ASSERT_FALSE(world.at<T>(entity));
}
}

TEST(stellarlib_ecs_world, should_spawn_entities)
{
	world world{};
	const auto entity1{world.spawn(std::int32_t{1}, std::int64_t{2})};
	check_insert(world, entity1, std::int32_t{1});
	check_insert(world, entity1, std::int64_t{2});
	const auto entity2{world.spawn(std::int32_t{3}, std::int64_t{4})};
	check_insert(world, entity2, std::int32_t{3});
	check_insert(world, entity2, std::int64_t{4});
}

TEST(stellarlib_ecs_world, should_insert_components)
{
	world world{};
	const auto entity{world.spawn(std::int8_t{1}, std::int16_t{2})};
	world.insert(entity, std::int32_t{3}, std::int64_t{4});
	check_insert(world, entity, std::int8_t{1});
	check_insert(world, entity, std::int16_t{2});
	check_insert(world, entity, std::int32_t{3});
	check_insert(world, entity, std::int64_t{4});
}

TEST(stellarlib_ecs_world, should_erase_components)
{
	world world{};
	const auto entity{world.spawn(std::int32_t{1}, std::int64_t{2})};
	world.erase<std::int32_t, std::int64_t>(entity);
	check_erase<std::int32_t>(world, entity);
	check_erase<std::int64_t>(world, entity);
}

TEST(stellarlib_ecs_world, should_despawn_entities)
{
	world world{};
	const auto entity1{world.spawn(std::int32_t{1}, std::int64_t{2})};
	const auto entity2{world.spawn(std::int32_t{3}, std::int64_t{4})};
	world.despawn(entity1);
	check_despawn<std::int32_t>(world, entity1);
	check_despawn<std::int64_t>(world, entity1);
	check_insert(world, entity2, std::int32_t{3});
	check_insert(world, entity2, std::int64_t{4});
	world.despawn(entity2);
	check_despawn<std::int32_t>(world, entity2);
	check_despawn<std::int64_t>(world, entity2);
}

TEST(stellarlib_ecs_world, should_evaluate_solo_query)
{
	world world{};
	const auto entity1{world.spawn(std::int32_t{1}, std::int64_t{2})};
	const auto entity2{world.spawn(std::int32_t{3}, std::int64_t{4})};
	ASSERT_FALSE(world.query<std::int8_t>().size());
	ASSERT_FALSE(world.query<std::int16_t>().size());
	ASSERT_EQ(world.query<std::int32_t>().size(), 2);
	ASSERT_TRUE(std::ranges::all_of(world.query<std::int32_t>(), [=](const auto &set) -> bool
	{
		return std::get<0>(set) == entity1 && std::get<1>(set) == 1
			|| std::get<0>(set) == entity2 && std::get<1>(set) == 3;
	}));
	ASSERT_EQ(world.query<std::int64_t>().size(), 2);
	ASSERT_TRUE(std::ranges::all_of(world.query<std::int64_t>(), [=](const auto &set) -> bool
	{
		return std::get<0>(set) == entity1 && std::get<1>(set) == 2
			|| std::get<0>(set) == entity2 && std::get<1>(set) == 4;
	}));
}

/* NOLINTEND(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
