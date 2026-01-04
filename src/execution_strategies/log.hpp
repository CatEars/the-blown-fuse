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
#include "../file_operations_data.hpp"
#include <boost/log/trivial.hpp>
#include <boost/leaf.hpp>

namespace leaf = boost::leaf;

struct log_operations
{
    leaf::result<getattr_result> getattr(const getattr_args &args)
    {
        BOOST_LOG_TRIVIAL(info) << "<getattr path='" << args.path << "' />";
        return args.next(args);
    }
};
