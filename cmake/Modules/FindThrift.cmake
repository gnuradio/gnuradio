INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_THRIFT thrift)

set(THRIFT_REQ_VERSION "0.9.2")

# If pkg-config found Thrift and it doesn't meet our version
# requirement, warn and exit -- does not cause an error; just doesn't
# enable Thrift.
if(PC_THRIFT_FOUND AND PC_THRIFT_VERSION VERSION_LESS ${THRIFT_REQ_VERSION})
  message(STATUS "Could not find appropriate version of Thrift: ${PC_THRIFT_VERSION} < ${THRIFT_REQ_VERSION}")
  return()
endif(PC_THRIFT_FOUND AND PC_THRIFT_VERSION VERSION_LESS ${THRIFT_REQ_VERSION})


# Else, look for it ourselves

FIND_PATH(THRIFT_INCLUDE_DIRS
  NAMES thrift/Thrift.h
  HINTS ${PC_THRIFT_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
  PATHS
  /usr/local/include
  /usr/include
  )

FIND_LIBRARY(THRIFT_LIBRARIES
  NAMES thrift
  HINTS ${PC_THRIFT_LIBDIR}
  ${CMAKE_INSTALL_PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib64
  PATHS
  ${THRIFT_INCLUDE_DIRS}/../lib
  /usr/local/lib
  /usr/lib
  )

# Get the thrift binary to build our files during cmake
FIND_PROGRAM(THRIFT_BIN thrift)

# Use binary to get version string and test against THRIFT_REQ_VERSION
EXECUTE_PROCESS(
   COMMAND ${THRIFT_BIN} --version
   OUTPUT_VARIABLE THRIFT_VERSION
   ERROR_VARIABLE THRIFT_VERSION_ERROR
   )

if(NOT THRIFT_BIN)
  message(STATUS "Binary 'thrift' not found.")
  return()
endif(NOT THRIFT_BIN)

STRING(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+"
  THRIFT_VERSION "${THRIFT_VERSION}")

if(THRIFT_VERSION VERSION_LESS THRIFT_REQ_VERSION)
  message(STATUS "Could not find appropriate version of Thrift: ${THRIFT_VERSION} < ${THRIFT_REQ_VERSION}")
  return()
endif(THRIFT_VERSION VERSION_LESS THRIFT_REQ_VERSION)


# Check that Thrift for Python is available
include(GrPython)
GR_PYTHON_CHECK_MODULE("Thrift" thrift "1" PYTHON_THRIFT_FOUND)

# Set to found if we've made it this far
if(THRIFT_INCLUDE_DIRS AND THRIFT_LIBRARIES AND PYTHON_THRIFT_FOUND)
  set(THRIFT_FOUND TRUE CACHE BOOL "If Thift has been found")
endif(THRIFT_INCLUDE_DIRS AND THRIFT_LIBRARIES AND PYTHON_THRIFT_FOUND)


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(THRIFT DEFAULT_MSG
  THRIFT_LIBRARIES THRIFT_INCLUDE_DIRS
  THRIFT_BIN PYTHON_THRIFT_FOUND THRIFT_FOUND
)
MARK_AS_ADVANCED(
  THRIFT_LIBRARIES THRIFT_INCLUDE_DIRS
  THRIFT_BIN PYTHON_THRIFT_FOUND THRIFT_FOUND
)
