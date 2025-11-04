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

#include <stellarlib/ecs/stack_vector.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

static const std::vector<std::shared_ptr<std::int32_t>> VALUES{
	std::make_shared<std::int32_t>(0),
	std::make_shared<std::int32_t>(5),
	std::make_shared<std::int32_t>(10),
	std::make_shared<std::int32_t>(15),
	std::make_shared<std::int32_t>(25)
};

namespace
{
void check_range_mut(stack_vector<std::shared_ptr<std::int32_t>> &vector)
{
	ASSERT_EQ(vector.size(), VALUES.size());
	for (std::uint32_t i{}; i != VALUES.size(); ++i) {
		ASSERT_EQ(vector[i], VALUES[i]);
	}
	ASSERT_EQ(vector.end() - vector.begin(), vector.size());
	ASSERT_TRUE(std::ranges::equal(vector, VALUES));
}

void check_range_const(const stack_vector<std::shared_ptr<std::int32_t>> &vector)
{
	ASSERT_EQ(vector.size(), VALUES.size());
	for (std::uint32_t i{}; i != VALUES.size(); ++i) {
		ASSERT_EQ(vector[i], VALUES[i]);
	}
	ASSERT_EQ(vector.end() - vector.begin(), vector.size());
	ASSERT_TRUE(std::ranges::equal(vector, VALUES));
}
}

TEST(ecs_stack_vector, should_init_via_ctor)
{
	const stack_vector<std::shared_ptr<std::int32_t>> vector{};
	ASSERT_FALSE(vector.size());
	ASSERT_FALSE(vector.begin());
	ASSERT_FALSE(vector.end());
}

TEST(ecs_stack_vector, should_skip_empty_copy_via_ctor)
{
	const stack_vector<std::shared_ptr<std::int32_t>> vector1{};
	const auto vector2{vector1};
	ASSERT_FALSE(vector2.size());
	ASSERT_FALSE(vector2.begin());
	ASSERT_FALSE(vector2.end());
}

TEST(ecs_stack_vector, should_copy_via_ctor)
{
	stack_vector<std::shared_ptr<std::int32_t>> vector1{};
	for (const auto &value : VALUES) {
		vector1.push(value);
	}
	auto vector2{vector1};
	ASSERT_NE(vector2.begin(), vector1.begin());
	check_range_mut(vector2);
	check_range_const(vector2);
}

TEST(ecs_stack_vector, should_move_via_ctor)
{
	stack_vector<std::shared_ptr<std::int32_t>> vector1{};
	for (const auto &value : VALUES) {
		vector1.push(value);
	}
	const auto begin{vector1.begin()};
	auto vector2{std::move(vector1)};
	ASSERT_EQ(vector2.begin(), begin);
	check_range_mut(vector2);
	check_range_const(vector2);
}

TEST(ecs_stack_vector, should_skip_self_copy_via_assignment)
{
	stack_vector<std::shared_ptr<std::int32_t>> vector{};
	for (const auto &value : VALUES) {
		vector.push(value);
	}
	const auto begin{vector.begin()};
	vector = vector;
	ASSERT_EQ(vector.begin(), begin);
	check_range_mut(vector);
	check_range_const(vector);
}

TEST(ecs_stack_vector, should_optimize_copy_via_assignment)
{
	stack_vector<std::shared_ptr<std::int32_t>> vector1{};
	for (const auto &value : VALUES) {
		vector1.push(value);
	}
	stack_vector<std::shared_ptr<std::int32_t>> vector2{};
	for (const auto &value : std::ranges::reverse_view(VALUES)) {
		vector2.push(value);
	}
	const auto begin{vector2.begin()};
	vector2 = vector1;
	ASSERT_EQ(vector2.begin(), begin);
	check_range_mut(vector2);
	check_range_const(vector2);
}

TEST(ecs_stack_vector, should_copy_via_assignment)
{
	stack_vector<std::shared_ptr<std::int32_t>> vector1{};
	for (const auto &value : VALUES) {
		vector1.push(value);
	}
	stack_vector<std::shared_ptr<std::int32_t>> vector2{};
	vector2.push(VALUES.front());
	vector2 = vector1;
	ASSERT_NE(vector2.begin(), vector1.begin());
	check_range_mut(vector2);
	check_range_const(vector2);
}

TEST(ecs_stack_vector, should_skip_self_move_via_assignment)
{
	stack_vector<std::shared_ptr<std::int32_t>> vector{};
	for (const auto &value : VALUES) {
		vector.push(value);
	}
	const auto begin{vector.begin()};
	vector = std::move(vector);
	ASSERT_EQ(vector.begin(), begin);
	check_range_mut(vector);
	check_range_const(vector);
}

TEST(ecs_stack_vector, should_move_via_assignment)
{
	stack_vector<std::shared_ptr<std::int32_t>> vector1{};
	for (const auto &value : VALUES) {
		vector1.push(value);
	}
	stack_vector<std::shared_ptr<std::int32_t>> vector2{};
	vector2.push(VALUES.front());
	const auto begin{vector1.begin()};
	vector2 = std::move(vector1);
	ASSERT_EQ(vector2.begin(), begin);
	check_range_mut(vector2);
	check_range_const(vector2);
}

TEST(ecs_stack_vector, should_optimize_extend)
{
	stack_vector<std::shared_ptr<std::int32_t>> vector{};
	for (const auto &value : VALUES) {
		vector.push(value);
	}
	const auto begin{vector.begin()};
	ASSERT_FALSE(vector.extend(vector.size()));
	ASSERT_EQ(vector.begin(), begin);
	check_range_mut(vector);
	check_range_const(vector);
}

TEST(ecs_stack_vector, should_extend)
{
	stack_vector<std::shared_ptr<std::int32_t>> vector{};
	vector.push(VALUES.front());
	ASSERT_TRUE(vector.extend(static_cast<std::uint32_t>(VALUES.size())));
	ASSERT_EQ(vector.size(), VALUES.size());
	ASSERT_EQ(vector[0], VALUES.front());
	for (std::uint32_t i{1}; i != VALUES.size(); ++i) {
		ASSERT_EQ(vector[i], std::shared_ptr<std::int32_t>{});
	}
	ASSERT_EQ(vector.end() - vector.begin(), vector.size());
}

TEST(ecs_stack_vector, should_push_and_pop_values)
{
	stack_vector<std::shared_ptr<std::int32_t>> vector{};
	for (std::uint32_t i{}; i != VALUES.size(); ++i) {
		vector.push(VALUES[i]);
		ASSERT_EQ(vector.size(), i + 1);
		ASSERT_EQ(vector[i], VALUES[i]);
		ASSERT_EQ(vector.end() - vector.begin(), i + 1);
		ASSERT_EQ(*std::ranges::find(vector, VALUES[i]), VALUES[i]);
		const auto begin{vector.begin()};
		vector.pop();
		ASSERT_EQ(vector.size(), i);
		ASSERT_EQ(vector.begin(), begin);
		ASSERT_EQ(vector.end() - vector.begin(), i);
		ASSERT_EQ(std::ranges::find(vector, VALUES[i]), vector.end());
		vector.push(VALUES[i]);
		ASSERT_EQ(vector.size(), i + 1);
		ASSERT_EQ(vector[i], VALUES[i]);
		ASSERT_EQ(vector.begin(), begin);
		ASSERT_EQ(vector.end() - vector.begin(), i + 1);
		ASSERT_EQ(*std::ranges::find(vector, VALUES[i]), VALUES[i]);
	}
	check_range_mut(vector);
	check_range_const(vector);
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
