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
#include "plan_file_operations.hpp"
#include <boost/leaf.hpp>
#include <sys/stat.h>
#include <string>

enum class execution_errors
{
    not_implemented
};

struct getattr_args
{
    const std::string path;

public:
    getattr_args(const char *_path) : path(_path)
    {
    }
};

struct getattr_result
{
    int error = 0;
    struct stat stbuf;
};

struct passthrough_operations
{
    leaf::result<getattr_result> getattr(const getattr_args &args)
    {
        getattr_result result;
        int res = lstat(args.path.c_str(), &result.stbuf);
        if (res == -1)
        {
            result.error = -errno;
        }
        return result;
    }
};

static passthrough_operations _passthrough_ops;

leaf::result<getattr_result> execute_getattr(
    const getattr_args &args, file_ops ops)
{
    if (ops == file_ops::passthrough)
    {
        return _passthrough_ops.getattr(args);
    }
    return leaf::new_error(execution_errors::not_implemented);
}
