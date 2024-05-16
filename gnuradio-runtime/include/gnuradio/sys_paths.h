/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 * Copyright 2024 mboersch, Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef GR_SYS_PATHS_H
#define GR_SYS_PATHS_H

#include "api.h"
#include <gnuradio/api.h>
#include <filesystem>

namespace gr {
namespace paths {
/*! \brief directory to create temporary files.
 *
 * On UNIX-oid systems, typically /tmp.
 */
GR_RUNTIME_API std::filesystem::path tmp();

/*! \brief directory that stores user data; typicall $HOME
 */
GR_RUNTIME_API std::filesystem::path appdata();

/*! \brief directory that stores configuration.
 *
 * Defaults to $XDG_CONFIG_HOME/gnuradio (fallback: appdata()/.config/gnuradio), but if
 * that doesn't exist, checks the legacy path, appdata()/.gnuradio
 */
GR_RUNTIME_API std::filesystem::path userconf();

/*! \brief directory to store non-portable caches (e.g. FFTW wisdom)
 *
 * Defaults to $XDG_CACHE_HOME, falls back to appdata()/cache
 */
GR_RUNTIME_API std::filesystem::path cache();

/*! \brief directory to store persistent application state (e.g. window layouts, generated
 *          GRC hier blocks)
 */

GR_RUNTIME_API std::filesystem::path persistent();
} /* namespace  paths */

//! directory to create temporary files
[[deprecated("use gr::paths::tmp()")]] GR_RUNTIME_API const char* tmp_path();

//! directory to store application data
[[deprecated("use gr::paths::appdata()")]] GR_RUNTIME_API const char* appdata_path();

//! directory to store user configuration
[[deprecated("use gr::paths::userconf()")]] GR_RUNTIME_API const char* userconf_path();

} /* namespace gr */

#endif /* GR_SYS_PATHS_H */
