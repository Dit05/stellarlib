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

#include <cstdint>

using namespace stellarlib;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

TEST(stellarlib_ecs_sparse_storage, should_assign_ids)
{
	ASSERT_EQ(ecs::internal::sparse_storage::id<std::int32_t>(), 0);
	ASSERT_EQ(ecs::internal::sparse_storage::id<std::int32_t>(), 0);
	ASSERT_EQ(ecs::internal::sparse_storage::id<std::int64_t>(), 1);
	ASSERT_EQ(ecs::internal::sparse_storage::id<std::int64_t>(), 1);
}

TEST(stellarlib_ecs_sparse_storage, should_copy_via_ctor)
{
	ecs::internal::sparse_storage storage1{};
	storage1.at<std::int32_t>().insert(0, 5);
	storage1.at<std::int64_t>().insert(10, 15);
	auto storage2{storage1};
	ASSERT_EQ(storage2.operator[]<std::int32_t>()[0], 5);
	ASSERT_EQ(storage2.operator[]<std::int64_t>()[10], 15);
}

TEST(stellarlib_ecs_sparse_storage, should_skip_self_copy_via_assignment)
{
	ecs::internal::sparse_storage storage{};
	storage.at<std::int32_t>().insert(0, 5);
	storage.at<std::int64_t>().insert(10, 15);
	storage = storage;
	ASSERT_EQ(storage.operator[]<std::int32_t>()[0], 5);
	ASSERT_EQ(storage.operator[]<std::int64_t>()[10], 15);
}

TEST(stellarlib_ecs_sparse_storage, should_copy_via_assignment)
{
	ecs::internal::sparse_storage storage1{};
	storage1.at<std::int32_t>().insert(0, 5);
	storage1.at<std::int64_t>().insert(10, 15);
	ecs::internal::sparse_storage storage2{};
	storage2 = storage1;
	ASSERT_EQ(storage2.operator[]<std::int32_t>()[0], 5);
	ASSERT_EQ(storage2.operator[]<std::int64_t>()[10], 15);
}

TEST(stellarlib_ecs_sparse_storage, should)
{
	ecs::internal::sparse_storage storage{};
	storage.at<std::int32_t>().insert(0, 5);
	storage.at<std::int64_t>().insert(10, 15);
	ASSERT_EQ(storage.operator[]<std::int32_t>()[0], 5);
	ASSERT_EQ(storage.operator[]<std::int64_t>()[10], 15);
}

TEST(stellarlib_ecs_sparse_storage, should_erase_keys)
{
	ecs::internal::sparse_storage storage{};
	storage.at<std::int32_t>().insert(0, 5);
	storage.at<std::int32_t>().insert(10, 15);
	storage.at<std::int64_t>().insert(0, 5);
	storage.at<std::int64_t>().insert(10, 15);
	storage.erase(0);
	ASSERT_FALSE(storage.operator[]<std::int32_t>().contains(0));
	ASSERT_EQ(storage.operator[]<std::int32_t>()[10], 15);
	ASSERT_FALSE(storage.operator[]<std::int64_t>().contains(0));
	ASSERT_EQ(storage.operator[]<std::int64_t>()[10], 15);
}

TEST(stellarlib_ecs_sparse_storage, should_clear_maps)
{
	ecs::internal::sparse_storage storage{};
	storage.at<std::int32_t>().insert(0, 5);
	storage.at<std::int64_t>().insert(10, 15);
	storage.clear();
	ASSERT_FALSE(storage.operator[]<std::int32_t>().size());
	ASSERT_FALSE(storage.operator[]<std::int64_t>().size());
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
