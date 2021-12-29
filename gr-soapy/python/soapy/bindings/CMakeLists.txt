# Copyright 2020-2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

# For setting string conversions
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../lib)

list(
    APPEND
    soapy_python_files
    block_python.cc
    soapy_common.cc
    soapy_types_python.cc
    source_python.cc
    sink_python.cc
    python_bindings.cc)

gr_pybind_make_check_hash(soapy ../../.. gr::soapy "${soapy_python_files}")

target_compile_features(soapy_python PRIVATE ${GR_CXX_VERSION_FEATURE})

install(
    TARGETS soapy_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/soapy
    COMPONENT pythonapi)
