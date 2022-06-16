# Copyright 2022 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# TODO: in cmake 3.10+ use include_guard()
if(__INCLUDED_ASIO)
  return()
endif()
set(__INCLUDED_ASIO TRUE)



# asio is header-only
find_path(ASIO_INCLUDE_DIR
  NAMES
    asio.hpp
  PATHS
    /usr/include
    /usr/local/include
  PATH_SUFFIXES
    asio
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Asio
  REQUIRED_VARS
    ASIO_INCLUDE_DIR
)


if(Asio_FOUND)
  add_library(asio::asio INTERFACE IMPORTED)
  set_target_properties(asio::asio PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ASIO_INCLUDE_DIR}"
  )
endif()

