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
#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include <boost/system.hpp>

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

    leaf::result<readdir_result> readdir(const readdir_args &args)
    {
        readdir_result result;
        boost::system::error_code ec;
        boost::filesystem::path p(args.path);
        boost::filesystem::directory_iterator dir_iter(p, ec);
        for (auto &dir_entry : dir_iter)
        {
            if (ec)
            {
                return leaf::new_error(file_operations_error_codes::could_not_read_directory);
            }
            if (dir_entry.is_regular_file() || dir_entry.is_directory())
            {
                struct stat file_stat;
                auto stat_err = stat(dir_entry.path().c_str(), &file_stat);
                if (stat_err == 0)
                {
                    struct dirinfo info;
                    info.inode = file_stat.st_ino;
                    info.mode = file_stat.st_mode;
                    // TODO: this is a string copy, lets see if we can use a reference instead
                    info.name = dir_entry.path().filename().string();
                    args.filler(info);
                }
                else
                {
                    return leaf::new_error(file_operations_error_codes::could_not_stat_file);
                }
            }
        }

        if (ec)
        {
            return leaf::new_error(file_operations_error_codes::could_not_read_directory);
        }

        return result;
    }
};
