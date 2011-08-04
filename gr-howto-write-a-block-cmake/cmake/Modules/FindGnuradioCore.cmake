INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(GNURADIO_CORE gnuradio-core)
IF(NOT GNURADIO_CORE_FOUND)

FIND_PATH(
    GNURADIO_CORE_INCLUDE_DIRS
    NAMES gr_random.h
    HINTS $ENV{GNURADIO_CORE_DIR}/include/gnuradio
    PATHS /usr/local/include/gnuradio
          /usr/include/gnuradio
)

FIND_LIBRARY(
    GNURADIO_CORE_LIBRARIES
    NAMES gnuradio-core
    HINTS $ENV{GNURADIO_CORE_DIR}/lib
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GNURADIO_CORE DEFAULT_MSG GNURADIO_CORE_LIBRARIES GNURADIO_CORE_INCLUDE_DIRS)

ENDIF(NOT GNURADIO_CORE_FOUND)
