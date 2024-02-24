/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef GR_SYS_PATHS_H
#define GR_SYS_PATHS_H

#include <gnuradio/api.h>
#include <filesystem>

namespace gr {

//! directory to create temporary files
GR_RUNTIME_API std::filesystem::path tmp_path();

//! directory to store application data
GR_RUNTIME_API std::filesystem::path appdata_path();

//! directory to store user configuration
GR_RUNTIME_API std::filesystem::path userconf_path();

} /* namespace gr */

#endif /* GR_SYS_PATHS_H */
