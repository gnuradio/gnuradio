########################################################################
# Find the library for the USRP Hardware Driver
########################################################################

# make this file non-reentrant
if(__INCLUDED_FIND_UHD_CMAKE)
    return()
endif()
set(__INCLUDED_FIND_UHD_CMAKE TRUE)

# First check to see if UHD installed its own CMake files

# save the current MODULE path
set(SAVED_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})

# clear the current MODULE path; uses system paths only
unset(CMAKE_MODULE_PATH)

# try to find UHD via the provided parameters,
# handle REQUIRED internally later
unset(UHD_FOUND)

# was the version specified?
unset(LOCAL_UHD_FIND_VERSION)
if(UHD_FIND_VERSION)
  set(LOCAL_UHD_FIND_VERSION ${UHD_FIND_VERSION})
endif(UHD_FIND_VERSION)

# was EXACT specified?
unset(LOCAL_UHD_FIND_VERSION_EXACT)
if(UHD_FIND_VERSION_EXACT)
  set(LOCAL_UHD_FIND_VERSION_EXACT "EXACT")
endif(UHD_FIND_VERSION_EXACT)

# try to find UHDConfig using the desired parameters;
# UHDConfigVersion will catch a pass-through version bug ...
find_package(
  UHD ${LOCAL_UHD_FIND_VERSION}
  ${LOCAL_UHD_FIND_VERSION_EXACT} QUIET
)

# restore CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${SAVED_CMAKE_MODULE_PATH})

# check if UHD was found above
if(NOT UHD_FOUND)

  # Not found; try the "old" method (not as robust)

  include(FindPkgConfig)
  pkg_check_modules(PC_UHD uhd)

  find_path(
    UHD_INCLUDE_DIRS
    NAMES uhd/config.hpp
    HINTS $ENV{UHD_DIR}/include
          ${PC_UHD_INCLUDEDIR}
    PATHS /usr/local/include
          /usr/include
  )

  find_library(
    UHD_LIBRARIES
    NAMES uhd
    HINTS $ENV{UHD_DIR}/lib
          ${PC_UHD_LIBDIR}
    PATHS /usr/local/lib
          /usr/lib
  )
endif(NOT UHD_FOUND)

if(UHD_LIBRARIES AND UHD_INCLUDE_DIRS)

  # if UHDConfig set UHD_FOUND==TRUE, then these have already been
  # done, but done quietly.  It does not hurt to redo them here.

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(UHD DEFAULT_MSG UHD_LIBRARIES UHD_INCLUDE_DIRS)
  mark_as_advanced(UHD_LIBRARIES UHD_INCLUDE_DIRS)

elseif(UHD_FIND_REQUIRED)

  message(FATAL_ERROR "UHD is required, but was not found.")

endif()
