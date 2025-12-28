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
#include <chrono>
#include <thread>
#include "../file_operations_data.hpp"

template <typename TResult, typename TArg>
using next_function = std::function<leaf::result<TResult>(TArg)>;

struct slow_operations
{
    leaf::result<getattr_result> getattr(const getattr_args &args, next_function<getattr_result, getattr_args> next)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        return next(args);
    }
};
