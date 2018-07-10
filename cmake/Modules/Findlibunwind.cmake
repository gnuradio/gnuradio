# - Try to find libunwind
# Once done, this will define
#
#  libunwind_FOUND - system has libunwind
#  libunwind_INCLUDE_DIRS - the libunwind include directories
#  libunwind_LIBRARIES - link these to use libunwind

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(libunwind_PKGCONF libunwind)

# Include dir
find_path(libunwind_INCLUDE_DIR
  NAMES libunwind.h
  PATHS ${libunwind_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(libunwind_LIBRARY
  NAMES unwind
  PATHS ${libunwind_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(libunwind_PROCESS_INCLUDES ${libunwind_INCLUDE_DIR})
set(libunwind_PROCESS_LIBS libunwind_LIBRARY)
libfind_process(libunwind)
