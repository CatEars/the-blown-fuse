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

file_node_from_config read_valid_config_string(const std::string &config)
{
    std::stringstream ss(config);
    auto result = read_configuration_stream(ss);
    BOOST_TEST(!result.has_error());
    return result.value();
}

leaf::result<file_node_from_config> read_invalid_config_string(const std::string &config)
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
        [&](const config_parse_error &err, const leaf::error_info &ei) -> leaf::result<file_node_from_config>
        {
            BOOST_TEST(err == expected_err);
            return ei.error();
        },
        [](const leaf::error_info &ei) -> leaf::result<file_node_from_config>
        {
            BOOST_FAIL("Did not get expected error code");
            return ei.error();
        });
}

BOOST_AUTO_TEST_CASE(can_read_with_a_root_config)
{
    auto config = read_valid_config_string(R"({"name":""})");
    BOOST_TEST(config.children.size() == 0);
    BOOST_TEST(config.name == "");
}

BOOST_AUTO_TEST_CASE(can_specify_child_nodes)
{
    auto config = read_valid_config_string(R"({
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
    })");
    BOOST_TEST(config.children.size() == 2);
    BOOST_TEST(config.children[0].name == "child1");
    BOOST_TEST(config.children[0].children.size() == 0);
    BOOST_TEST(config.children[1].name == "child2");
    BOOST_TEST(config.children[1].children.size() == 1);
    BOOST_TEST(config.children[1].children[0].name == "child2-child1");
    BOOST_TEST(config.children[1].children[0].children.size() == 0);
}

BOOST_AUTO_TEST_CASE(default_file_ops_is_passthrough)
{
    auto config = read_valid_config_string(R"({"name":""})");
    BOOST_TEST(config.file_ops == file_ops::passthrough);
}

BOOST_AUTO_TEST_CASE(returns_error_when_name_is_wrong_type)
{
    assert_error_code(R"({"name":true})", config_parse_error::field_wrong_type);
}

BOOST_AUTO_TEST_CASE(returns_error_when_children_is_wrong_type)
{
    assert_error_code(R"({"name":"","children":false})", config_parse_error::field_wrong_type);
}

BOOST_AUTO_TEST_CASE(returns_error_when_name_is_not_there)
{
    assert_error_code(R"({"not-name":""})", config_parse_error::missing_field);
}

BOOST_AUTO_TEST_CASE(returns_error_when_invalid_json)
{
    assert_error_code(R"({"name":"")", config_parse_error::parse_error);
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
    auto config = R"({"name":"","operation":")" + ss.str() + R"("})";
    auto result = read_valid_config_string(config);
    BOOST_TEST(result.file_ops == file_op);
}