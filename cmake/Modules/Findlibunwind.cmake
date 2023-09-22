# - Try to find libunwind
# Once done, this will define
#
#  libunwind_FOUND - system has libunwind
#  libunwind_INCLUDE_DIRS - the libunwind include directories
#  libunwind_LIBRARIES - link these to use libunwind
#  libunwind::libunwind target

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(libunwind_PKGCONF libunwind)

# Include dir
find_path(
    libunwind_INCLUDE_DIR
    NAMES libunwind.h
    HINTS ${libunwind_PKGCONF_INCLUDE_DIRS})

# Finally the library itself
find_library(
    libunwind_LIBRARY
    NAMES unwind
    HINTS ${libunwind_PKGCONF_LIBRARY_DIRS})

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(libunwind_PROCESS_INCLUDES libunwind_INCLUDE_DIR)
set(libunwind_PROCESS_LIBS libunwind_LIBRARY)
libfind_process(libunwind)

add_library(libunwind::libunwind INTERFACE IMPORTED)
# Only add properties if libunwind was found, so target can be used even
# when libunwind was not found
if(libunwind_FOUND)
    set_target_properties(
        libunwind::libunwind
        PROPERTIES INTERFACE_COMPILE_DEFINITIONS HAVE_LIBUNWIND
                   INTERFACE_INCLUDE_DIRECTORIES "${libunwind_INCLUDE_DIRS}"
                   INTERFACE_LINK_LIBRARIES "${libunwind_LIBRARIES}")
endif()
