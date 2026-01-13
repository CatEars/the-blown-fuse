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
#include <string>
#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/json.hpp>
#include <boost/leaf.hpp>
#include "plannable_operations.hpp"
#include "file_tree.hpp"

namespace leaf = boost::leaf;

struct config_stream_options
{
    std::string cwd_hint = "";
};

enum class config_parse_error
{
    parse_error,
    not_an_object,
    missing_field,
    field_wrong_type,
    field_unexpected_value
};

std::ostream &operator<<(std::ostream &os, const config_parse_error &op)
{
    switch (op)
    {
    case config_parse_error::parse_error:
        return os << "parse_error";
    case config_parse_error::not_an_object:
        return os << "not_an_object";
    case config_parse_error::missing_field:
        return os << "missing_field";
    case config_parse_error::field_wrong_type:
        return os << "field_wrong_type";
    case config_parse_error::field_unexpected_value:
        return os << "field_unexpected_value";
    default:
        return os << "unknown";
    }
}

using file_node_from_config = faked_file;

leaf::result<void> _parse_name(const boost::json::object &config_object, file_node_from_config &ret)
{
    auto name_ptr = config_object.if_contains("name");
    if (name_ptr == nullptr)
    {
        return leaf::new_error(config_parse_error::missing_field);
    }
    auto name = name_ptr->if_string();
    if (name == nullptr)
    {
        return leaf::new_error(config_parse_error::field_wrong_type);
    }
    ret.name = *name;
    return {};
}

leaf::result<void> _parse_operation(const boost::json::object &config_object, file_node_from_config &ret)
{
    auto ops_ptr = config_object.if_contains("operation");
    if (ops_ptr == nullptr)
    {
        ret.file_operation = file_ops::passthrough;
        return {};
    }

    auto ops = *ops_ptr;
    auto cast_ops = ops.if_string();
    if (cast_ops == nullptr)
    {
        return leaf::new_error(config_parse_error::field_wrong_type);
    }

    auto ops_str = *cast_ops;
    if (ops_str == "passthrough")
    {
        ret.file_operation = file_ops::passthrough;
    }
    else if (ops_str == "log")
    {
        ret.file_operation = file_ops::log;
    }
    else if (ops_str == "fail")
    {
        ret.file_operation = file_ops::fail;
    }
    else if (ops_str == "slow")
    {
        ret.file_operation = file_ops::slow;
    }
    else
    {
        return leaf::new_error(config_parse_error::field_unexpected_value);
    }

    return {};
}

leaf::result<file_node_from_config> _recursive_parse(const boost::json::value &val);

leaf::result<void> _parse_children(const boost::json::object &config_object, file_node_from_config &ret)
{
    auto children_ptr = config_object.if_contains("children");
    if (children_ptr != nullptr)
    {
        auto cast_children_ptr = children_ptr->if_array();
        if (cast_children_ptr == nullptr)
        {
            return leaf::new_error(config_parse_error::field_wrong_type);
        }
        auto children = *cast_children_ptr;
        for (auto child : children)
        {
            BOOST_LEAF_AUTO(child_result, _recursive_parse(child));
            ret.children.push_back(child_result);
        }
    }
    return {};
}

leaf::result<file_node_from_config> _recursive_parse(const boost::json::value &val)
{
    file_node_from_config ret;
    auto obj = val.if_object();
    if (obj == nullptr)
    {
        return leaf::new_error(config_parse_error::not_an_object);
    }
    auto config_object = *obj;
    BOOST_LEAF_CHECK(_parse_name(config_object, ret));
    BOOST_LEAF_CHECK(_parse_operation(config_object, ret));
    BOOST_LEAF_CHECK(_parse_children(config_object, ret));
    return ret;
}

leaf::result<file_node_from_config> _parse_tree(const boost::json::object &val)
{
    auto files_ptr = val.if_contains("files");
    if (files_ptr == nullptr)
    {
        return leaf::new_error(config_parse_error::missing_field);
    }

    auto files = files_ptr->if_object();
    if (files == nullptr)
    {
        return leaf::new_error(config_parse_error::field_wrong_type);
    }

    return _recursive_parse(*files);
}

leaf::result<std::string> _parse_mirror(const boost::json::object &val, const config_stream_options &opts)
{
    auto mirror_ptr = val.if_contains("mirror");
    if (mirror_ptr == nullptr)
    {
        return leaf::new_error(config_parse_error::missing_field);
    }

    auto mirror = mirror_ptr->if_string();
    if (mirror == nullptr)
    {
        return leaf::new_error(config_parse_error::field_wrong_type);
    }

    boost::filesystem::path p(opts.cwd_hint);
    std::string without_dot_slash(*mirror);
    if (without_dot_slash.substr(0, 2) == "./")
    {
        without_dot_slash = without_dot_slash.substr(2);
    }
    p.append(without_dot_slash);
    std::string result(p.string());
    return result;
}

leaf::result<file_tree> read_configuration_stream(std::istream &io, const config_stream_options &opts)
{
    boost::json::parse_options parse_opts;
    parse_opts.allow_comments = true;
    parse_opts.allow_trailing_commas = true;
    std::error_code ec;
    auto val = boost::json::parse(io, ec, {}, parse_opts);
    if (ec)
    {
        return leaf::new_error(config_parse_error::parse_error);
    }

    auto obj_ptr = val.if_object();
    if (obj_ptr == nullptr)
    {
        return leaf::new_error(config_parse_error::field_wrong_type);
    }
    auto obj = *obj_ptr;
    BOOST_LEAF_AUTO(parsed, _parse_tree(obj));
    BOOST_LEAF_AUTO(mirror, _parse_mirror(obj, opts));

    // Override root default
    parsed.is_root = true;
    parsed.name = "";

    file_tree tree;
    tree.root = parsed;
    tree.mirror = mirror;

    return tree;
}

leaf::result<file_tree> read_configuration_stream(std::istream &io)
{
    config_stream_options opts;
    return read_configuration_stream(io, opts);
}
