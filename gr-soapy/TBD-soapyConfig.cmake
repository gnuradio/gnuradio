INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_SOAPY soapy)

FIND_PATH(
    SOAPY_INCLUDE_DIRS
    NAMES soapy/api.h
    HINTS $ENV{SOAPY_DIR}/include
        ${PC_SOAPY_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    SOAPY_LIBRARIES
    NAMES gnuradio-soapy
    HINTS $ENV{SOAPY_DIR}/lib
        ${PC_SOAPY_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/soapyTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SOAPY DEFAULT_MSG SOAPY_LIBRARIES SOAPY_INCLUDE_DIRS)
MARK_AS_ADVANCED(SOAPY_LIBRARIES SOAPY_INCLUDE_DIRS)
