#######################################################################
# Find the library for SWIG
#
# The goal here is to intercept calls to "FIND_PACKAGE(SWIG)" in order
# to do a global version check locally after passing on the "find" to
# SWIG-provided scripts.
########################################################################

# make this file non-reentrant within the current context

if(__INCLUDED_FIND_SWIG_CMAKE)
    return()
endif()
set(__INCLUDED_FIND_SWIG_CMAKE TRUE)

# some status messages

message(STATUS "")
message(STATUS "Checking for module SWIG")

#
# First check to see if SWIG installed its own CMake file, or if the
# one provided by CMake finds SWIG.
#

# save the current MODULE path

set(SAVED_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})

# clear the current MODULE path; uses system paths only

unset(CMAKE_MODULE_PATH)

# try to find SWIG via the provided parameters,
# handle REQUIRED internally later

unset(SWIG_FOUND)

# was the version specified?

unset(LOCAL_SWIG_FIND_VERSION)
if(SWIG_FIND_VERSION)
  set(LOCAL_SWIG_FIND_VERSION ${SWIG_FIND_VERSION})
endif(SWIG_FIND_VERSION)

# was EXACT specified?

unset(LOCAL_SWIG_FIND_VERSION_EXACT)
if(SWIG_FIND_VERSION_EXACT)
  set(LOCAL_SWIG_FIND_VERSION_EXACT "EXACT")
endif(SWIG_FIND_VERSION_EXACT)

# was REQUIRED specified?

unset(LOCAL_SWIG_FIND_REQUIRED)
if(SWIG_FIND_REQUIRED)
  set(LOCAL_SWIG_FIND_REQUIRED "REQUIRED")
endif(SWIG_FIND_REQUIRED)

# try to find SWIG using the provided parameters, quietly;
#
# this call will error out internally (and not quietly) if:
# 1: EXACT is specified and the version found does not match the requested version;
# 2: REQUIRED is set and SWIG was not found;
#
# this call will return SWIG_FOUND == FALSE if REQUIRED is not set, and:
# 1: SWIG was not found;
# 2: The version found is less than the requested version.

find_package(
  SWIG
  ${LOCAL_SWIG_FIND_VERSION}
  ${LOCAL_SWIG_FIND_VERSION_EXACT}
  ${LOCAL_SWIG_FIND_REQUIRED}
  QUIET
)

# restore CMAKE_MODULE_PATH

set(CMAKE_MODULE_PATH ${SAVED_CMAKE_MODULE_PATH})

# specific version checks

set(SWIG_VERSION_CHECK FALSE)
if(SWIG_FOUND)

  # SWIG was found; make sure the version meets GR's needs
  message(STATUS "Found SWIG version ${SWIG_VERSION}.")
  if("${SWIG_VERSION}" VERSION_GREATER "2.0.3")
    if(NOT "${SWIG_VERSION}" VERSION_EQUAL "3.0.3" AND
       NOT "${SWIG_VERSION}" VERSION_EQUAL "3.0.4")
      set(SWIG_VERSION_CHECK TRUE)
    else()
      message(STATUS "SWIG versions 3.0.3 and 3.0.4 fail to work with GNU Radio.")
    endif()
  else()
    message(STATUS "Minimum SWIG version required is 2.0.4 for GNU Radio.")
  endif()

else()

  # SWIG was either not found, or is less than the requested version
  if(SWIG_VERSION)
    # SWIG_VERSION is set, but SWIG_FOUND is false; probably a version mismatch
    message(STATUS "Found SWIG version ${SWIG_VERSION}.")
    message(STATUS "Requested SWIG version is at least ${SWIG_FIND_VERSION}.")
  endif()
endif()

# did the version check fail?

if(NOT SWIG_VERSION_CHECK)

  # yes: clear various variables and set FOUND to FALSE
  message(STATUS "Disabling SWIG because version check failed.")
  unset(SWIG_VERSION CACHE)
  unset(SWIG_DIR CACHE)
  unset(SWIG_EXECUTABLE CACHE)
  set(SWIG_FOUND FALSE CACHE BOOL "Set to TRUE if a compatible version of SWIG is found" FORCE)

endif()

# check to see if SWIG variables were set

if(SWIG_FOUND AND SWIG_DIR AND SWIG_EXECUTABLE)

  # yes: even if set SWIG_FOUND==TRUE, then these have already been
  # done, but done quietly.  It does not hurt to redo them here.

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(SWIG DEFAULT_MSG SWIG_EXECUTABLE SWIG_DIR)
  mark_as_advanced(SWIG_EXECUTABLE SWIG_DIR)

elseif(SWIG_FIND_REQUIRED)

  if(SWIG_FIND_VERSION)
    message(FATAL_ERROR "The found SWIG version (${SWIG_VERSION}) is not compatible with the version required (${SWIG_FIND_VERSION}).")
  else()
    message(FATAL_ERROR "SWIG is required, but was not found.")
  endif()
endif()
