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
#include <boost/leaf.hpp>
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include "plan_file_operations.hpp"
#include "file_operations_data.hpp"
#include "execution_strategies/fail.hpp"
#include "execution_strategies/log.hpp"
#include "execution_strategies/passthrough.hpp"
#include "execution_strategies/slow.hpp"

enum class execution_errors
{
    not_implemented
};

static passthrough_operations passthrough_ops;
static slow_operations _slow_ops;
static fail_operations _fail_ops;
static log_operations _log_ops;

leaf::result<getattr_result>
execute_getattr(
    const getattr_args &args, file_ops ops)
{
    if (ops == file_ops::passthrough)
    {
        return passthrough_ops.getattr(args);
    }
    else if (ops == file_ops::slow)
    {
        return _slow_ops.getattr(args);
    }
    else if (ops == file_ops::fail)
    {
        return _fail_ops.getattr(args);
    }
    else if (ops == file_ops::log)
    {
        return _log_ops.getattr(args);
    }
    return leaf::new_error(execution_errors::not_implemented);
}
