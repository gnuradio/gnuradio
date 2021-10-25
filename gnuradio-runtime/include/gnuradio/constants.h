/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_CONSTANTS_H
#define INCLUDED_GR_CONSTANTS_H

#include <gnuradio/api.h>
#include <string>

namespace gr {

/*!
 * \brief return SYSCONFDIR. Typically ${CMAKE_INSTALL_PREFIX}/etc or /etc
 */
GR_RUNTIME_API const std::string prefix();

/*!
 * \brief return SYSCONFDIR. Typically ${CMAKE_INSTALL_PREFIX}/etc or /etc
 */
GR_RUNTIME_API const std::string sysconfdir();

/*!
 * \brief return preferences file directory. Typically ${SYSCONFDIR}/etc/conf.d
 */
GR_RUNTIME_API const std::string prefsdir();

/*!
 * \brief return date/time of build, as set when 'cmake' is run
 */
GR_RUNTIME_API const std::string build_date();

/*!
 * \brief return version string defined by cmake (GrVersion.cmake)
 */
GR_RUNTIME_API const std::string version();

/*!
 * \brief return just the major version defined by cmake
 */
GR_RUNTIME_API const std::string major_version();

/*!
 * \brief return just the api version defined by cmake
 */
GR_RUNTIME_API const std::string api_version();

/*!
 * \brief returnjust the minor version defined by cmake
 */
GR_RUNTIME_API const std::string minor_version();

/*!
 * \brief return C compiler used to build this version of GNU Radio
 */
GR_RUNTIME_API const std::string c_compiler();

/*!
 * \brief return C++ compiler used to build this version of GNU Radio
 */
GR_RUNTIME_API const std::string cxx_compiler();

/*!
 * \brief return C and C++ compiler flags used to build this version of GNU Radio
 */
GR_RUNTIME_API const std::string compiler_flags();

/*!
 * \brief return build-time enabled components
 */
GR_RUNTIME_API const std::string build_time_enabled_components();

/*!
 * \brief return the pybind11 version used to build this version of GNU Radio
 */
GR_RUNTIME_API const std::string pybind_version();


} /* namespace gr */

#endif /* INCLUDED_GR_CONSTANTS_H */
