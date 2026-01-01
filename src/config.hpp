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

namespace leaf = boost::leaf;

enum class config_parse_error
{
    parse_error,
    not_an_object,
    missing_field,
    field_wrong_type
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
    default:
        return os << "unknown";
    }
}

struct file_node_from_config
{
    std::string name;
    std::vector<file_node_from_config> children;

    bool is_directory() const {
        return this->children.size() == 0;
    }
};

leaf::result<file_node_from_config> _recursive_parse(const boost::json::value& val) 
{
    file_node_from_config ret;
    auto obj = val.if_object();
    if (obj == nullptr) {
        return leaf::new_error(config_parse_error::not_an_object);
    }
    auto config_object = *obj;
    auto name_ptr = config_object.if_contains("name");
    if (name_ptr == nullptr) {
        return leaf::new_error(config_parse_error::missing_field);
    }
    auto name = name_ptr->if_string();
    if (name == nullptr) {
        return leaf::new_error(config_parse_error::field_wrong_type);
    }
    ret.name = *name;
    auto children_ptr = config_object.if_contains("children");
    if (children_ptr != nullptr) {
        auto cast_children_ptr = children_ptr->if_array();
        if (cast_children_ptr == nullptr) {
            return leaf::new_error(config_parse_error::field_wrong_type);
        }
        auto children = *cast_children_ptr;
        for (auto child : children) {
            BOOST_LEAF_AUTO(child_result, _recursive_parse(child));
            ret.children.push_back(child_result);
        }
    }
    return ret;
}

leaf::result<file_node_from_config> read_configuration_stream(std::istream& io)
{
    boost::json::parse_options parse_opts;
    parse_opts.allow_comments = true;
    parse_opts.allow_trailing_commas = true;
    std::error_code ec;
    auto val = boost::json::parse(io, ec, {}, parse_opts);
    if (ec) {
        return leaf::new_error(config_parse_error::parse_error);
    }
    return _recursive_parse(val);
}