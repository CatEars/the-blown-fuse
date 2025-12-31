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
#pragma once
#include <boost/leaf.hpp>
#include <string>
#include "plannable_operations.hpp"

namespace leaf = boost::leaf;

enum class file_errors
{
    no_such_file
};

std::ostream &operator<<(std::ostream &os, const file_errors &err)
{
    switch (err)
    {
    case file_errors::no_such_file:
        os << "no_such_file";
        break;
    }
    return os;
}

struct faked_file
{
    std::string path;
    std::string name;
    file_ops file_operation;
    std::vector<faked_file> children;
};
static faked_file _log_file{
    .path = "/",
    .name = "log",
    .file_operation = file_ops::log};
static faked_file _slow_file{
    .path = "/",
    .name = "slow",
    .file_operation = file_ops::slow};
static faked_file _fail_file{
    .path = "/",
    .name = "fail",
    .file_operation = file_ops::fail};
static faked_file _passthrough_file{
    .path = "/",
    .name = "passthrough",
    .file_operation = file_ops::passthrough};
static faked_file _root_file{
    .path = "/",
    .name = "",
    .children = std::vector<faked_file>{_log_file, _slow_file, _fail_file, _passthrough_file},
};

class faked_file_tree
{
public:
    leaf::result<faked_file &> get(const std::string &path) const
    {
        if (path == "/log")
        {
            return _log_file;
        }
        else if (path == "/slow")
        {
            return _slow_file;
        }
        else if (path == "/fail")
        {
            return _fail_file;
        }
        else if (path == "/passthrough")
        {
            return _passthrough_file;
        }
        else if (path == "/")
        {
            return _root_file;
        }

        return leaf::new_error(file_errors::no_such_file);
    }
};
