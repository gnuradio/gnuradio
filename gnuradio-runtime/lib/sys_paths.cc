/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/sys_paths.h>
#include <cstdio>  //P_tmpdir (maybe)
#include <cstdlib> //getenv
#include <filesystem>

namespace gr {

const char* tmp_path()
{
    const char* path;

    // first case, try TMP environment variable
    path = getenv("TMP");
    if (path)
        return path;

// second case, try P_tmpdir when its defined
#ifdef P_tmpdir
    if (P_tmpdir)
        return P_tmpdir;
#endif /*P_tmpdir*/

    // fall-through case, nothing worked
    return "/tmp";
}

const char* appdata_path()
{
    const char* path;

    // first case, try HOME environment variable (unix)
    path = getenv("HOME");
    if (path)
        return path;

    // second case, try APPDATA environment variable (windows)
    path = getenv("APPDATA");
    if (path)
        return path;

    // fall-through case, nothing worked
    return tmp_path();
}

std::string __userconf_path()
{
    const char* path;

    // First determine if there is an environment variable specifying the prefs path
    path = getenv("GR_PREFS_PATH");
    std::filesystem::path p;
    if (path) {
        p = path;
    } else {
        p = appdata_path();
        p = p / ".gnuradio";
    }

    return p.string();
}

const char* userconf_path()
{
    static std::string p(__userconf_path());
    return p.c_str();
}

} /* namespace gr */
