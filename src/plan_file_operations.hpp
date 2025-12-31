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
#include "plannable_operations.hpp"
#include "file_tree.hpp"

namespace leaf = boost::leaf;

std::ostream &operator<<(std::ostream &os, const file_ops &op)
{
    switch (op)
    {
    case file_ops::passthrough:
        return os << "passthrough";
    case file_ops::slow:
        return os << "slow";
    case file_ops::fail:
        return os << "slow";
    case file_ops::log:
        return os << "log";
    default:
        return os << "unknown";
    }
}

leaf::result<file_ops> plan_file_operations(
    const faked_file_tree& tree, 
    const std::string &path)
{
    auto res = tree.get(path);
    if (res.has_error()) {
        return leaf::new_error();
    } else {
        return res.value().file_operation;
    }
}
