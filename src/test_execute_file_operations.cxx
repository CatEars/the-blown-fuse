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
#define BOOST_TEST_MODULE Execute file operations
#include <boost/test/included/unit_test.hpp>
#include <sys/stat.h>
#include <chrono>
#include "execute_file_operations.hpp"

BOOST_AUTO_TEST_CASE(passthrough_getattr_will_perform_stat_like_normal)
{
    passthrough_operations pass;
    getattr_args args("/tmp");
    auto res = pass.getattr(args);
    BOOST_TEST(!res.has_error());
    auto val = res.value();
    BOOST_TEST(val.error == 0);
    BOOST_TEST(val.stbuf.st_size > 0);
    BOOST_TEST((val.stbuf.st_mode & S_IFDIR) > 0);
}

BOOST_AUTO_TEST_CASE(slow_getattr_will_perform_stat_with_a_delay)
{
    passthrough_operations pass;
    slow_operations slow;
    getattr_args args("/tmp");
    auto start = std::chrono::steady_clock::now();

    auto res = slow.getattr(args, [&](const getattr_args &inner_args) -> leaf::result<getattr_result>
                            { return pass.getattr(inner_args); });

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;

    BOOST_TEST(!res.has_error());
    auto val = res.value();
    BOOST_TEST(val.error == 0);
    BOOST_TEST(val.stbuf.st_size > 0);
    BOOST_TEST((val.stbuf.st_mode & S_IFDIR) > 0);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
    BOOST_TEST(ms > 750);
}

BOOST_AUTO_TEST_CASE(fail_getattr_will_always_return_an_error)
{
    fail_operations fails;
    getattr_args args("/tmp");
    auto res = fails.getattr(args);
    BOOST_TEST(!res.has_error());
    auto val = res.value();
    BOOST_TEST(val.error != 0);
}
