/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 * Copyright 2024 mboersch, Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/sys_paths.h>
#include <type_traits>
#include <cstdlib> //getenv
#include <filesystem>

namespace gr {

namespace paths {
std::filesystem::path tmp()
{
    // first case, try TMP environment variable
    const char* path = getenv("TMP");
    if (path) {
        return { path };
    }

    // second case, try P_tmpdir when its defined
#ifdef P_tmpdir
    if (P_tmpdir)
        return { P_tmpdir };
#endif /*P_tmpdir*/

    // fall-through case, nothing worked
    return { "/tmp" };
}
std::filesystem::path appdata()
{
    // first case, try HOME environment variable (unix)
    const char* path = getenv("HOME");
    if (path) {
        return { path };
    }

    // second case, try APPDATA environment variable (windows)
    path = getenv("APPDATA");
    if (path) {
        return { path };
    }

    // fall-through case, nothing worked
    return tmp();
}
std::filesystem::path userconf()
{
    namespace fs = std::filesystem;
    // explicit env variable always wins
    const char* path = getenv("GR_PREFS_PATH");
    if (path) {
        return { path };
    }

    // try $XDG_CONFIG_HOME/.config/gnuradio first
    path = getenv("XDG_CONFIG_HOME");
    fs::path xdg_path = (path ? fs::path{ path } : appdata() / ".config") / "gnuradio";
    if (fs::exists(xdg_path) && fs::is_directory(fs::canonical(xdg_path))) {
        // The new path is an existing directory – use it!
        return xdg_path;
    }

    // try the old path second
    auto legacy_path = appdata() / ".gnuradio";
    if (fs::exists(legacy_path) && fs::is_directory(fs::canonical(legacy_path))) {
        // The new path is an existing directory – use it!
        return legacy_path;
    }

    // if neither new nor old path exist as directory, return the new one.
    return xdg_path;
}

std::filesystem::path cache()
{
    namespace fs = std::filesystem;
    // explicit env variable always wins
    const char* path = getenv("GR_CACHE_PATH");
    if (path) {
        return { path };
    }
    path = getenv("XDG_CACHE_HOME");
    return (path ? fs::path{ path } : appdata() / ".cache") / "gnuradio";
}

std::filesystem::path persistent()
{
    namespace fs = std::filesystem;
    // explicit env variable always wins
    const char* path = getenv("GR_STATE_PATH");
    if (path) {
        return { path };
    }
    path = getenv("XDG_STATE_HOME");
    return (path ? fs::path{ path } : appdata() / ".local" / "state") / "gnuradio";
}
} // namespace paths

/* windows is peculiar: a path is typically a wstring with a not standard-specified
 * encoding. C++ is peculiar. There's no single safe way to convert without copies
 * that aren't necessary on most platforms. We're taking the sensible way out here,
 * and go through an intermediate string if necessary.
 */
template <typename path_type, bool sane>
struct converter {
    /* we have to make this a template type, since partial function templates are
     * impossible */
    static const char* c_str(const path_type& path);
};
template <typename path_type>
struct converter<path_type, true> {
    static const char* c_str(const path_type& path)
    {
        return (new std::string(path.c_str()))->c_str();
    }
};
template <typename path_type>
struct converter<path_type, false> {
    static const char* c_str(const path_type& path)
    {
        return (new std::string(path.string()))->c_str();
    }
};
const char* convert_path_to_C_string(const std::filesystem::path& path)
{
    /* Yes. This is a memory leak. The only alternative would have been to return the data
     * from a temporary string, which would have been a use-after-free. Notice that the
     * const char*-returning API was broken from the start: the original tmpdir_path()
     * used a static local variable to work around returning data from temporary objects;
     * which is a very bad idea, becausea suddenly the value of some C string changes
     * because someone somewhere else changed the environ. Like we need to do in unit
     * tests to avoid messing up system configuration, in our highly multi-threaded
     * framework.
     */
    return converter<
        std::filesystem::path,
        std::is_convertible<decltype(path.c_str()), const char*>::value>::c_str(path);
}

const char* tmp_path() { return convert_path_to_C_string(gr::paths::tmp()); }

const char* appdata_path() { return convert_path_to_C_string(gr::paths::appdata()); }

const char* userconf_path() { return convert_path_to_C_string(gr::paths::userconf()); }

} /* namespace gr */
