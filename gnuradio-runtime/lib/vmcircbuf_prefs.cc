/* -*- c++ -*- */
/*
 * Copyright 2003,2010,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "vmcircbuf.h"
#include "vmcircbuf_prefs.h"
#include <gnuradio/sys_paths.h>
#include <string_view>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
namespace fs = std::filesystem;

namespace gr {

/*
 * The simplest thing that could possibly work:
 *  the key is the filename; the value is the file contents.
 */

template <typename string_type>
static auto pathname(string_type key)
{
    return gr::paths::userconf() / "prefs" / key;
}

static void ensure_dir_path()
{
    // recursively make sure the directory exists
    fs::path path = gr::paths::userconf() / "prefs";
    fs::create_directories(path);
}

template <typename stream_t>
static void
log_ioerror(const std::string& who, const fs::path& path, const stream_t& stream)
{
    gr::logger logger(std::string{ "vmcircbuf_prefs::" } + who);
    logger.set_level(logging::singleton().default_level());
    logger.warn("{} failed to open: bad {}, fail {}, eof {}",
                path.string(),
                static_cast<bool>(stream.badbit),
                static_cast<bool>(stream.failbit),
                static_cast<bool>(stream.eofbit));
}

std::optional<std::string> vmcircbuf_prefs::get(std::string_view key)
{
    auto path = pathname(key);
    gr::thread::scoped_lock guard(s_vm_mutex);
    std::ifstream in_file(path, std::ios::in | std::ios::binary);
    if (!in_file.good()) {
        log_ioerror("get(open)", path, in_file);
        return {};
    }

    try {
        in_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
        return { { std::istreambuf_iterator<char>{ in_file }, {} } };
    } catch (const std::ios_base::failure& fail) {
        log_ioerror("get(read)", path, in_file);
        return {};
    }
}

bool vmcircbuf_prefs::set(std::string_view key, std::string_view value)
{
    gr::logger logger("vmcircbuf_prefs::set");
    logger.set_level(logging::singleton().default_level());
    gr::thread::scoped_lock guard(s_vm_mutex);

    ensure_dir_path();
    auto path = pathname(key);

    std::ofstream out_file(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!out_file.good()) {
        log_ioerror("set(open)", path, out_file);
        return false;
    }

    try {
        out_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
        out_file << value;
        out_file.close();
        return true;
    } catch (const std::ios_base::failure& fail) {
        log_ioerror("set(write)", path, out_file);
        logger.error("Could not write vmcircbuf preference.");
    }
    return false;
};

} /* namespace gr */
