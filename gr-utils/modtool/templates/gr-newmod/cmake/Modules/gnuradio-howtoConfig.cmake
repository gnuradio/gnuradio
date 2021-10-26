find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_HOWTO gnuradio-howto)

FIND_PATH(
    GR_HOWTO_INCLUDE_DIRS
    NAMES gnuradio/howto/api.h
    HINTS $ENV{HOWTO_DIR}/include
        ${PC_HOWTO_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_HOWTO_LIBRARIES
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

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-howtoTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_HOWTO DEFAULT_MSG GR_HOWTO_LIBRARIES GR_HOWTO_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_HOWTO_LIBRARIES GR_HOWTO_INCLUDE_DIRS)
