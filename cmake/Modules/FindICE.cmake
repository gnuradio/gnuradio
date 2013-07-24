# Override the search path for ICE; useful for manually installed libs.
#    set(ICE_MANUAL_INSTALL_PATH /opt/Ice-3.4.2/)

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(PC_ICE Ice-3.4 QUIET)

if(NOT ICE_FOUND)
  # Maybe we don't have a .pc file for Ice. Check for Config.h. If
  # that exists, get the version string and parse it for the proper
  # version.
  FIND_PATH(
    ICE_CONFIG_INCLUDE_DIR 
    NAMES IceUtil/Config.h
    HINTS ${CMAKE_INSTALL_PREFIX}/${HEADER_DIR} ${ICE_MANUAL_INSTALL_PATH}/include/
    )
  if(ICE_CONFIG_INCLUDE_DIR)
    file(STRINGS "${ICE_CONFIG_INCLUDE_DIR}/IceUtil/Config.h"
      ICE_STRING_VERSION REGEX "ICE_STRING_VERSION")
    string(REGEX MATCH "3.4" ICE_FOUND ${ICE_STRING_VERSION})
    if(ICE_FOUND)
      set(ICE_FOUND TRUE)
    endif(ICE_FOUND)
  endif(ICE_CONFIG_INCLUDE_DIR)
endif(NOT ICE_FOUND)

if(NOT ICE_FOUND)
  message(STATUS "  package 'Ice-3.4' not found")
endif(NOT ICE_FOUND)


# Recheck if we found the right version of ICE and proceed if true.
if(ICE_FOUND)

# Prepare the path hint for the libraries based on the include
# directory found.
string(REGEX REPLACE "/include" "" ICE_PATH ${ICE_CONFIG_INCLUDE_DIR})

FIND_PATH(
  ICE_INCLUDE_DIR 
  NAMES IceUtil/IceUtil.h Ice/Ice.h
  NO_DEFAULT_PATH
  HINTS ${ICE_PATH}/include
)

set(ICE_LIBRARY )

FIND_LIBRARY(
  ICE_ICESTORM IceStorm 
  NO_DEFAULT_PATH
  HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
)

FIND_LIBRARY(
  ICE_ICESTORM IceStorm
  NO_DEFAULT_PATH
  HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
)
FIND_LIBRARY(
  ICE_ICE Ice
  NO_DEFAULT_PATH
  HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
)
FIND_LIBRARY(
  ICE_ICEGRID IceGrid
  NO_DEFAULT_PATH
  HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
)
FIND_LIBRARY(
  ICE_ICEUTIL IceUtil
  NO_DEFAULT_PATH
  HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
)
FIND_LIBRARY(
  ICE_GLACIER2 Glacier2
  NO_DEFAULT_PATH
  HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
)

if(APPLE)
  FIND_LIBRARY(
    ICE_ZEROCICE ZeroCIce
    NO_DEFAULT_PATH
    HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
    )
endif(APPLE)

FIND_LIBRARY(
  ICE_PTHREAD NAMES pthread pthread-2.13
  HINTS ${CMAKE_INSTALL_PREFIX}/lib64/ ${CMAKE_INSTALL_PREFIX}/lib/
  HINTS ${PC_ICE_LIBDIR} ${PC_ICE_LIBRARY_DIRS} /lib/i386-linux-gnu /lib/x86_64-linux-gnu /usr/lib /lib /lib64
  ENV LD_LIBRARY_PATH
)

set(ICE_FOUND FALSE)

if(ICE_ICE OR ICE_ZEROCICE)
  if(ICE_ICEUTIL)
    list(APPEND ICE_LIBRARY
      ${ICE_ICE}
      ${ICE_ZEROCICE}
      ${ICE_ICEUTIL}
      )

    FIND_PROGRAM(ICE_SLICE2CPP slice2cpp
      HINTS ${CMAKE_INSTALL_PREFIX}/bin ${ICE_MANUAL_INSTALL_PATH}/bin/)
    FIND_PROGRAM(ICE_SLICE2PY slice2py
      HINTS ${CMAKE_INSTALL_PREFIX}/bin ${ICE_MANUAL_INSTALL_PATH}/bin/)

    # Check that the ICE Python package is installed
    GR_PYTHON_CHECK_MODULE("Ice >= 3.4" Ice "Ice.stringVersion() >= '3.4.0'" PYTHON_ICE_FOUND)
    if(PYTHON_ICE_FOUND)
      set(ICE_FOUND TRUE)
    endif(PYTHON_ICE_FOUND)

    if(ICE_FOUND)
      message(STATUS "Ice-3.4 found")

      set(ICE_LIBRARIES ${ICE_LIBRARY})
      set(ICE_INCLUDE_DIRS ${ICE_INCLUDE_DIR})

      include(FindPackageHandleStandardArgs)
      find_package_handle_standard_args(ICE DEFAULT_MSG ICE_LIBRARY ICE_INCLUDE_DIR)
      mark_as_advanced(ICE_INCLUDE_DIR ICE_LIBRARY)
    endif(ICE_FOUND)
  endif(ICE_ICEUTIL)
endif(ICE_ICE OR ICE_ZEROCICE)

endif(ICE_FOUND)
