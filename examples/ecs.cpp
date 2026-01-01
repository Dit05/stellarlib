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

#include <stellarlib/ecs/world.hpp>

#include <cstddef>
#include <cstdint>

static constexpr auto ENTITIES = 50'000'000 * 2;

struct position
{
	float x;
	float y;
};

struct velocity
{
	float x;
	float y;
};

struct health
{
	float value;
};

struct shield
{
	float value;
};

auto main([[maybe_unused]] const std::int32_t argc, [[maybe_unused]] const char *const*argv)
	-> std::int32_t
{
	stellarlib::ecs::world world{};

	for (std::size_t i{}; i != ENTITIES; ++i) {
		world.spawn(position{}, velocity{}, health{}, shield{});
	}

	return 0;
}

/* NOLINTEND(cert-msc51-cpp,cert-msc32-c) */
