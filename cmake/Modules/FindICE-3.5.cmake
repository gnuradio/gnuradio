# Override the search path for ICE; useful for manually installed libs.
#    set(ICE_MANUAL_INSTALL_PATH /opt/Ice-3.5.0/)

message(STATUS "Checking for Ice-3.5")

FIND_PATH(
  ICE_CONFIG_INCLUDE_DIR
  NAMES IceUtil/Config.h
  HINTS ${ICE_MANUAL_INSTALL_PATH}/include/ ${CMAKE_INSTALL_PREFIX}/include/
)

if(ICE_CONFIG_INCLUDE_DIR)
  file(STRINGS "${ICE_CONFIG_INCLUDE_DIR}/IceUtil/Config.h"
    ICE_STRING_VERSION REGEX "ICE_STRING_VERSION")
  string(REGEX MATCH "3.5" ICE_FOUND ${ICE_STRING_VERSION})
  if(ICE_FOUND)
    set(ICE_FOUND TRUE)
  endif(ICE_FOUND)
endif(ICE_CONFIG_INCLUDE_DIR)

if(NOT ICE_FOUND)
  message(STATUS "  package 'Ice-3.5' not found")

else(NOT ICE_FOUND)
  message(STATUS "  ICE 3.5 Include directory found: ${ICE_CONFIG_INCLUDE_DIR}")

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

  if(APPLE)
    set(ICE_LIB_PREFIX "Zeroc")
  else()
    set(ICE_LIB_PREFIX "")
  endif(APPLE)

  FIND_LIBRARY(
    ICE_ICE ${ICE_LIB_PREFIX}Ice
    HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
    )
  FIND_LIBRARY(
    ICE_ICESTORM ${ICE_LIB_PREFIX}IceStorm
    HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
    )
  FIND_LIBRARY(
    ICE_ICEGRID ${ICE_LIB_PREFIX}IceGrid
    HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
    )
  FIND_LIBRARY(
    ICE_ICEUTIL ${ICE_LIB_PREFIX}IceUtil
    HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
    )
  FIND_LIBRARY(
    ICE_GLACIER2 Glacier2
    HINTS ${ICE_PATH}/lib ${ICE_PATH}/lib64
    )

  FIND_LIBRARY(
    ICE_PTHREAD NAMES pthread pthread-2.13
    HINTS ${CMAKE_INSTALL_PREFIX}/lib64/ ${CMAKE_INSTALL_PREFIX}/lib/
    HINTS ${PC_ICE_LIBDIR} ${PC_ICE_LIBRARY_DIRS} /lib/i386-linux-gnu /lib/x86_64-linux-gnu /usr/lib /lib /lib64
    ENV LD_LIBRARY_PATH
    )

  set(ICE_FOUND FALSE)

  message(STATUS "  libIce: ${ICE_ICE}")
  message(STATUS "  libIceUtil: ${ICE_ICEUTIL}")

  # Make sure we have these two libs in particular
  # - me might expand this list requirement in the future
  if(ICE_ICE AND ICE_ICEUTIL)
    list(APPEND ICE_LIBRARY
      ${ICE_ICE}
      ${ICE_ICEUTIL}
      )

    if(CMAKE_CROSSCOMPILING)
      # When cross-compiling, we set up the environment/toolchain to put
      # the right slice2cpp/py in the path. We just need to grab that here.
      FIND_PROGRAM(ICE_SLICE2CPP slice2cpp)
      FIND_PROGRAM(ICE_SLICE2PY slice2py)

      # We also don't need to look for Python in this case, so just
      # force this to TRUE here.
      set(ICE_FOUND TRUE)

    else(CMAKE_CROSSCOMPILING)

      FIND_PROGRAM(ICE_SLICE2CPP slice2cpp
        HINTS ${CMAKE_INSTALL_PREFIX}/bin ${ICE_MANUAL_INSTALL_PATH}/bin/)
      FIND_PROGRAM(ICE_SLICE2PY slice2py
        HINTS ${CMAKE_INSTALL_PREFIX}/bin ${ICE_MANUAL_INSTALL_PATH}/bin/)

      # Check that the ICE Python package is installed
      include(GrPython)
      GR_PYTHON_CHECK_MODULE("Ice >= 3.5" Ice "Ice.stringVersion() >= '3.5.0'" PYTHON_ICE_FOUND)
      if(PYTHON_ICE_FOUND)
        set(ICE_FOUND TRUE)
      endif(PYTHON_ICE_FOUND)
    endif(CMAKE_CROSSCOMPILING)

    message(STATUS "  SLICE2CPP: ${ICE_SLICE2CPP}")
    message(STATUS "  SLICE2PY: ${ICE_SLICE2PY}")

    if(ICE_FOUND)
      message(STATUS "Ice-3.5 found")

      set(ICE_LIBRARIES ${ICE_LIBRARY})
      set(ICE_INCLUDE_DIRS ${ICE_INCLUDE_DIR})

      include(FindPackageHandleStandardArgs)
      find_package_handle_standard_args(ICE DEFAULT_MSG ICE_LIBRARY ICE_INCLUDE_DIR)
      mark_as_advanced(ICE_INCLUDE_DIR ICE_LIBRARY)
    endif(ICE_FOUND)
  endif(ICE_ICE AND ICE_ICEUTIL)

endif(NOT ICE_FOUND)
