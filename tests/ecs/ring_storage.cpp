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

#include <stellarlib/ecs/ring_storage.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <vector>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

TEST(stellarlib_ecs_ring_storage, should_reuse_keys)
{
	ring_storage<std::shared_ptr<std::int32_t>> storage{};
	ASSERT_EQ(storage.acquire(), 0);
	ASSERT_EQ(storage.acquire(), 1);
	ASSERT_EQ(storage.acquire(), 2);
	storage.release(2);
	storage.release(0);
	storage.release(1);
	ASSERT_EQ(storage.acquire(), 1);
	ASSERT_EQ(storage.acquire(), 0);
	ASSERT_EQ(storage.acquire(), 2);
}

TEST(stellarlib_ecs_ring_storage, should_acquire_and_release_keys)
{
	ring_storage<std::vector<std::int32_t>> storage{};
	ASSERT_FALSE(storage.contains(0));
	ASSERT_FALSE(storage.at(0));
	ASSERT_EQ(storage.acquire(), 0);
	ASSERT_TRUE(storage.contains(0));
	ASSERT_TRUE(storage.at(0));
	storage.at(0)->emplace_back();
	storage[0].emplace_back();
	ASSERT_EQ(storage.at(0)->size(), 2);
	ASSERT_EQ(storage[0].size(), 2);
	const auto data{storage.at(0)->data()};
	ASSERT_EQ(storage[0].data(), data);
	storage.release(0);
	ASSERT_FALSE(storage.contains(0));
	ASSERT_FALSE(storage.at(0));
	ASSERT_EQ(storage.acquire(), 0);
	ASSERT_TRUE(storage.contains(0));
	ASSERT_TRUE(storage.at(0));
	ASSERT_TRUE(storage.at(0)->empty());
	ASSERT_TRUE(storage[0].empty());
	ASSERT_EQ(storage.at(0)->data(), data);
	ASSERT_EQ(storage[0].data(), data);
}

/* NOLINTEND(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
