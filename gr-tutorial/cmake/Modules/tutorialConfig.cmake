INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_TUTORIAL tutorial)

FIND_PATH(
    TUTORIAL_INCLUDE_DIRS
    NAMES tutorial/api.h
    HINTS $ENV{TUTORIAL_DIR}/include
        ${PC_TUTORIAL_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    TUTORIAL_LIBRARIES
    NAMES gnuradio-tutorial
    HINTS $ENV{TUTORIAL_DIR}/lib
        ${PC_TUTORIAL_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TUTORIAL DEFAULT_MSG TUTORIAL_LIBRARIES TUTORIAL_INCLUDE_DIRS)
MARK_AS_ADVANCED(TUTORIAL_LIBRARIES TUTORIAL_INCLUDE_DIRS)

