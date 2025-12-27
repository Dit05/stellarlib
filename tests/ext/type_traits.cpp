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

#include <stellarlib/ext/type_traits.hpp>

using namespace stellarlib::ext;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#pragma clang diagnostic ignored "-Wself-move"

/* NOLINTBEGIN(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

class trivially_relocatable final
{
public:
	[[nodiscard]]
	explicit trivially_relocatable();

	[[nodiscard]]
	trivially_relocatable(const trivially_relocatable &);

	[[nodiscard]]
	constexpr trivially_relocatable(trivially_relocatable &&) = default;

	auto operator=(const trivially_relocatable &)
		-> trivially_relocatable &;

	auto operator=(trivially_relocatable &&)
		-> trivially_relocatable &;

	constexpr ~trivially_relocatable() = default;
};

class non_standard_layout final
{
public:
	[[nodiscard]]
	explicit non_standard_layout();

	[[nodiscard]]
	non_standard_layout(const non_standard_layout &);

	[[nodiscard]]
	constexpr non_standard_layout(non_standard_layout &&) = default;

	auto operator=(const non_standard_layout &)
		-> non_standard_layout &;

	auto operator=(non_standard_layout &&)
		-> non_standard_layout &;

	constexpr ~non_standard_layout() = default;

private:
	bool _field1{};

public:
	bool _field2{};
};

class non_trivially_move_constructible final
{
public:
	[[nodiscard]]
	explicit non_trivially_move_constructible();

	[[nodiscard]]
	non_trivially_move_constructible(const non_trivially_move_constructible &);

	[[nodiscard]]
	non_trivially_move_constructible(non_trivially_move_constructible &&);

	auto operator=(const non_trivially_move_constructible &)
		-> non_trivially_move_constructible &;

	auto operator=(non_trivially_move_constructible &&)
		-> non_trivially_move_constructible &;

	constexpr ~non_trivially_move_constructible() = default;
};

class non_trivially_destructible final
{
public:
	[[nodiscard]]
	explicit non_trivially_destructible();

	[[nodiscard]]
	non_trivially_destructible(const non_trivially_destructible &);

	[[nodiscard]]
	constexpr non_trivially_destructible(non_trivially_destructible &&) = default;

	auto operator=(const non_trivially_destructible &)
		-> non_trivially_destructible &;

	auto operator=(non_trivially_destructible &&)
		-> non_trivially_destructible &;

	~non_trivially_destructible();
};

static_assert(is_trivially_relocatable_v<trivially_relocatable>);
static_assert(!is_trivially_relocatable_v<non_standard_layout>);
static_assert(!is_trivially_relocatable_v<non_trivially_move_constructible>);
static_assert(!is_trivially_relocatable_v<non_trivially_destructible>);

/* NOLINTEND(cert-err58-cpp,cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes,performance-unnecessary-copy-initialization) */

#pragma clang diagnostic pop
