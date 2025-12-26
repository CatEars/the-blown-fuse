#pragma once

#ifndef BF_FUSE_PARSE_ENVIRONMENT
#define BF_FUSE_PARSE_ENVIRONMENT

#include <string>
#include <boost/leaf.hpp>
#include <boost/process.hpp>

namespace leaf = boost::leaf;

static const std::string config_directory_env_key = "BF_FUSE_CONFIG_DIRECTORY";

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
    bf_fuse_environment result;
    const auto &config_dir = env.find(config_directory_env_key);
    if (config_dir == env.end())
    {
        return leaf::new_error(parse_environment_errors::not_found);
    }

    result.config_directory = config_dir->to_string();

    return result;
}

#endif