#pragma once

#ifndef BF_FUSE_PARSE_ENVIRONMENT
#define BF_FUSE_PARSE_ENVIRONMENT

#include <string>
#include <boost/leaf.hpp>
#include <boost/process.hpp>

namespace leaf = boost::leaf;

enum class parse_environment_errors
{
    not_found
};

struct bf_fuse_environment
{
    std::string config_directory;
};

leaf::result<bf_fuse_environment> parse_environment_variables(const boost::process::environment &env)
{
    return leaf::new_error(parse_environment_errors::not_found);
}

#endif