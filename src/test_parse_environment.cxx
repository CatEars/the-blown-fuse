#define BOOST_TEST_MODULE Parse Environment
#include <boost/test/included/unit_test.hpp>

#include <boost/process.hpp>

#include "parse_environment.hpp"

BOOST_AUTO_TEST_CASE(without_bf_fuse_config_directory_set_returns_error)
{
    auto env = boost::process::environment();
    env.set(config_directory_env_key, "");
    auto result = parse_environment_variables(env);
    BOOST_TEST(result.has_error());
}

BOOST_AUTO_TEST_CASE(with_bf_fuse_config_directory_set_returns_value_of_that)
{
    auto env = boost::process::environment();
    env.set(config_directory_env_key, "/tmp");
    auto result = parse_environment_variables(env);
    BOOST_TEST(!result.has_error());
    auto environment = result.value();
    BOOST_TEST(environment.config_directory == "/tmp");
}
