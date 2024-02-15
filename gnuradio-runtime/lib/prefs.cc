/* -*- c++ -*- */
/*
 * Copyright 2006,2013,2015 Free Software Foundation, Inc.
 * Copyright 2024 mboersch, Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/constants.h>
#include <gnuradio/prefs.h>
#include <gnuradio/sys_paths.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>
namespace fs = std::filesystem;
namespace po = boost::program_options;
typedef std::ifstream::char_type char_t;

namespace gr {

namespace {

void stolower(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}
void stoupper(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}

} // namespace

prefs* prefs::singleton()
{
    static prefs instance;
    return &instance;
}

prefs::prefs() { _read_files(_sys_prefs_filenames()); }

std::vector<std::string> prefs::_sys_prefs_filenames()
{
    std::vector<std::string> fnames;

    fs::path dir = prefsdir();
    if (fs::is_directory(dir)) {
        for (const auto& p : fs::directory_iterator(dir)) {
            if (p.path().extension() == ".conf")
                fnames.push_back(p.path().string());
        }
        std::sort(fnames.begin(), fnames.end());
    }

    // Find if there is a ~/.gnuradio/config.conf file and add this to
    // the end of the file list to override any preferences in the
    // installed path config files.
    auto userconf =
        gr::paths::userconf() / "config.conf"; // TODO move filename to constants
    if (fs::exists(userconf)) {
        fnames.push_back(userconf.string());
    }

    return fnames;
}

void prefs::_read_files(const std::vector<std::string>& filenames)
{
    for (const auto& fname : filenames) {
        std::ifstream infile(fname.c_str());
        if (!infile.good()) {
            std::cerr << "WARNING: Config file '" << fname
                      << "' could not be opened for reading." << std::endl;
            continue;
        }

        try {
            po::basic_parsed_options<char_t> parsed =
                po::parse_config_file(infile, po::options_description(), true);
            for (const auto& o : parsed.options) {
                std::string okey = o.string_key;
                size_t pos = okey.find(".");
                std::string section, key;
                if (pos != std::string::npos) {
                    section = okey.substr(0, pos);
                    key = okey.substr(pos + 1);
                } else {
                    section = "default";
                    key = okey;
                }
                stolower(section);
                stolower(key);
                // value of a basic_option is always a std::vector<string>; we only
                // allow single values, so:
                std::string value = o.value[0];
                std::lock_guard<std::mutex> lk(d_mutex);
                d_config_map[section][key] = value;
            }
        } catch (std::exception& e) {
            std::cerr << "WARNING: Config file '" << fname
                      << "' failed to parse:" << std::endl;
            std::cerr << e.what() << std::endl;
            std::cerr << "Skipping it" << std::endl;
        }
    }
}

void prefs::add_config_file(const std::string& configfile)
{
    _read_files(std::vector<std::string>{ configfile });
}


std::string prefs::to_string()
{
    std::stringstream s;
    std::lock_guard<std::mutex> lk(d_mutex);

    for (const auto& sections : d_config_map) {
        s << "[" << sections.first << "]" << std::endl;
        for (const auto& options : sections.second) {
            s << options.first << " = " << options.second << std::endl;
        }
        s << std::endl;
    }

    return s.str();
}

void prefs::save()
{
    const std::string conf = to_string();

    // Write temp file.
    const auto tmp =
        gr::paths::userconf() / "config.conf.tmp"; // TODO move filename to constants
    std::ofstream fout(tmp);
    fout << conf;
    fout.close();
    if (!fout.good()) {
        const std::string write_err = strerror(errno);
        {
            std::error_code err;
            fs::remove(tmp, err);
            if (err) {
                std::cerr << "Failed to remove temp file: " << err << std::endl;
            }
        }
        throw std::runtime_error("failed to write updated config: " + write_err);
    }

    // Atomic rename.
    const fs::path userconf = gr::paths::userconf() / "config.conf";
    fs::rename(tmp, userconf);
    // If fs::rename() fails, we'll leak the tempfile and throw. That's fine.
    // If the user wants it (it was written successfully) they can have it.
    // Or it'll be overwritten on the next save.
}

char* prefs::option_to_env(std::string section, std::string option)
{
    stoupper(section);
    stoupper(option);
    const auto envname = "GR_CONF_" + section + "_" + option;
    return getenv(envname.c_str());
}

bool prefs::has_section(const std::string& section)
{
    std::string s = section;
    stolower(s);
    std::lock_guard<std::mutex> lk(d_mutex);
    return d_config_map.count(s) > 0;
}

bool prefs::has_option(const std::string& section, const std::string& option)
{
    if (option_to_env(section, option))
        return true;

    if (!has_section(section)) {
        return false;
    }

    std::string s = section;
    stolower(s);
    std::string o = option;
    stolower(o);

    std::lock_guard<std::mutex> lk(d_mutex);
    const auto& sec = d_config_map[s];
    return sec.find(o) != sec.end();
}

// get_string needs specialization because it can have spaces.
const std::string prefs::get_string(const std::string& section,
                                    const std::string& option,
                                    const std::string& default_val)
{
    char* env = option_to_env(section, option);
    if (env)
        return std::string(env);

    if (!has_option(section, option)) {
        return default_val;
    }

    std::string s = section;
    stolower(s);

    std::string o = option;
    stolower(o);

    std::lock_guard<std::mutex> lk(d_mutex);
    return d_config_map[s][o];
}

// get_bool() needs specialization because there are multiple text strings for true/false
bool prefs::get_bool(const std::string& section,
                     const std::string& option,
                     bool default_val)
{
    if (!has_option(section, option)) {
        return default_val;
    }
    std::string str = get_string(section, option, "");
    if (str.empty()) {
        return default_val;
    }
    stolower(str);
    if ((str == "true") || (str == "on") || (str == "1"))
        return true;
    if ((str == "false") || (str == "off") || (str == "0"))
        return false;
    return default_val;
}

template <typename T>
T prefs::get_general(const std::string& section,
                     const std::string& option,
                     const T& default_val)
{
    if (!has_option(section, option)) {
        return default_val;
    }

    std::string str = get_string(section, option, "");
    if (str.empty()) {
        return default_val;
    }
    std::stringstream sstr(str);
    T n;
    sstr >> n;
    return n;
}

template <typename T>
void prefs::set_general(const std::string& section,
                        const std::string& option,
                        const T& val)
{
    std::string s = section;
    stolower(s);
    std::string o = option;
    stolower(o);

    std::stringstream sstr;
    sstr << val;

    std::lock_guard<std::mutex> lk(d_mutex);
    d_config_map[s][o] = sstr.str();
}

void prefs::set_bool(const std::string& section, const std::string& option, bool val)
{
    return set_general(section, option, val);
}

long prefs::get_long(const std::string& section,
                     const std::string& option,
                     long default_val)
{
    return get_general(section, option, default_val);
}

void prefs::set_long(const std::string& section, const std::string& option, long val)
{
    return set_general(section, option, val);
}

double prefs::get_double(const std::string& section,
                         const std::string& option,
                         double default_val)
{
    return get_general(section, option, default_val);
}

void prefs::set_double(const std::string& section, const std::string& option, double val)
{
    return set_general(section, option, val);
}

void prefs::set_string(const std::string& section,
                       const std::string& option,
                       const std::string& val)
{
    return set_general(section, option, val);
}


} /* namespace gr */
