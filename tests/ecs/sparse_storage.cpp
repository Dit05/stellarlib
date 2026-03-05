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

#include <stellarlib/ecs/sparse_storage.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <memory>

using namespace stellarlib;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

class foo final {};

class bar final {};

TEST(stellarlib_ecs_sparse_storage, should_assign_and_cache_ids)
{
	const auto id1{ecs::internal::sparse_storage::ids<foo>().front()};
	ASSERT_EQ(ecs::internal::sparse_storage::ids<foo>().front(), id1);
	const auto id2{ecs::internal::sparse_storage::ids<bar>().front()};
	ASSERT_EQ(ecs::internal::sparse_storage::ids<bar>().front(), id2);
	ASSERT_EQ(id1 + 1, id2);
	ASSERT_EQ(std::addressof(ecs::internal::sparse_storage::ids<foo, bar>().front()), std::addressof(ecs::internal::sparse_storage::ids<foo, bar>().front()));
	ASSERT_TRUE(std::ranges::equal(ecs::internal::sparse_storage::ids<foo, bar>(), std::initializer_list<std::uint16_t>{id1, id2}));
	ASSERT_EQ(std::addressof(ecs::internal::sparse_storage::ids<bar, foo>().front()), std::addressof(ecs::internal::sparse_storage::ids<bar, foo>().front()));
	ASSERT_TRUE(std::ranges::equal(ecs::internal::sparse_storage::ids<bar, foo>(), std::initializer_list<std::uint16_t>{id2, id1}));
}

TEST(stellarlib_ecs_sparse_storage, should_copy_via_ctor)
{
	ecs::internal::sparse_storage storage1{};
	storage1.at<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front()).insert(0, 5);
	storage1.at<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front()).insert(10, 15);
	const auto storage2{storage1};
	ASSERT_EQ(storage2.operator[]<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front())[0], 5);
	ASSERT_EQ(storage2.operator[]<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front())[10], 15);
}

TEST(stellarlib_ecs_sparse_storage, should_skip_self_copy_via_assignment)
{
	ecs::internal::sparse_storage storage{};
	storage.at<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front()).insert(0, 5);
	storage.at<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front()).insert(10, 15);
	storage = storage;
	ASSERT_EQ(storage.operator[]<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front())[0], 5);
	ASSERT_EQ(storage.operator[]<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front())[10], 15);
}

TEST(stellarlib_ecs_sparse_storage, should_copy_via_assignment)
{
	ecs::internal::sparse_storage storage1{};
	storage1.at<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front()).insert(0, 5);
	storage1.at<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front()).insert(10, 15);
	ecs::internal::sparse_storage storage2{};
	storage2 = storage1;
	ASSERT_EQ(storage2.operator[]<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front())[0], 5);
	ASSERT_EQ(storage2.operator[]<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front())[10], 15);
}

TEST(stellarlib_ecs_sparse_storage, should_insert_and_erase_pairs)
{
	ecs::internal::sparse_storage storage{};
	storage.at<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front()).insert(0, 5);
	storage.at<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front()).insert(10, 15);
	storage.at<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front()).insert(0, 5);
	storage.at<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front()).insert(10, 15);
	storage.erase(0);
	ASSERT_FALSE(storage.operator[]<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front()).contains(0));
	ASSERT_EQ(storage.operator[]<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front())[10], 15);
	ASSERT_FALSE(storage.operator[]<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front()).contains(0));
	ASSERT_EQ(storage.operator[]<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front())[10], 15);
}

TEST(stellarlib_ecs_sparse_storage, should_clear_pairs)
{
	ecs::internal::sparse_storage storage{};
	storage.at<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front()).insert(0, 5);
	storage.at<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front()).insert(10, 15);
	storage.clear();
	ASSERT_FALSE(storage.operator[]<std::int32_t>(ecs::internal::sparse_storage::ids<std::int32_t>().front()).size());
	ASSERT_FALSE(storage.operator[]<std::int64_t>(ecs::internal::sparse_storage::ids<std::int64_t>().front()).size());
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
