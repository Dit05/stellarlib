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

#include <stellarlib/ext/utility.hpp>

#include <gtest/gtest.h>

#include <cstdint>

using namespace stellarlib;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

class foo final {};

class bar final {};

class baz final {};

TEST(stellarlib_ext_utility, sequential_id)
{
	ASSERT_EQ((ext::sequential_id<foo, std::uint64_t>)(), 0);
	ASSERT_EQ((ext::sequential_id<foo, std::uint64_t>)(), 1);
	ASSERT_EQ((ext::sequential_id<foo, std::uint64_t>)(), 2);
	ASSERT_EQ((ext::sequential_id<foo, std::uint64_t>)(), 3);
	ASSERT_EQ((ext::sequential_id<foo, std::int64_t>)(), 0);
	ASSERT_EQ((ext::sequential_id<foo, std::int64_t>)(), 1);
	ASSERT_EQ((ext::sequential_id<foo, std::int64_t>)(), 2);
	ASSERT_EQ((ext::sequential_id<foo, std::int64_t>)(), 3);
	ASSERT_EQ((ext::sequential_id<bar, std::uint64_t>)(), 0);
	ASSERT_EQ((ext::sequential_id<bar, std::uint64_t>)(), 1);
	ASSERT_EQ((ext::sequential_id<bar, std::uint64_t>)(), 2);
	ASSERT_EQ((ext::sequential_id<bar, std::uint64_t>)(), 3);
}

TEST(stellarlib_ext_utility, scoped_typeid)
{
	ASSERT_EQ((ext::scoped_typeid<bar, std::int32_t, std::int64_t>)(), 0);
	ASSERT_EQ((ext::scoped_typeid<bar, std::int32_t, std::int64_t>)(), 0);
	ASSERT_EQ((ext::scoped_typeid<bar, std::int64_t, std::int64_t>)(), 1);
	ASSERT_EQ((ext::scoped_typeid<bar, std::int64_t, std::int64_t>)(), 1);
	ASSERT_EQ((ext::scoped_typeid<baz, std::int32_t, std::uint64_t>)(), 0);
	ASSERT_EQ((ext::scoped_typeid<baz, std::int32_t, std::uint64_t>)(), 0);
	ASSERT_EQ((ext::scoped_typeid<baz, std::int64_t, std::uint64_t>)(), 1);
	ASSERT_EQ((ext::scoped_typeid<baz, std::int64_t, std::uint64_t>)(), 1);
	ASSERT_EQ((ext::scoped_typeid<baz, std::int32_t, std::int64_t>)(), 0);
	ASSERT_EQ((ext::scoped_typeid<baz, std::int32_t, std::int64_t>)(), 0);
	ASSERT_EQ((ext::scoped_typeid<baz, std::int64_t, std::int64_t>)(), 1);
	ASSERT_EQ((ext::scoped_typeid<baz, std::int64_t, std::int64_t>)(), 1);
}

/* NOLINTEND(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
