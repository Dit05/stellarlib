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

#include <stellarlib/ext/memory.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <ranges>
#include <string>
#include <type_traits>

using namespace stellarlib;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

static_assert(std::is_same_v<ext::vector_allocator<std::int32_t>::value_type, std::allocator<std::int32_t>::value_type>);
static_assert(std::is_same_v<ext::vector_allocator<std::int32_t>::size_type, std::allocator<std::int32_t>::size_type>);
static_assert(std::is_same_v<ext::vector_allocator<std::int32_t>::difference_type, std::allocator<std::int32_t>::difference_type>);
static_assert(std::is_same_v<ext::vector_allocator<std::int32_t>::propagate_on_container_move_assignment, std::allocator<std::int32_t>::propagate_on_container_move_assignment>);

TEST(stellarlib_ext_memory, vector_allocator_should_acquire_and_release_trivial_arena)
{
	std::unique_ptr<std::int32_t, void (*)(std::int32_t *)> arena{nullptr, ext::vector_allocator<std::int32_t>::deallocate};
	const auto size{100};
	ext::vector_allocator<std::int32_t>::allocate(arena, size);
	ASSERT_TRUE(arena);
	std::uninitialized_fill_n(arena.get(), size, -1);
	std::destroy_n(arena.get(), size);
}

TEST(stellarlib_ext_memory, vector_allocator_should_acquire_and_release_non_trivial_arena)
{
	std::unique_ptr<std::string, void (*)(std::string *)> arena{nullptr, ext::vector_allocator<std::string>::deallocate};
	const auto size{100};
	ext::vector_allocator<std::string>::allocate(arena, size);
	ASSERT_TRUE(arena);
	std::uninitialized_fill_n(arena.get(), size, std::to_string(-1));
	std::destroy_n(arena.get(), size);
}

TEST(stellarlib_ext_memory, vector_allocator_should_handle_unsafe_arena)
{
	std::unique_ptr<std::int32_t, void (*)(std::int32_t *)> arena{nullptr, ext::vector_allocator<std::int32_t>::deallocate};
	const std::size_t size1{100};
	ext::vector_allocator<std::int32_t>::allocate(arena, size1);
	for (const auto i : std::views::iota(std::size_t{}, size1)) {
		std::construct_at(arena.get() + i, i);
	}
	const std::size_t size2{125};
	ext::vector_allocator<std::int32_t>::reallocate(arena, size2);
	ASSERT_TRUE(arena);
	std::uninitialized_fill(arena.get() + size1, arena.get() + size2, -1);
	for (const auto i : std::views::iota(std::size_t{}, size1)) {
		ASSERT_EQ(arena.get()[i], i);
	}
	const std::size_t size3{62};
	ext::vector_allocator<std::int32_t>::reallocate(arena, size3);
	ASSERT_TRUE(arena);
	for (const auto i : std::views::iota(std::size_t{}, size3)) {
		ASSERT_EQ(arena.get()[i], i);
	}
}

TEST(stellarlib_ext_memory, vector_allocator_should_grow_trivial_arena)
{
	std::unique_ptr<std::int32_t, void (*)(std::int32_t *)> arena{nullptr, ext::vector_allocator<std::int32_t>::deallocate};
	std::size_t capacity{1};
	ext::vector_allocator<std::int32_t>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 1);
	++capacity;
	ext::vector_allocator<std::int32_t>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 2);
	++capacity;
	ext::vector_allocator<std::int32_t>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 3);
	++capacity;
	ext::vector_allocator<std::int32_t>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 5);
	++capacity;
	ext::vector_allocator<std::int32_t>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 7);
	++capacity;
	ext::vector_allocator<std::int32_t>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 10);
}

TEST(stellarlib_ext_memory, vector_allocator_should_resize_trivial_arena)
{
	std::unique_ptr<std::int32_t, void (*)(std::int32_t *)> arena{nullptr, ext::vector_allocator<std::int32_t>::deallocate};
	std::size_t size{100};
	ext::vector_allocator<std::int32_t>::allocate(arena, size);
	for (const auto i : std::views::iota(std::size_t{}, size)) {
		std::construct_at(arena.get() + i, i);
	}
	auto capacity{size};
	ext::vector_allocator<std::int32_t>::reallocate(arena, size, capacity);
	ASSERT_TRUE(arena);
	ASSERT_EQ(capacity, 125);
	std::uninitialized_fill(arena.get() + size, arena.get() + capacity, -1);
	for (const auto i : std::views::iota(std::size_t{}, size)) {
		ASSERT_EQ(arena.get()[i], i);
	}
	size = 50;
	std::destroy(arena.get() + size, arena.get() + capacity);
	capacity = size;
	ext::vector_allocator<std::int32_t>::reallocate(arena, size, capacity);
	ASSERT_TRUE(arena);
	ASSERT_EQ(capacity, 62);
	for (const auto i : std::views::iota(std::size_t{}, size)) {
		ASSERT_EQ(arena.get()[i], i);
	}
	std::destroy_n(arena.get(), size);
}

TEST(stellarlib_ext_memory, vector_allocator_should_grow_non_trivial_arena)
{
	std::unique_ptr<std::string, void (*)(std::string *)> arena{nullptr, ext::vector_allocator<std::string>::deallocate};
	std::size_t capacity{1};
	ext::vector_allocator<std::string>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 1);
	++capacity;
	ext::vector_allocator<std::string>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 2);
	++capacity;
	ext::vector_allocator<std::string>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 4);
	++capacity;
	ext::vector_allocator<std::string>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 8);
	++capacity;
	ext::vector_allocator<std::string>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 16);
	++capacity;
	ext::vector_allocator<std::string>::reallocate(arena, 0, capacity);
	ASSERT_EQ(capacity, 32);
}

TEST(stellarlib_ext_memory, vector_allocator_should_resize_non_trivial_arena)
{
	std::unique_ptr<std::string, void (*)(std::string *)> arena{nullptr, ext::vector_allocator<std::string>::deallocate};
	std::size_t size{100};
	ext::vector_allocator<std::string>::allocate(arena, size);
	for (const auto i : std::views::iota(std::size_t{}, size)) {
		std::construct_at(arena.get() + i, std::to_string(i));
	}
	auto capacity{size};
	ext::vector_allocator<std::string>::reallocate(arena, size, capacity);
	ASSERT_TRUE(arena);
	ASSERT_EQ(capacity, 128);
	std::uninitialized_fill(arena.get() + size, arena.get() + capacity, std::to_string(-1));
	for (const auto i : std::views::iota(std::size_t{}, size)) {
		ASSERT_EQ(arena.get()[i], std::to_string(i));
	}
	size = 50;
	std::destroy(arena.get() + size, arena.get() + capacity);
	capacity = size;
	ext::vector_allocator<std::string>::reallocate(arena, size, capacity);
	ASSERT_TRUE(arena);
	ASSERT_EQ(capacity, 64);
	for (const auto i : std::views::iota(std::size_t{}, size)) {
		ASSERT_EQ(arena.get()[i], std::to_string(i));
	}
	std::destroy_n(arena.get(), size);
}

static_assert(ext::vector_allocator<std::int32_t>{} == ext::vector_allocator<std::int32_t>{});
static_assert(!(ext::vector_allocator<std::int32_t>{} != ext::vector_allocator<std::int32_t>{}));

/* NOLINTEND(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
