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

std::string get_description()
{
    auto po = get_program_options();
    std::stringstream ss;
    ss << po;

    return ss.str();
}

BOOST_AUTO_TEST_CASE(has_a_description)
{
    auto description = get_description();
    auto expected_string = "bf_fuse";

    BOOST_TEST(description.find(expected_string) != std::string::npos);
}

BOOST_AUTO_TEST_CASE(has_a_help_flag)
{
    auto description = get_description();
    auto expected_string = "--help";

    BOOST_TEST(description.find(expected_string) != std::string::npos);
}

BOOST_AUTO_TEST_CASE(accepts_all_flags_that_fuse_does)
{
    std::vector<std::string> options{
        "-h", "--help",
        "-V", "--version",
        "-d",
        "-f",
        "-s",
        "-o"};
    auto description = get_description();
    for (const auto &expected : options)
    {
        BOOST_TEST(description.find(expected) != std::string::npos);
    }
}