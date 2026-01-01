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
#include "options.hpp"
#include "plan_file_operations.hpp"
#include "execute_file_operations.hpp"
#include "file_tree.hpp"
#include "version.hpp"

namespace leaf = boost::leaf;
namespace po = boost::program_options;

static faked_file_tree global_file_tree;

int bf_fuse_getattr(
    const char *path,
    struct stat *stbuf,
    struct fuse_file_info *fi)
{
    std::string converted_path(path);
    auto plan_result = plan_file_operations(global_file_tree, converted_path);
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

    auto parsed = po::command_line_parser(argc, argv)
        .options(desc)
        .allow_unregistered() 
        .run();

    po::variables_map vm;
    po::store(parsed, vm);
    po::notify(vm);

    fuse_args_without_bf_fuse fargs = filter_fuse_args(argc, argv, parsed, vm);

    if (vm.count("help"))
    {
        // display help message from fuse
        fuse_main(argc, argv, &bf_fuse_oper, NULL);
        std::cout << std::endl
                  << desc;
        return 0;
    }

    if (vm.count("version"))
    {
        std::cout << "bf_fuse version " << BF_FUSE_VERSION << std::endl;
    }
    return fuse_main(fargs.argc, fargs.argv, &bf_fuse_oper, NULL);
}