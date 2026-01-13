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
#include <fstream>
#include <boost/leaf.hpp>
#include <boost/program_options.hpp>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include "fuse_params.hpp"
#include <fuse3/fuse.h>
#include "options.hpp"
#include "plan_file_operations.hpp"
#include "execute_file_operations.hpp"
#include "file_tree.hpp"
#include "config.hpp"
#include "version.hpp"

namespace leaf = boost::leaf;
namespace po = boost::program_options;

static std::string mountpoint;
static faked_file global_file_tree;
static fuse_fill_dir_flags fill_dir_plus;

std::string prepend_mountpoint(const char *path)
{
    std::stringstream ss;
    ss << mountpoint << path;
    return ss.str();
}

int bf_fuse_getattr(
    const char *path,
    struct stat *stbuf,
    struct fuse_file_info *fi)
{
    auto passthrough_getattr = [&](const getattr_args &arg)
    {
        return passthrough_ops.getattr(arg);
    };
    getattr_args args(prepend_mountpoint(path), passthrough_getattr);

    auto plan_result = plan_file_operations(global_file_tree, args.path);
    if (plan_result.has_error())
    {
        return -errno;
    }

    auto plan = plan_result.value();
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

int bf_fuse_readdir(
    const char *path,
    void *buf,
    fuse_fill_dir_t filler,
    off_t offset,
    struct fuse_file_info *fi,
    enum fuse_readdir_flags flags)
{
    auto passthrough_readdir = [&](const readdir_args &arg)
    {
        return passthrough_ops.readdir(arg);
    };

    auto filler_function = [&](const dirinfo &info)
    {
        // This snippet is copied from libfuse passthrough example
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = info.inode;
        st.st_mode = info.mode << 12;
        filler(buf, info.name.c_str(), &st, 0, fill_dir_plus);
    };

    readdir_args args(prepend_mountpoint(path), filler_function, passthrough_readdir);

    auto plan_result = plan_file_operations(global_file_tree, args.path);
    if (plan_result.has_error())
    {
        return -errno;
    }

    auto plan = plan_result.value();
    auto execute_result = execute_readdir(args, plan);
    if (execute_result.has_error())
    {
        return -errno;
    }

    auto execution = execute_result.value();
    if (execution.error != 0)
    {
        return -errno;
    }

    // result is carried back to libfuse by `filler_function` rather than as a result written down here.
    return 0;
}

static const struct fuse_operations bf_fuse_oper = {
    .getattr = bf_fuse_getattr,
    .readdir = bf_fuse_readdir,
};

int main(int argc, char *argv[])
{
    auto desc = get_program_options();
    auto positional_desc = get_program_positional_options();

    auto parsed = po::command_line_parser(argc, argv)
                      .options(desc)
                      .positional(positional_desc)
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
    else if (!vm.count("config"))
    {
        std::cerr << "config file must be specified for bf_fuse to work" << std::endl;
        return 1;
    }

    mountpoint = fargs.mountpoint;
    if (mountpoint.back() == '/')
    {
        // All fuse paths will start with `/....`
        // ensure we can just combine the mountpoint with any paths from fuse and be OK.
        mountpoint.pop_back();
    }
    const auto &config = vm.at("config");
    const auto &config_path = config.as<std::string>();
    boost::filesystem::path config_path_b(config_path);
    if (!boost::filesystem::exists(config_path_b))
    {
        std::cerr << "No config file at " << config_path << std::endl;
        return -1;
    }
    std::ifstream config_stream(config_path);

    auto config_result = leaf::try_handle_some(
        [&]() -> leaf::result<file_node_from_config>
        {
            return read_configuration_stream(config_stream);
        },
        [](config_parse_error parse_error) -> leaf::result<file_node_from_config>
        {
            std::cerr << parse_error << std::endl;
            return {};
        });

    if (config_result.has_error())
    {
        return 1;
    }
    global_file_tree = *config_result;
    return fuse_main(fargs.argc, fargs.argv, &bf_fuse_oper, NULL);
}