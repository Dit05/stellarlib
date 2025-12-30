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

#include <stellarlib/ext/memory.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>

using namespace stellarlib::ext;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

static_assert(std::is_same_v<arena_allocator<std::int32_t>::value_type, std::allocator<std::int32_t>::value_type>);
static_assert(std::is_same_v<arena_allocator<std::int32_t>::size_type, std::allocator<std::int32_t>::size_type>);
static_assert(std::is_same_v<arena_allocator<std::int32_t>::difference_type, std::allocator<std::int32_t>::difference_type>);
static_assert(std::is_same_v<arena_allocator<std::int32_t>::propagate_on_container_move_assignment, std::allocator<std::int32_t>::propagate_on_container_move_assignment>);

TEST(stellarlib_ext_memory, should_acquire_and_release_trivial_arena)
{
	const arena_allocator<std::int32_t> allocator{};
	std::int32_t *arena{};
	std::size_t size{10};
	allocator.allocate(arena, size);
	ASSERT_TRUE(arena);
	ASSERT_EQ(size, 12);
	std::uninitialized_default_construct_n(arena, size);
	std::destroy_n(arena, size);
	allocator.deallocate(arena);
}

TEST(stellarlib_ext_memory, should_acquire_and_release_non_trivial_arena)
{
	const arena_allocator<std::string> allocator{};
	std::string *arena{};
	std::size_t size{10};
	allocator.allocate(arena, size);
	ASSERT_TRUE(arena);
	ASSERT_EQ(size, 20);
	std::uninitialized_default_construct_n(arena, size);
	std::destroy_n(arena, size);
	allocator.deallocate(arena);
}

TEST(stellarlib_ext_memory, should_resize_trivial_arena)
{
	const arena_allocator<std::int32_t> allocator{};
	std::int32_t *arena{};
	std::size_t size{10};
	allocator.allocate(arena, size);
	for (std::size_t i{}; i != size; ++i) {
		std::construct_at(arena + i, i);
	}
	const auto old{size};
	allocator.reallocate(arena, old, size);
	ASSERT_TRUE(arena);
	ASSERT_EQ(size, 15);
	std::uninitialized_fill(arena + old, arena + size, -1);
	for (std::size_t i{}; i != old; ++i) {
		ASSERT_EQ(arena[i], i);
	}
	std::destroy_n(arena, size);
	allocator.deallocate(arena);
}

TEST(stellarlib_ext_memory, should_resize_non_trivial_arena)
{
	const arena_allocator<std::string> allocator{};
	std::string *arena{};
	std::size_t size{10};
	allocator.allocate(arena, size);
	for (std::size_t i{}; i != size; ++i) {
		std::construct_at(arena + i, std::to_string(i));
	}
	const auto old{size};
	allocator.reallocate(arena, old, size);
	ASSERT_TRUE(arena);
	ASSERT_EQ(size, 40);
	std::uninitialized_fill(arena + old, arena + size, std::to_string(-1));
	for (std::size_t i{}; i != old; ++i) {
		ASSERT_EQ(arena[i], std::to_string(i));
	}
	std::destroy_n(arena, size);
	allocator.deallocate(arena);
}

static_assert(arena_allocator<std::int32_t>{} == arena_allocator<std::int32_t>{});
static_assert(!(arena_allocator<std::int32_t>{} != arena_allocator<std::int32_t>{}));

/* NOLINTEND(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
