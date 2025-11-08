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

using namespace stellarlib::ecs;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,performance-unnecessary-copy-initialization) */

TEST(ecs_sparse_storage, should_assign_ids)
{
	ASSERT_EQ(sparse_storage::id_of<std::int8_t>(), 0);
	ASSERT_EQ(sparse_storage::id_of<std::int16_t>(), 1);
	ASSERT_EQ(sparse_storage::id_of<std::int32_t>(), 2);
	ASSERT_EQ(sparse_storage::id_of<std::int64_t>(), 3);
}

/* NOLINTEND(cert-err58-cpp,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
