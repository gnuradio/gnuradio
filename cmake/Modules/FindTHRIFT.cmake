find_package(PkgConfig)
pkg_check_modules(PC_THRIFT thrift)

set(THRIFT_REQ_VERSION ${THRIFT_FIND_VERSION})
message(STATUS "thrift looking for version ${THRIFT_REQ_VERSION}")

# If pkg-config found Thrift and it doesn't meet our version
# requirement, warn and exit -- does not cause an error; just doesn't
# enable Thrift.
if(PC_THRIFT_FOUND AND PC_THRIFT_VERSION VERSION_LESS ${THRIFT_REQ_VERSION})
    message(
        STATUS
            "Could not find appropriate version of Thrift: ${PC_THRIFT_VERSION} < ${THRIFT_REQ_VERSION}"
    )
    return()
endif(PC_THRIFT_FOUND AND PC_THRIFT_VERSION VERSION_LESS ${THRIFT_REQ_VERSION})

# Else, look for it ourselves
find_path(
    THRIFT_INCLUDE_DIRS
    NAMES thrift/Thrift.h
    HINTS ${PC_THRIFT_INCLUDE_DIRS} ${CMAKE_INSTALL_PREFIX}/include
    PATHS /usr/local/include /usr/include)

find_library(
    THRIFT_LIBRARIES
    NAMES thrift thriftmd thriftmt
    HINTS ${PC_THRIFT_LIBDIR} ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS ${THRIFT_INCLUDE_DIRS}/../lib /usr/local/lib /usr/lib)

# Get the thrift binary to build our files during cmake
if(CMAKE_CROSSCOMPILING)
    find_program(THRIFT_BIN thrift NO_CMAKE_FIND_ROOT_PATH)
else(CMAKE_CROSSCOMPILING)
    find_program(THRIFT_BIN thrift)
endif(CMAKE_CROSSCOMPILING)

# Use binary to get version string and test against THRIFT_REQ_VERSION
execute_process(
    COMMAND ${THRIFT_BIN} --version
    OUTPUT_VARIABLE THRIFT_VERSION
    ERROR_VARIABLE THRIFT_VERSION_ERROR)

if(NOT THRIFT_BIN)
    message(STATUS "Binary 'thrift' not found.")
    return()
endif(NOT THRIFT_BIN)

string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" THRIFT_VERSION "${THRIFT_VERSION}")

if(THRIFT_VERSION VERSION_LESS THRIFT_REQ_VERSION)
    message(
        STATUS
            "Could not find appropriate version of Thrift: ${THRIFT_VERSION} < ${THRIFT_REQ_VERSION}"
    )
    return()
endif(THRIFT_VERSION VERSION_LESS THRIFT_REQ_VERSION)

# Check that Thrift for Python is available
if(CMAKE_CROSSCOMPILING)
    set(PYTHON_THRIFT_FOUND TRUE)
else(CMAKE_CROSSCOMPILING)
    gr_python_check_module("Thrift" thrift "1" PYTHON_THRIFT_FOUND)
endif(CMAKE_CROSSCOMPILING)

# Set to found if we've made it this far
if(THRIFT_INCLUDE_DIRS
   AND THRIFT_LIBRARIES
   AND PYTHON_THRIFT_FOUND)
    set(THRIFT_FOUND TRUE)

    find_file(THRIFT_HAS_VERSION_H thrift/version.h PATH ${THRIFT_INCLUDE_DIRS}
              NO_DEFAULT_PATH)
    if(THRIFT_HAS_VERSION_H-FOUND)
        set(THRIFT_HAS_VERSION_H TRUE)
    endif()

    find_file(THRIFT_HAS_THREADFACTORY_H thrift/concurrency/ThreadFactory.h PATH
              ${THRIFT_INCLUDE_DIRS} NO_DEFAULT_PATH)
    if(THRIFT_HAS_THREADFACTORY_H-FOUND)
        set(THRIFT_HAS_THREADFACTORY_H TRUE)
    endif()
endif(
    THRIFT_INCLUDE_DIRS
    AND THRIFT_LIBRARIES
    AND PYTHON_THRIFT_FOUND)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    THRIFT
    DEFAULT_MSG
    THRIFT_LIBRARIES
    THRIFT_INCLUDE_DIRS
    THRIFT_BIN
    PYTHON_THRIFT_FOUND
    THRIFT_FOUND)
mark_as_advanced(THRIFT_LIBRARIES THRIFT_INCLUDE_DIRS THRIFT_BIN PYTHON_THRIFT_FOUND
                 THRIFT_FOUND)

if(THRIFT_FOUND AND NOT TARGET Thrift::thrift)
    add_library(Thrift::thrift INTERFACE IMPORTED)
    set_target_properties(
        Thrift::thrift PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${THRIFT_INCLUDE_DIRS}"
                                  INTERFACE_LINK_LIBRARIES "${THRIFT_LIBRARIES}")
endif()
