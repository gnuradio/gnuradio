find_package(PkgConfig)
pkg_check_modules(PC_GMP "gmp")

set(GMP_DEFINITIONS ${PC_GMP_CFLAGS_OTHER})

find_path(
    GMP_INCLUDE_DIR
    NAMES gmpxx.h
    HINTS ${PC_GMP_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include /usr/local/include /usr/include)
set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})
set(GMP_PC_ADD_CFLAGS "-I${GMP_INCLUDE_DIR}")

find_library(
    GMPXX_LIBRARY
    NAMES gmpxx
    HINTS ${PC_GMP_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64 /usr/local/lib
          /usr/local/lib64 /usr/lib /usr/lib64)

find_library(
    GMP_LIBRARY
    NAMES gmp
    HINTS ${PC_GMP_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64 /usr/local/lib
          /usr/local/lib64 /usr/lib /usr/lib64)
set(GMP_LIBRARIES ${GMPXX_LIBRARY} ${GMP_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG GMPXX_LIBRARY GMP_LIBRARY
                                  GMP_INCLUDE_DIR)
mark_as_advanced(GMPXX_LIBRARY GMP_LIBRARY GMP_INCLUDE_DIR)
