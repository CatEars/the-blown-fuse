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
    std::string name;
    file_ops file_operation;
    std::vector<faked_file> children;
    bool is_root = false;
};

struct file_tree
{
    faked_file root;
    std::string mirror;
};
