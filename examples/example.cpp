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
#include <stellarlib/stellarlib.hpp>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

auto main([[maybe_unused]] const std::int32_t argc, [[maybe_unused]] const char **argv)
	-> std::int32_t
{
	stellarlib::hello_world();
	stellarlib::ecs::world world{};
	const auto player1{world.spawn(std::string{"Player 0"}, std::uint16_t{0})};
	std::puts("--------------------------------------------------------------------------------");

	for (const auto [entity, name, score] : world.query<std::string, std::uint16_t>()) {
		std::cout << entity << ": { name: " << name << ", score: " << score << " }\n";
	}

	const auto player2{world.spawn(std::uint16_t{5}, std::string{"Player 1"})};
	std::puts("--------------------------------------------------------------------------------");

	for (const auto [entity, score, name] : world.query<std::uint16_t, std::string>()) {
		std::cout << entity << ": { name: " << name << ", score: " << score << " }\n";
	}

	const auto player3{world.spawn(std::string{"Player 2"}, std::uint16_t{10})};
	std::puts("--------------------------------------------------------------------------------");

	for (const auto [entity, name, score] : world.query<std::string, std::uint16_t>()) {
		std::cout << entity << ": { name: " << name << ", score: " << score << " }\n";
	}

	world.despawn(player3);
	world.despawn(player2);
	world.despawn(player1);
	return EXIT_SUCCESS;
}
