/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

namespace gr {

//! directory to create temporary files
const char* tmp_path();

//! directory to store application data
const char* appdata_path();

//! directory to store user configuration
const char* userconf_path();

} /* namespace gr */
