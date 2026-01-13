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
#include <string>
#include <vector>
#include <sys/stat.h>
#include <memory>

template <typename TResult, typename TArg>
using next_function = std::function<leaf::result<TResult>(TArg)>;

enum class file_operations_error_codes
{
    could_not_read_directory,
    could_not_stat_file
};

struct dirinfo
{
    ino_t inode;
    mode_t mode;
    std::string name;
};

using filler_function = std::function<void(const dirinfo &)>;

struct getattr_result
{
    int error = 0;
    struct stat stbuf;
};

struct getattr_args
{
    const std::string path;
    const next_function<getattr_result, getattr_args> next;

    getattr_args(const std::string &_path, const next_function<getattr_result, getattr_args> _next)
        : path(_path), next(_next)
    {
    }
};

struct readdir_result
{
    int error = 0;
};

struct readdir_args
{
    const std::string path;
    const filler_function filler;
    const next_function<readdir_result, readdir_args> next;

    readdir_args(
        const std::string &_path,
        const filler_function &_filler,
        const next_function<readdir_result, readdir_args> _next)
        : path(_path), filler(_filler), next(_next)
    {
    }
};