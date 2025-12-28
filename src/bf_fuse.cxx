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
#include <iostream>
#include <boost/leaf.hpp>
#include <boost/program_options.hpp>
#include <boost/process.hpp>
#include "fuse_params.hpp"
#include <fuse3/fuse.h>
#include "arguments.hpp"
#include "plan_file_operations.hpp"
#include "execute_file_operations.hpp"

namespace leaf = boost::leaf;
namespace po = boost::program_options;

int bf_fuse_getattr(
    const char *path,
    struct stat *stbuf,
    struct fuse_file_info *fi)
{
    auto plan_result = plan_file_operations();
    if (plan_result.has_error())
    {
        return -errno;
    }

    auto plan = plan_result.value();
    getattr_args args(path);
    auto execute_result = execute_getattr(args, plan);
    if (execute_result.has_error())
    {
        return -errno;
    }

    auto execution = execute_result.value();
    if (execution.error != 0)
    {
        return -errno;
    }

    *stbuf = execution.stbuf;
    return 0;
}

static const struct fuse_operations bf_fuse_oper = {
    .getattr = bf_fuse_getattr,
};

int main(int argc, char *argv[])
{
    auto desc = get_program_options();

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc;
        return 0;
    }

    return fuse_main(argc, argv, &bf_fuse_oper, NULL);
}