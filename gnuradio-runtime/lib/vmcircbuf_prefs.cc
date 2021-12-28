/* -*- c++ -*- */
/*
 * Copyright 2003,2010,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <fstream>
#include <ios>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "vmcircbuf.h"
#include "vmcircbuf_prefs.h"
#include <gnuradio/sys_paths.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <boost/format.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
namespace fs = std::filesystem;

namespace gr {

/*
 * The simplest thing that could possibly work:
 *  the key is the filename; the value is the file contents.
 */

static std::string pathname(const std::string& key)
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

std::string vmcircbuf_prefs::get(const std::string& key)
{
    gr::logger_ptr logger, debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "vmcircbuf_prefs::get");

    gr::thread::scoped_lock guard(s_vm_mutex);

    std::ifstream file;
    file.exceptions(file.exceptions() | std::ios::failbit);

    try {
        file.open(pathname(key));
        std::string value;
        file >> value;
        return value;
    } catch (std::ios_base::failure& exception) {
        logger->error("{} : {}", pathname(key), exception.what());
        throw exception;
    }
}

void vmcircbuf_prefs::set(const char* key, const char* value)
{
    gr::thread::scoped_lock guard(s_vm_mutex);

    ensure_dir_path();
    gr::logger_ptr logger, debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "vmcircbuf_prefs::set");

    FILE* fp = fopen(pathname(key).c_str(), "w");
    if (fp == 0) {
        GR_LOG_ERROR(logger,
                     boost::format("%s: %s") % pathname(key).c_str() % strerror(errno));
        return;
    }

    size_t ret = fwrite(value, 1, strlen(value), fp);
    if (ret == 0) {
        if (ferror(fp) != 0) {
            GR_LOG_ERROR(logger,
                         boost::format("%s: %s") % pathname(key).c_str() %
                             strerror(errno));
            fclose(fp);
            return;
        }
    }
    fclose(fp);
};

} /* namespace gr */
