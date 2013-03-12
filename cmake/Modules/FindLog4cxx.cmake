#   CMake module to find LOG4CXX library

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_LOG4CXX liblog4cxx)

FIND_PATH(
    LOG4CXX_INCLUDE_DIRS
    NAMES log4cxx/log4cxx.h
    HINTS $ENV{LOG4CXX_DIR}/include
        ${PC_LOG4CXX_INCLUDE_DIRS}
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    LOG4CXX_LIBRARIES
    NAMES log4cxx
    HINTS $ENV{LOG4CXX_DIR}/lib
        ${PC_LOG4CXX_LIBRARIES}
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LOG4CXX DEFAULT_MSG LOG4CXX_LIBRARIES LOG4CXX_INCLUDE_DIRS)
MARK_AS_ADVANCED(LOG4CXX_LIBRARIES LOG4CXX_INCLUDE_DIRS)
