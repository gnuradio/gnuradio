INCLUDE(FindPkgConfig)

FIND_PATH(
    ICONV_INCLUDE_DIR
    NAMES iconv.h
    PATHS /usr/local/include
)

FIND_LIBRARY(
    ICONV_LIBRARY
    NAMES iconv
    PATHS /usr/local/lib
)
