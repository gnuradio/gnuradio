INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(UHD uhd)
IF(NOT UHD_FOUND)

FIND_PATH(
    UHD_INCLUDE_DIRS
    NAMES uhd/config.hpp
    HINTS $ENV{UHD_DIR}/include
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    UHD_LIBRARIES
    NAMES uhd
    HINTS $ENV{UHD_DIR}/lib
    PATHS /usr/local/lib
          /usr/lib
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(UHD DEFAULT_MSG UHD_LIBRARIES UHD_INCLUDE_DIRS)

ENDIF(NOT UHD_FOUND)
