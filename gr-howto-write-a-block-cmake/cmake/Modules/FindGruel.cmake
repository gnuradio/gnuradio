INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(GRUEL gnuradio-core)
IF(NOT GRUEL_FOUND)

FIND_PATH(
    GRUEL_INCLUDE_DIRS
    NAMES gruel/attributes.h
    HINTS $ENV{GRUEL_DIR}/include
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GRUEL_LIBRARIES
    NAMES gruel
    HINTS $ENV{GRUEL_DIR}/lib
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GRUEL DEFAULT_MSG GRUEL_LIBRARIES GRUEL_INCLUDE_DIRS)

ENDIF(NOT GRUEL_FOUND)
