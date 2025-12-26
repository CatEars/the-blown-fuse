#define BOOST_TEST_MODULE Get Int Test
#include <boost/test/included/unit_test.hpp>
#include "get_int.hpp"

BOOST_AUTO_TEST_CASE(crash_returns_error_code)
{
    auto result = get_int(false, true);
    BOOST_TEST(result.has_error());
}

BOOST_AUTO_TEST_CASE(use_negative_returns_negative_number)
{
    auto result = get_int(true, false);
    BOOST_TEST(!result.has_error());
    BOOST_TEST(result.value() < 0);
}

BOOST_AUTO_TEST_CASE(no_use_negative_returns_positive_number)
{
    auto result = get_int(false, false);
    BOOST_TEST(!result.has_error());
    BOOST_TEST(result.value() > 0);
}