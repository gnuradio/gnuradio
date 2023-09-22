/* -*- c++ -*- */
/*
 * Copyright 2006,2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_PREFS_H
#define INCLUDED_GR_PREFS_H

#include <gnuradio/api.h>
#include <gnuradio/thread/thread.h>
#include <map>
#include <mutex>
#include <string>

namespace gr {

/*!
 * \brief Base class for representing user preferences a la windows INI files.
 * \ingroup misc
 *
 * The real implementation is in Python, and is accessible from C++
 * via the magic of SWIG directors.
 */
class GR_RUNTIME_API prefs
{
public:
    static prefs* singleton();

    /*!
     * \brief Creates an object to read preference files.
     *
     * \details
     *
     * If no file name is given (empty arg list or ""), this opens up
     * the standard GNU Radio configuration files in
     * prefix/etc/gnuradio/conf.d as well as ~/.gnuradio/config.conf.
     *
     * Only access this through the singleton defined here:
     * \code
     * prefs *p = prefs::singleton();
     * \endcode
     */
    prefs();

    /*!
     * If specifying a file name, this opens that specific
     * configuration file of the standard form containing sections and
     * key-value pairs:
     *
     * \code
     * [SectionName]
     * key0 = value0
     * key1 = value1
     * \endcode
     */
    void add_config_file(const std::string& configfile);

    /*!
     * \brief Returns the configuration options as a string.
     */
    std::string to_string();

    /*!
     * \brief Saves the configuration settings to
     * ${HOME}/.gnuradio/config.conf.
     *
     * WARNING: this will overwrite your current config.conf file.
     */
    void save();

    /*!
     * \brief Does \p section exist?
     */
    bool has_section(const std::string& section);

    /*!
     * \brief Does \p option exist?
     */
    bool has_option(const std::string& section, const std::string& option);

    /*!
     * \brief If option exists return associated value; else
     * default_val.
     */
    const std::string get_string(const std::string& section,
                                 const std::string& option,
                                 const std::string& default_val);

    /*!
     * \brief Set or add a string \p option to \p section with value
     * \p val.
     */
    void set_string(const std::string& section,
                    const std::string& option,
                    const std::string& val);

    /*!
     * \brief If option exists and value can be converted to bool,
     * return it; else default_val.
     */
    bool
    get_bool(const std::string& section, const std::string& option, bool default_val);

    /*!
     * \brief Set or add a bool \p option to \p section with value \p val.
     */
    void set_bool(const std::string& section, const std::string& option, bool val);

    /*!
     * \brief If option exists and value can be converted to long,
     * return it; else default_val.
     */
    long
    get_long(const std::string& section, const std::string& option, long default_val);

    /*!
     * \brief Set or add a long \p option to \p section with value \p val.
     */
    void set_long(const std::string& section, const std::string& option, long val);

    /*!
     * \brief If option exists and value can be converted to double,
     * return it; else default_val.
     */
    double
    get_double(const std::string& section, const std::string& option, double default_val);

    /*!
     * \brief Set or add a double \p option to \p section with value \p val.
     */
    void set_double(const std::string& section, const std::string& option, double val);

protected:
    std::vector<std::string> _sys_prefs_filenames();
    void _read_files(const std::vector<std::string>& filenames);
    char* option_to_env(std::string section, std::string option);
    template <typename T>
    T get_general(const std::string& section,
                  const std::string& option,
                  const T& default_val);
    template <typename T>
    void set_general(const std::string& section, const std::string& option, const T& val);

private:
    std::mutex d_mutex;
    std::map<std::string, std::map<std::string, std::string>> d_config_map;
};

} /* namespace gr */

#endif /* INCLUDED_GR_PREFS_H */
