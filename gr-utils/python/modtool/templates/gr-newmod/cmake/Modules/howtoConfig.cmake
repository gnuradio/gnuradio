INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_HOWTO howto)

FIND_PATH(
    HOWTO_INCLUDE_DIRS
    NAMES howto/api.h
    HINTS $ENV{HOWTO_DIR}/include
        ${PC_HOWTO_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    HOWTO_LIBRARIES
    NAMES gnuradio-howto
    HINTS $ENV{HOWTO_DIR}/lib
        ${PC_HOWTO_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/howtoTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HOWTO DEFAULT_MSG HOWTO_LIBRARIES HOWTO_INCLUDE_DIRS)
MARK_AS_ADVANCED(HOWTO_LIBRARIES HOWTO_INCLUDE_DIRS)
