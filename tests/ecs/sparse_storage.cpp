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

#include <stellarlib/ecs/sparse_storage.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <utility>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

namespace
{
void check_values(sparse_storage<void> &storage)
{
	ASSERT_TRUE(std::as_const(storage).by_type<std::int32_t>());
	ASSERT_EQ((*std::as_const(storage).by_type<std::int32_t>())[0], 1);
	ASSERT_EQ(storage.by_type<std::int32_t>()[0], 1);
	ASSERT_TRUE(std::as_const(storage).by_type<std::int64_t>());
	ASSERT_EQ((*std::as_const(storage).by_type<std::int64_t>())[0], 2);
	ASSERT_EQ(storage.by_type<std::int64_t>()[0], 2);
}
}

TEST(ecs_sparse_storage, should_init_via_ctor)
{
	const sparse_storage<void> storage{};
	ASSERT_FALSE(std::as_const(storage).by_type<std::int32_t>());
}

TEST(ecs_sparse_storage, should_copy_via_ctor)
{
	sparse_storage<void> storage1{};
	storage1.by_type<std::int32_t>().insert(0, 1);
	storage1.by_type<std::int64_t>().insert(0, 2);
	auto storage2{storage1};
	check_values(storage2);
}

TEST(ecs_sparse_storage, should_move_via_ctor)
{
	sparse_storage<void> storage1{};
	storage1.by_type<std::int32_t>().insert(0, 1);
	storage1.by_type<std::int64_t>().insert(0, 2);
	auto storage2{std::move(storage1)};
	check_values(storage2);
}

TEST(ecs_sparse_storage, should_copy_via_assignment)
{
	sparse_storage<void> storage1{};
	storage1.by_type<std::int32_t>().insert(0, 1);
	storage1.by_type<std::int64_t>().insert(0, 2);
	sparse_storage<void> storage2{};
	storage2 = storage1;
	check_values(storage2);
}

TEST(ecs_sparse_storage, should_move_via_assignment)
{
	sparse_storage<void> storage1{};
	storage1.by_type<std::int32_t>().insert(0, 1);
	storage1.by_type<std::int64_t>().insert(0, 2);
	sparse_storage<void> storage2{};
	storage2 = std::move(storage1);
	check_values(storage2);
}

TEST(ecs_sparse_storage, should_pick_set_by_type)
{
	sparse_storage<void> storage{};
	storage.by_type<std::int32_t>().insert(0, 1);
	storage.by_type<std::int64_t>().insert(0, 2);
	check_values(storage);
}

TEST(ecs_sparse_storage, should_iter_sets)
{
	sparse_storage<void> storage{};
	storage.by_type<std::int32_t>().insert(0, 1);
	storage.by_type<std::int64_t>().insert(0, 2);
	ASSERT_EQ(storage.sets().size(), 2);
	for (auto &set : storage.sets()) {
		set.erase(0);
	}
	ASSERT_FALSE(storage.by_type<std::int32_t>().contains(0));
	ASSERT_FALSE(storage.by_type<std::int64_t>().contains(0));
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
