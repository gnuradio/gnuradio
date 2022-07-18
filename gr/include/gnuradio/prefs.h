/* -*- c++ -*- */
/*
 * Copyright 2006,2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once


#include <gnuradio/constants.h>
#include <gnuradio/sys_paths.h>
#include <yaml-cpp/yaml.h>
#include <cstdio>  //P_tmpdir (maybe)
#include <cstdlib> //getenv
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

namespace gr {

class prefs
{
public:
    static prefs* get_instance();
    static YAML::Node get_section(const std::string& name);
    static const std::string get_string(const std::string& section,
                                        const std::string& option,
                                        const std::string& default_val);
    static const double get_double(const std::string& section,
                                   const std::string& option,
                                   const double default_val);
    static const long get_long(const std::string& section,
                               const std::string& option,
                               const long default_val);
    static const bool get_bool(const std::string& section,
                               const std::string& option,
                               const bool default_val);
    static const char* appdata_path();

private:
    YAML::Node _config;

    prefs();
    void _read_files(const std::vector<std::string>& filenames);
    static const char* tmp_path();
    std::vector<std::string> _sys_prefs_filenames();
};

} // namespace gr