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
#include <algorithm>
#include <boost/leaf.hpp>
#include <boost/filesystem.hpp>
#include "plannable_operations.hpp"
#include "file_tree.hpp"

namespace leaf = boost::leaf;

enum class planning_error_codes
{
    no_such_file
};

leaf::result<file_ops> plan_file_operations(
    const file_tree &file_tree_root,
    const std::string &path)
{
    boost::filesystem::path p(path);
    bool at_root = true;
    const faked_file *file_ptr = &file_tree_root.root;
    for (const auto &elem : p)
    {
        if (at_root)
        {
            at_root = false;
            continue;
        }
        auto it = std::find_if(
            file_ptr->children.begin(),
            file_ptr->children.end(),
            [&](const auto &inner_elem)
            {
                return inner_elem.name == elem;
            });

        if (it == file_ptr->children.end())
        {
            return leaf::new_error(planning_error_codes::no_such_file);
        }
        else
        {
            file_ptr = &(*it);
        }
    }
    return file_ptr->file_operation;
}
