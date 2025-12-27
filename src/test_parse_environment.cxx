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
#define BOOST_TEST_MODULE Parse Environment
#include <boost/test/included/unit_test.hpp>

#include <boost/process.hpp>

#include "parse_environment.hpp"

BOOST_AUTO_TEST_CASE(without_bf_fuse_config_directory_set_returns_error)
{
    auto env = boost::process::environment();
    env.set(config_directory_env_key, "");
    auto result = parse_environment_variables(env);
    BOOST_TEST(result.has_error());
}

BOOST_AUTO_TEST_CASE(with_bf_fuse_config_directory_set_returns_value_of_that)
{
    auto env = boost::process::environment();
    env.set(config_directory_env_key, "/tmp");
    auto result = parse_environment_variables(env);
    BOOST_TEST(!result.has_error());
    auto environment = result.value();
    BOOST_TEST(environment.config_directory == "/tmp");
}
