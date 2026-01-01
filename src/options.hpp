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
#include <boost/program_options.hpp>
#include <vector>
#include <string>
#include <cstring>

namespace po = boost::program_options;

po::options_description get_program_options()
{
    po::options_description desc("bf_fuse options");
    auto options = desc.add_options();
    options("help,h", "print help");
    options("version,V", "print version");
    options("config,c", boost::program_options::value<std::string>(), "path to configuration file");
    return desc;
}

struct fuse_args_without_bf_fuse {
    int argc;
    char** argv;

    void free() {
        for (int i = 0; i < argc; ++i) delete[] argv[i];
        delete[] argv;
    }
};

fuse_args_without_bf_fuse filter_fuse_args(int argc, char* argv[], 
    const po::parsed_options& parsed,
    const po::variables_map& vm) {
    std::vector<std::string> unrecognized = po::collect_unrecognized(parsed.options, po::include_positional);
    std::vector<std::string> fuse_vec;
    
    fuse_vec.push_back(argv[0]);
    fuse_vec.insert(fuse_vec.end(), unrecognized.begin(), unrecognized.end());

    if (vm.count("help")) {
        fuse_vec.push_back("-h");
    }
    if (vm.count("version")) {
        fuse_vec.push_back("-V");
    }

    int f_argc = static_cast<int>(fuse_vec.size());
    char** f_argv = new char*[f_argc + 1];
    for (int i = 0; i < f_argc; ++i) {
        f_argv[i] = new char[fuse_vec[i].size() + 1];
        std::copy(fuse_vec[i].begin(), fuse_vec[i].end(), f_argv[i]);
        f_argv[i][fuse_vec[i].size()] = '\0';
    }
    f_argv[f_argc] = nullptr;

    return { f_argc, f_argv };
}
