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
#define BOOST_TEST_MODULE Plan file operations tests
#include <boost/test/included/unit_test.hpp>
#include "plan_file_operations.hpp"

BOOST_AUTO_TEST_CASE(calculate_fuse_operation_always_returns_passthrough)
{
    auto ops = plan_file_operations();
    BOOST_TEST(!ops.has_error());
    auto requested = *ops;
    BOOST_TEST(requested == file_ops::passthrough);
}
