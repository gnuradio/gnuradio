# http://www.cmake.org/pipermail/cmake/2006-October/011446.html
# Modified to use pkg config and use standard var names

#
# Find the CppUnit includes and library
#
# This module defines
# CPPUNIT_INCLUDE_DIR, where to find tiff.h, etc.
# CPPUNIT_LIBRARIES, the libraries to link against to use CppUnit.
# CPPUNIT_FOUND, If false, do not try to use CppUnit.

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_CPPUNIT "cppunit")

FIND_PATH(CPPUNIT_INCLUDE_DIRS
    NAMES cppunit/TestCase.h
    HINTS ${PC_CPPUNIT_INCLUDE_DIR}
    PATHS
    /usr/local/include
    /usr/include
)

FIND_LIBRARY(CPPUNIT_LIBRARIES
    NAMES cppunit
    HINTS ${PC_CPPUNIT_LIBDIR}
    PATHS
    ${CPPUNIT_INCLUDE_DIRS}/../lib
    /usr/local/lib
    /usr/lib
)

LIST(APPEND CPPUNIT_LIBRARIES ${CMAKE_DL_LIBS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CPPUNIT DEFAULT_MSG CPPUNIT_LIBRARIES CPPUNIT_INCLUDE_DIRS)
MARK_AS_ADVANCED(CPPUNIT_LIBRARIES CPPUNIT_INCLUDE_DIRS)
