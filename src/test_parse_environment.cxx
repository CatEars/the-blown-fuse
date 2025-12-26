#define BOOST_TEST_MODULE Parse Environment
#include <boost/test/included/unit_test.hpp>

#include <boost/process.hpp>

#include "parse_environment.hpp"

BOOST_AUTO_TEST_CASE(always_returns_error)
{
    auto env = boost::process::environment();
    auto result = parse_environment_variables(env);
    BOOST_TEST(result.has_error());
}
