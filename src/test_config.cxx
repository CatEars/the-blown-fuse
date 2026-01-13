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
#define BOOST_TEST_MODULE Read configuration
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <sstream>
#include "config.hpp"
#include "plannable_operations.hpp"

file_tree read_valid_config_string(const std::string &config)
{
    std::stringstream ss(config);
    auto result = read_configuration_stream(ss);
    BOOST_TEST(!result.has_error());
    return result.value();
}

leaf::result<file_tree> read_invalid_config_string(const std::string &config)
{
    std::stringstream ss(config);
    auto result = read_configuration_stream(ss);
    BOOST_TEST(result.has_error());
    return result;
}

void assert_error_code(const std::string &config, config_parse_error expected_err)
{
    auto _ = leaf::try_handle_some(
        [&]()
        { return read_invalid_config_string(config); },
        [&](const config_parse_error &err, const leaf::error_info &ei) -> leaf::result<file_tree>
        {
            BOOST_TEST(err == expected_err);
            return ei.error();
        },
        [](const leaf::error_info &ei) -> leaf::result<file_tree>
        {
            BOOST_FAIL("Did not get expected error code");
            return ei.error();
        });
}

BOOST_AUTO_TEST_CASE(root_node_is_root_but_not_child_nodes)
{
    auto config = read_valid_config_string(R"({
            "mirror":"",
            "files": {
                "name":"",
                "children": [
                    {
                        "name": "child"
                    }
                ]
            }
        })");
    BOOST_TEST(config.root.is_root);
    BOOST_TEST(!config.root.children[0].is_root);
}

BOOST_AUTO_TEST_CASE(root_node_name_is_overwritten_to_empty_string)
{
    auto config = read_valid_config_string(R"({"mirror": "", "files": {"name":"test"}})");
    BOOST_TEST(config.root.name == "");
}

BOOST_AUTO_TEST_CASE(can_specify_child_nodes)
{
    auto config = read_valid_config_string(R"({
        "mirror":"",
        "files": {
            "name":"", 
            "children": [
                {
                    "name":"child1"
                },
                {
                    "name":"child2",
                    "children": [
                        {
                            "name": "child2-child1"
                        }
                    ]
                }
            ]
        }
    })");
    BOOST_TEST(config.root.children.size() == 2);
    BOOST_TEST(config.root.children[0].name == "child1");
    BOOST_TEST(config.root.children[0].children.size() == 0);
    BOOST_TEST(config.root.children[1].name == "child2");
    BOOST_TEST(config.root.children[1].children.size() == 1);
    BOOST_TEST(config.root.children[1].children[0].name == "child2-child1");
    BOOST_TEST(config.root.children[1].children[0].children.size() == 0);
}

BOOST_AUTO_TEST_CASE(default_file_ops_is_passthrough)
{
    auto config = read_valid_config_string(R"({"mirror":"", "files": {"name":""}})");
    BOOST_TEST(config.root.file_operation == file_ops::passthrough);
}

BOOST_AUTO_TEST_CASE(returns_error_when_name_is_wrong_type)
{
    assert_error_code(R"({"mirror":"", "files": {"name":true}})", config_parse_error::field_wrong_type);
}

BOOST_AUTO_TEST_CASE(returns_error_when_children_is_wrong_type)
{
    assert_error_code(R"({"mirror":"", "files": {"name":"", "children":false}})", config_parse_error::field_wrong_type);
}

BOOST_AUTO_TEST_CASE(returns_error_when_name_is_not_there)
{
    assert_error_code(R"({"mirror":"", "files": {"not-name":""}})",
                      config_parse_error::missing_field);
}

BOOST_AUTO_TEST_CASE(returns_error_when_invalid_json)
{
    assert_error_code(R"({"mirror":"", "files": {"not-name":""})", config_parse_error::parse_error);
}

std::vector<file_ops> tested_file_ops{
    file_ops::passthrough,
    file_ops::fail,
    file_ops::log,
    file_ops::slow};

namespace bdata = boost::unit_test::data;

BOOST_DATA_TEST_CASE(can_parse_all_valid_file_enums, bdata::make(tested_file_ops), file_op)
{
    std::stringstream ss;
    ss << file_op;
    auto config = R"({"mirror":"", "files": {"name":"","operation":")" + ss.str() + R"("}})";
    auto result = read_valid_config_string(config);
    BOOST_TEST(result.root.file_operation == file_op);
}

BOOST_AUTO_TEST_CASE(returns_value_of_mirror)
{
    auto config = R"({"mirror":"test-mirror", "files": {"name":""}})";
    auto result = read_valid_config_string(config);
    BOOST_TEST(result.mirror == "test-mirror");
}

BOOST_AUTO_TEST_CASE(returns_fault_when_no_mirror)
{
    assert_error_code(R"({"files": {"name":""}})",
                      config_parse_error::missing_field);
}

BOOST_AUTO_TEST_CASE(returns_fault_when_mirror_wrong_type)
{
    assert_error_code(R"({"mirror":false, "files": {"name":""}})",
                      config_parse_error::field_wrong_type);
}
