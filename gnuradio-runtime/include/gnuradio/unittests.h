/* -*- c++ -*- */
/*
 * Copyright 2010,2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <filesystem>
#include <string>

static std::string get_unittest_path(const std::string& filename)
{
    std::filesystem::path path = std::filesystem::current_path() / ".unittests";
    if (!std::filesystem::is_directory(path))
        std::filesystem::create_directory(path);
    return (path / filename).string();
}
