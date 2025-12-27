/**
    bf_fuse - A FUSE library for testing file-access failure scenarios
    Copyright (C) 2025  Henrik "CatEars" Adolfsson

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#define BOOST_TEST_MODULE Argument Parsing Tests
#include <boost/test/included/unit_test.hpp>
#include "arguments.hpp"
#include <iostream>
#include <string>

BOOST_AUTO_TEST_CASE(has_a_description)
{
    auto po = get_program_options();
    std::stringstream ss;
    ss << po;

    auto description = ss.str();
    auto expected_string = "BF FUSE";

    BOOST_TEST(description.find(expected_string) != std::string::npos);
}
