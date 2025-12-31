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

#include <stellarlib/ext/functional.hpp>

#include <gtest/gtest.h>

#include <utility>

using namespace stellarlib;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

TEST(stellarlib_ext_functional, truthy)
{
	ASSERT_TRUE(ext::truthy(1));
	ASSERT_FALSE(ext::truthy(0));
	ASSERT_TRUE(ext::truthy(true));
	ASSERT_FALSE(ext::truthy(false));
	ASSERT_TRUE(ext::truthy("true"));
	ASSERT_FALSE(ext::truthy(nullptr));
}

TEST(stellarlib_ext_functional, falsy)
{
	ASSERT_TRUE(ext::falsy(0));
	ASSERT_FALSE(ext::falsy(1));
	ASSERT_TRUE(ext::falsy(false));
	ASSERT_FALSE(ext::falsy(true));
	ASSERT_TRUE(ext::falsy(nullptr));
	ASSERT_FALSE(ext::falsy("true"));
}

TEST(stellarlib_ext_functional, subset)
{
	ASSERT_TRUE(ext::subset(0xb1100, 0xb1101));
	ASSERT_TRUE(ext::subset(0xb1101, 0xb1101));
	ASSERT_FALSE(ext::subset(0xb1101, 0xb1100));
}

TEST(stellarlib_ext_functional, superset)
{
	ASSERT_TRUE(ext::superset(0xb1101, 0xb1100));
	ASSERT_TRUE(ext::superset(0xb1101, 0xb1101));
	ASSERT_FALSE(ext::superset(0xb1100, 0xb1101));
}

TEST(stellarlib_ext_functional, zip_subset)
{
	ASSERT_TRUE(ext::zip::subset(std::pair{0xb1100, 0xb1101}));
	ASSERT_TRUE(ext::zip::subset(std::pair{0xb1101, 0xb1101}));
	ASSERT_FALSE(ext::zip::subset(std::pair{0xb1101, 0xb1100}));
}

TEST(stellarlib_ext_functional, zip_superset)
{
	ASSERT_TRUE(ext::zip::superset(std::pair{0xb1101, 0xb1100}));
	ASSERT_TRUE(ext::zip::superset(std::pair{0xb1101, 0xb1101}));
	ASSERT_FALSE(ext::zip::superset(std::pair{0xb1100, 0xb1101}));
}

/* NOLINTEND(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
