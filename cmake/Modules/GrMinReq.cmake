# Copyright 2011-2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

if(DEFINED __INCLUDED_GR_MIN_REQ_CMAKE)
    return()
endif()
set(__INCLUDED_GR_MIN_REQ_CMAKE TRUE)
# Minimum dependency versions for central dependencies:
set(GR_BOOST_MIN_VERSION "1.69") ## Version in CentOS 8 (EPEL)
# !!!WHEN CHANGING MINIMUM CMAKE VERSION!!!
# ADJUST MAIN CMakeLists.txt OF PROJECT, gr-newmod AND THE GRC C++ TEMPLATE
set(GR_CMAKE_MIN_VERSION "3.16.3") ## Version in Ubuntu 20.04LTS 
set(GR_MAKO_MIN_VERSION "1.1.0") ## Version in Ubuntu 20.04LTS
set(GR_PYTHON_MIN_VERSION "3.7.2") ## 3.7 is already EOL, but was 3.6.5
set(GR_PYGCCXML_MIN_VERSION "2.0.0") ## Version to support c++17 (in pip)
set(GR_NUMPY_MIN_VERSION "1.17.4") ## Version in Ubuntu 20.04LTS
set(GCC_MIN_VERSION "9.3.0") ## Version in Ubuntu 20.04LTS
set(CLANG_MIN_VERSION "11.0.0") ## Version in Ubuntu 20.04LTS
set(APPLECLANG_MIN_VERSION "1100") ## same as clang 11.0.0, in Xcode11
set(MSVC_MIN_VERSION "1916") ## VS2017 (default in conda-forge)
set(VOLK_MIN_VERSION "2.4.1") ## first version with CPU features
set(PYBIND11_MIN_VERSION "2.4"
)# pybind11 sets versions like 2.4.dev4, which compares < 2.4.3
set(GR_THRIFT_MIN_VERSION "0.13")
