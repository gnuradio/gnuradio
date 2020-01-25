/* -*- c++ -*- */
/*
 * Copyright 2003,2010,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "vmcircbuf.h"
#include "vmcircbuf_prefs.h"
#include <gnuradio/sys_paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

namespace gr {

/*
 * The simplest thing that could possibly work:
 *  the key is the filename; the value is the file contents.
 */

static std::string pathname(const char* key)
{
    static fs::path path;
    path = fs::path(gr::appdata_path()) / ".gnuradio" / "prefs" / key;
    return path.string();
}

static void ensure_dir_path()
{
    fs::path path = fs::path(gr::appdata_path()) / ".gnuradio";
    if (!fs::is_directory(path))
        fs::create_directory(path);

    path = path / "prefs";
    if (!fs::is_directory(path))
        fs::create_directory(path);
}

int vmcircbuf_prefs::get(const char* key, char* value, int value_size)
{
    gr::thread::scoped_lock guard(s_vm_mutex);

    FILE* fp = fopen(pathname(key).c_str(), "r");
    if (fp == 0) {
        perror(pathname(key).c_str());
        return 0;
    }

    const size_t ret = fread(value, 1, value_size - 1, fp);
    value[ret] = '\0';
    if (ret == 0 && !feof(fp)) {
        if (ferror(fp) != 0) {
            perror(pathname(key).c_str());
            fclose(fp);
            return -1;
        }
    }
    fclose(fp);
    return ret;
}

void vmcircbuf_prefs::set(const char* key, const char* value)
{
    gr::thread::scoped_lock guard(s_vm_mutex);

    ensure_dir_path();

    FILE* fp = fopen(pathname(key).c_str(), "w");
    if (fp == 0) {
        perror(pathname(key).c_str());
        return;
    }

    size_t ret = fwrite(value, 1, strlen(value), fp);
    if (ret == 0) {
        if (ferror(fp) != 0) {
            perror(pathname(key).c_str());
            fclose(fp);
            return;
        }
    }
    fclose(fp);
};

} /* namespace gr */
