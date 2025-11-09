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

#include <cstdint>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

TEST(ecs_world, should_spawn_entities)
{
	world world{};
	const auto entity1{world.spawn(std::int32_t{1}, std::int64_t{2})};
	ASSERT_FALSE(world.get<std::int8_t>(entity1));
	ASSERT_FALSE(world.get<std::int16_t>(entity1));
	ASSERT_TRUE(world.get<std::int32_t>(entity1));
	ASSERT_EQ(*world.get<std::int32_t>(entity1), 1);
	ASSERT_TRUE(world.get<std::int64_t>(entity1));
	ASSERT_EQ(*world.get<std::int64_t>(entity1), 2);
	const auto entity2{world.spawn(std::int32_t{3}, std::int64_t{4})};
	ASSERT_FALSE(world.get<std::int8_t>(entity2));
	ASSERT_FALSE(world.get<std::int16_t>(entity2));
	ASSERT_TRUE(world.get<std::int32_t>(entity2));
	ASSERT_EQ(*world.get<std::int32_t>(entity2), 3);
	ASSERT_TRUE(world.get<std::int64_t>(entity2));
	ASSERT_EQ(*world.get<std::int64_t>(entity2), 4);
}

TEST(ecs_world, should_insert_components)
{
	world world{};
	const auto entity{world.spawn(std::int8_t{1}, std::int16_t{2})};
	world.insert(entity, std::int32_t{3}, std::int64_t{4});
	ASSERT_TRUE(world.get<std::int8_t>(entity));
	ASSERT_EQ(*world.get<std::int8_t>(entity), 1);
	ASSERT_TRUE(world.get<std::int16_t>(entity));
	ASSERT_EQ(*world.get<std::int16_t>(entity), 2);
	ASSERT_TRUE(world.get<std::int32_t>(entity));
	ASSERT_EQ(*world.get<std::int32_t>(entity), 3);
	ASSERT_TRUE(world.get<std::int64_t>(entity));
	ASSERT_EQ(*world.get<std::int64_t>(entity), 4);
}

TEST(ecs_world, should_erase_components)
{
	world world{};
	const auto entity{world.spawn(std::int32_t{1}, std::int64_t{2})};
	world.erase<std::int32_t, std::int64_t>(entity);
	ASSERT_FALSE(world.get<std::int32_t>(entity));
	ASSERT_FALSE(world.get<std::int64_t>(entity));
}

TEST(ecs_world, should_despawn_entities)
{
	world world{};
	const auto entity1{world.spawn(std::int32_t{1}, std::int64_t{2})};
	const auto entity2{world.spawn(std::int32_t{3}, std::int64_t{4})};
	world.despawn(entity1);
	ASSERT_FALSE(world.get<std::int32_t>(entity1));
	ASSERT_FALSE(world.get<std::int64_t>(entity1));
	ASSERT_TRUE(world.get<std::int32_t>(entity2));
	ASSERT_EQ(*world.get<std::int32_t>(entity2), 3);
	ASSERT_TRUE(world.get<std::int64_t>(entity2));
	ASSERT_EQ(*world.get<std::int64_t>(entity2), 4);
	world.despawn(entity2);
	ASSERT_FALSE(world.get<std::int32_t>(entity2));
	ASSERT_FALSE(world.get<std::int64_t>(entity2));
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
