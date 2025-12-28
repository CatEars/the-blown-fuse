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
#define BOOST_TEST_MODULE File tree tests
#include <boost/test/included/unit_test.hpp>
#include <boost/leaf.hpp>
#include "global_file_tree.hpp"

namespace leaf = boost::leaf;

BOOST_AUTO_TEST_CASE(file_tree_has_a_file_for_each_execution_strategy)
{
    BOOST_TEST(!global_file_tree.get("/log").has_error());
    BOOST_TEST(!global_file_tree.get("/slow").has_error());
    BOOST_TEST(!global_file_tree.get("/fail").has_error());
    BOOST_TEST(!global_file_tree.get("/passthrough").has_error());
}

BOOST_AUTO_TEST_CASE(file_tree_has_a_root_file_with_four_children)
{
    auto result = global_file_tree.get("/");
    BOOST_TEST(!result.has_error());
    BOOST_TEST((*result).children.size() == 4);
}

BOOST_AUTO_TEST_CASE(file_tree_returns_no_such_file_when_file_does_not_exist)
{
    auto main = []() -> leaf::result<faked_file>
    {
        auto result = global_file_tree.get("/this-file-does-not-exist");
        BOOST_TEST(result.has_error());
        return result;
    };

    auto handler = [](file_errors file_error) -> leaf::result<faked_file>
    {
        BOOST_TEST(file_error == file_errors::no_such_file);
        return leaf::new_error(file_error);
    };

    auto res = leaf::try_handle_some(main, handler);
    BOOST_TEST(res.has_error());
}
