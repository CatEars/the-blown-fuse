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
#include <fuse.h>
#include "fuse_params.h"

namespace leaf = boost::leaf;
namespace po = boost::program_options;

enum class err1
{
    e1,
    e2
};

leaf::result<int> getInt(bool useNegative, bool crash)
{
    if (useNegative && crash)
    {
        return leaf::new_error(err1::e1);
    }

    if (crash)
    {
        return leaf::new_error(err1::e2);
    }

    return useNegative ? -1 : 1;
}

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");

    auto options = desc.add_options();
    options("help", "produce help message");
    options("negative", "return negative value");
    options("crash", "crash and return error");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }

    bool useNegative = false;
    bool crash = false;
    if (vm.count("negative"))
    {
        useNegative = true;
    }
    if (vm.count("crash"))
    {
        crash = true;
    }

    leaf::result<int> result = leaf::try_handle_some(
        [&]() -> leaf::result<int>
        {
            return getInt(useNegative, crash);
        },
        [](err1 err) -> leaf::result<int>
        {
            switch (err)
            {
            case err1::e1:
                std::cout << "Failed with e1" << std::endl;
                break;
            case err1::e2:
                std::cout << "Failed with e2" << std::endl;
                break;
            }
            return leaf::new_error(err);
        });

    if (result.has_value())
    {
        std::cout << "Got: " << result.value() << std::endl;
        return 0;
    }
    else
    {
        return 1;
    }
}