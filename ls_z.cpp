/*
 * Copyright (C) 2014  Andrew Gunnerson <andrewgunnerson@gmail.com>
 *
 * This file is part of MultiBootPatcher
 *
 * MultiBootPatcher is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MultiBootPatcher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MultiBootPatcher.  If not, see <http://www.gnu.org/licenses/>.
 */

#define __STDC_FORMAT_MACROS

#include <string>
#include <vector>

#include <cerrno>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/xattr.h>

#define SELINUX_XATTR           "security.selinux"


static bool selinux_lget_context(const std::string &path, std::string *context)
{
    ssize_t size;
    std::vector<char> value;

    size = lgetxattr(path.c_str(), SELINUX_XATTR, nullptr, 0);
    if (size < 0) {
        return false;
    }

    value.resize(size);

    size = lgetxattr(path.c_str(), SELINUX_XATTR, value.data(), size);
    if (size < 0) {
        return false;
    }

    value.push_back('\0');
    *context = value.data();

    return true;
}

static bool list_file(const char *path)
{
    std::string context("(error)");
    struct stat sb;

    if (lstat(path, &sb) < 0) {
        fprintf(stderr, "%s: Failed to lstat: %s\n",
                path, strerror(errno));
        return false;
    }

    selinux_lget_context(path, &context);

    printf("mode: %o; nlink: %u; uid: %u; gid: %u; size: %" PRIu64 "; "
           "context: %s; path: %s\n",
           sb.st_mode, sb.st_nlink, sb.st_uid, sb.st_gid, sb.st_size,
           context.c_str(), path);

    return true;
}

static bool list_dir(const char *path)
{
    DIR *dp = opendir(path);
    if (!dp) {
        fprintf(stderr, "%s: Failed to open directory: %s\n",
                path, strerror(errno));
        return false;
    }

    struct dirent *ent;
    std::string curpath;
    std::string context;

    while ((ent = readdir(dp))) {
        curpath = path;
        curpath += '/';
        curpath += ent->d_name;

        if (!list_file(curpath.c_str())) {
            continue;
        }
    }

    closedir(dp);
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct stat sb;

    if (lstat(argv[1], &sb) < 0) {
        fprintf(stderr, "%s: Failed to lstat: %s\n", argv[1], strerror(errno));
        return EXIT_FAILURE;
    }

    if (S_ISDIR(sb.st_mode)) {
        return list_dir(argv[1]) ? EXIT_SUCCESS : EXIT_FAILURE;
    } else {
        return list_file(argv[1]) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
}
