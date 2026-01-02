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
#include <iostream>
#include <string>
#include "options.hpp"

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

BOOST_AUTO_TEST_CASE(accepts_help_and_version_with_same_format_as_fuse)
{
    std::vector<std::string> options{
        "-h", "--help",
        "-V", "--version"};
    auto description = get_description();
    for (const auto &expected : options)
    {
        BOOST_TEST(description.find(expected) != std::string::npos);
    }
}

BOOST_AUTO_TEST_CASE(can_filter_out_fuse_specific_flags_from_bf_fuse_flags)
{
    std::vector<std::string> argv_vec{
        "bf_fuse",
        "-c",
        "config_file.json",
        "/tmp/mountpoint",
        "-f",
        "-o",
        "some_option=1"};
    std::vector<char *> argv_char{};
    for (auto &arg : argv_vec)
    {
        argv_char.push_back(&arg[0]);
    }
    auto argc = argv_vec.size();
    auto argv = argv_char.data();
    auto desc = get_program_options();

    auto parsed = po::command_line_parser(argc, argv)
                      .options(desc)
                      .allow_unregistered()
                      .run();

    po::variables_map vm;
    po::store(parsed, vm);
    po::notify(vm);

    fuse_args_without_bf_fuse fargs = filter_fuse_args(argc, argv, parsed, vm);
    BOOST_TEST(fargs.argc == 5);
    BOOST_TEST(fargs.argv[0] == "bf_fuse");
    BOOST_TEST(fargs.argv[1] == "/tmp/mountpoint");
    BOOST_TEST(fargs.argv[2] == "-f");
    BOOST_TEST(fargs.argv[3] == "-o");
    BOOST_TEST(fargs.argv[4] == "some_option=1");
}

BOOST_AUTO_TEST_CASE(accepts_a_flag_for_bf_fuse_config_file)
{
    auto description = get_description();
    BOOST_TEST(description.find("--config") != std::string::npos);
}