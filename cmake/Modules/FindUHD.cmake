########################################################################
# Find the library for the USRP Hardware Driver
########################################################################

# First check to see if UHD installed its own version
# of FindUHD, and if so use it.

unset(UHD_FOUND)

# obvious locations for FindUHD.cmake to already be installed are:
#   ${prefix}/share/cmake-X.Y/Modules  (${CMAKE_ROOT})
#   ${prefix}/share/cmake/Modules
#   ${prefix}/lib/cmake
#   ${prefix}/lib64/cmake

get_filename_component(CMAKE_ROOT_PARENT "${CMAKE_ROOT}" PATH)

find_path(
  LOCAL_FINDUHD_DIR
  NAMES FindUHD.cmake
  PATH_SUFFIXES cmake cmake/Modules Modules
  HINTS ${CMAKE_MODULES_DIR}
  PATHS ${CMAKE_ROOT}
    ${CMAKE_ROOT_PARENT}
    ${CMAKE_INSTALL_PREFIX}/lib
    ${CMAKE_INSTALL_PREFIX}/lib64
)

if(LOCAL_FINDUHD_DIR)

  # save the current MODULE path
  set(SAVED_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})

  # remove the current directory from the MODULE path
  list(REMOVE_ITEM CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

  # prepend the found directory to the MODULE path
  list(INSERT CMAKE_MODULE_PATH 0 ${LOCAL_FINDUHD_DIR})

  # "QUIET" works on CMake 2.8+ only
  unset(LOCAL_UHD_QUIET)
  if(NOT ${CMAKE_VERSION} VERSION_LESS "2.8.0")
      set(LOCAL_UHD_QUIET "QUIET")
  endif()

  # set REQUIRED, as directed
  unset(LOCAL_UHD_REQUIRED)
  if(UHD_FIND_REQUIRED)
    set(LOCAL_UHD_REQUIRED "REQUIRED")
  endif(UHD_FIND_REQUIRED)

  # set VERSION to be checking, as directed
  unset(LOCAL_UHD_VERSION)
  if(UHD_FIND_VERSION)
    set(LOCAL_UHD_VERSION "${UHD_FIND_VERSION}")
  endif(UHD_FIND_VERSION)

  # set EXACT, as directed, but only if VERSION was specified
  unset(LOCAL_UHD_VERSION_EXACT)
  if(UHD_FIND_VERSION_EXACT)
    if(LOCAL_UHD_VERSION)
      set(LOCAL_UHD_VERSION_EXACT "EXACT")
    endif(LOCAL_UHD_VERSION)
  endif(UHD_FIND_VERSION_EXACT)

  # try to find UHD using the already-installed FindUHD, as directed
  find_package(
    UHD ${LOCAL_UHD_VERSION} ${LOCAL_UHD_VERSION_EXACT}
    ${LOCAL_UHD_REQUIRED} ${LOCAL_UHD_QUIET}
  )

  # restore CMAKE_MODULE_PATH

  set(CMAKE_MODULE_PATH ${SAVED_CMAKE_MODULE_PATH})

  # print the standard message iff UHD was found
  if(UHD_FOUND)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(UHD DEFAULT_MSG UHD_LIBRARIES UHD_INCLUDE_DIRS)
  endif(UHD_FOUND)

endif(LOCAL_FINDUHD_DIR)

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

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(UHD DEFAULT_MSG UHD_LIBRARIES UHD_INCLUDE_DIRS)
  mark_as_advanced(UHD_LIBRARIES UHD_INCLUDE_DIRS)

endif(NOT UHD_FOUND)
