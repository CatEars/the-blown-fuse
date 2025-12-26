#pragma once

#ifndef BF_FUSE_GET_INT
#define BF_FUSE_GET_INT

#include <boost/leaf.hpp>

enum class my_errors
{
    e1,
    e2
};

boost::leaf::result<int> get_int(bool use_negative, bool crash)
{
    if (use_negative && crash)
    {
        return boost::leaf::new_error(my_errors::e1);
    }

    if (crash)
    {
        return boost::leaf::new_error(my_errors::e2);
    }

    return use_negative ? -1 : 1;
}

#endif