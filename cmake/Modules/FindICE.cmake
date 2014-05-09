# Override the search path for ICE; useful for manually installed libs.
#    set(ICE_MANUAL_INSTALL_PATH /opt/Ice-3.4.2/)

message(STATUS "Checking for Ice-3.4")

FIND_PATH(
  ICE_CONFIG_INCLUDE_DIR
  NAMES IceUtil/Config.h
  HINTS ${ICE_MANUAL_INSTALL_PATH}/include/ ${CMAKE_INSTALL_PREFIX}/include/
)

if(ICE_CONFIG_INCLUDE_DIR)
  file(STRINGS "${ICE_CONFIG_INCLUDE_DIR}/IceUtil/Config.h"
    ICE_STRING_VERSION REGEX "ICE_STRING_VERSION")
  string(REGEX MATCH "3.4" ICE_FOUND ${ICE_STRING_VERSION})
  if(ICE_FOUND)
    set(ICE_FOUND TRUE)
  endif(ICE_FOUND)
endif(ICE_CONFIG_INCLUDE_DIR)

if(NOT ICE_FOUND)
  message(STATUS "  package 'Ice-3.4' not found")

else(NOT ICE_FOUND)
  message(STATUS "  ICE 3.4 Include directory found: ${ICE_CONFIG_INCLUDE_DIR}")

  # Prepare the path hint for the libraries based on the include
  # directory found.
  string(REGEX REPLACE "/include" "" ICE_PATH ${ICE_CONFIG_INCLUDE_DIR})

  FIND_PATH(
    ICE_INCLUDE_DIR
    NAMES IceUtil/IceUtil.h Ice/Ice.h
    NO_DEFAULT_PATH
    HINTS ${CMAKE_INSTALL_PREFIX}/include
    HINTS ${ICE_PATH}/include
    )

  set(ICE_LIBRARY )

  if(APPLE)
    set(ICE_LIB_PREFIX "Zeroc")
  else()
    set(ICE_LIB_PREFIX "")
  endif(APPLE)

  FIND_LIBRARY(
    ICE_ICE ${ICE_LIB_PREFIX}Ice
    HINTS ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64
    HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
    )
  FIND_LIBRARY(
    ICE_ICESTORM ${ICE_LIB_PREFIX}IceStorm
    HINTS ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64
    HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
    )
  FIND_LIBRARY(
    ICE_ICEGRID ${ICE_LIB_PREFIX}IceGrid
    HINTS ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64
    HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
    )
  FIND_LIBRARY(
    ICE_ICEUTIL ${ICE_LIB_PREFIX}IceUtil
    HINTS ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64
    HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
    )
  FIND_LIBRARY(
    ICE_GLACIER2 Glacier2
    HINTS ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64
    HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
    )

  FIND_LIBRARY(
    ICE_PTHREAD NAMES pthread pthread-2.13
    HINTS ${CMAKE_INSTALL_PREFIX}/lib64/ ${CMAKE_INSTALL_PREFIX}/lib/
    HINTS ${PC_ICE_LIBDIR} ${PC_ICE_LIBRARY_DIRS} /lib/i386-linux-gnu /lib/x86_64-linux-gnu /usr/lib /lib /lib64
    ENV LD_LIBRARY_PATH
    )

  set(ICE_FOUND FALSE)

  if(ICE_ICE AND ICE_ICEUTIL)
    list(APPEND ICE_LIBRARY
      ${ICE_ICE}
      ${ICE_ICEUTIL}
      )

    message(STATUS "  libIce: ${ICE_ICE}")
    message(STATUS "  libIceUtil: ${ICE_ICEUTIL}")

    FIND_PROGRAM(ICE_SLICE2CPP slice2cpp
      HINTS ${CMAKE_INSTALL_PREFIX}/bin ${ICE_MANUAL_INSTALL_PATH}/bin/)
    FIND_PROGRAM(ICE_SLICE2PY slice2py
      HINTS ${CMAKE_INSTALL_PREFIX}/bin ${ICE_MANUAL_INSTALL_PATH}/bin/)

    # Check that the ICE Python package is installed
    include(GrPython)
    GR_PYTHON_CHECK_MODULE("Ice >= 3.4" Ice "Ice.stringVersion() >= '3.4.0'" PYTHON_ICE_FOUND)
    if(PYTHON_ICE_FOUND)
      set(ICE_FOUND TRUE)
    endif(PYTHON_ICE_FOUND)

    if(ICE_FOUND)
      message(STATUS "Ice-3.4 found")

      if(CMAKE_COMPILER_IS_GNUCXX)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
          OUTPUT_VARIABLE gcc_version)
        string(REGEX MATCH "[0-9].[0-9].[0-9]" gcc_version_num ${gcc_version})
        if(NOT ${gcc_version_num} VERSION_LESS "4.7")
          message(STATUS "-- Found GCC version: ${gcc_version_num}")
          message(STATUS "-- GCC incompatible with Ice 3.4, disabling ControlPort")
          set(ICE_FOUND FALSE)
          return() # exit now
        endif(NOT ${gcc_version_num} VERSION_LESS "4.7")
      endif(CMAKE_COMPILER_IS_GNUCXX)

      set(ICE_LIBRARIES ${ICE_LIBRARY})
      set(ICE_INCLUDE_DIRS ${ICE_INCLUDE_DIR})

      include(FindPackageHandleStandardArgs)
      find_package_handle_standard_args(ICE DEFAULT_MSG ICE_LIBRARY ICE_INCLUDE_DIR)
      mark_as_advanced(ICE_INCLUDE_DIR ICE_LIBRARY)
    endif(ICE_FOUND)
  endif(ICE_ICE AND ICE_ICEUTIL)

endif(NOT ICE_FOUND)
