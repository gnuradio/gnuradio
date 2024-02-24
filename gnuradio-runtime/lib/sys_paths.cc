/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/sys_paths.h>
#include <cstdlib> //getenv
#include <filesystem>

namespace gr {

std::filesystem::path tmp_path()
{
    const char* path;

    // first case, try TMP environment variable
    path = getenv("TMP");
    if (path)
        return { path };

// second case, try P_tmpdir when its defined
#ifdef P_tmpdir
    if (P_tmpdir)
        return { P_tmpdir };
#endif /*P_tmpdir*/

    // fall-through case, nothing worked
    return { "/tmp" };
}

std::filesystem::path appdata_path()
{
    // first case, try HOME environment variable (unix)
    auto path = getenv("HOME");
    if (path)
        return { path };

    // second case, try APPDATA environment variable (windows)
    path = getenv("APPDATA");
    if (path)
        return { path };

    // fall-through case, nothing worked
    return tmp_path();
}

std::filesystem::path userconf_path()
{
    const char* path;

    // First determine if there is an environment variable specifying the prefs path
    path = getenv("GR_PREFS_PATH");
    std::filesystem::path p;
    if (path) {
        return { path };
    }

    // Second, adhere to the XDG spec instead of ~/.gnuradio
    path = getenv("XDG_CONFIG_HOME");
    if (path) {
        p = path;
    } else {
        // we should default to $HOME/.config, for portability we use the
        // appdata_path
        p = appdata_path() / ".config";
    }
    p = p / "gnuradio";

    return p;
}

} /* namespace gr */
