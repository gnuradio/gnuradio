find_package(PkgConfig)
pkg_check_modules(PC_MPIR "mpir >= 3.0")

set(MPIR_DEFINITIONS ${PC_MPIR_CFLAGS_OTHER})

find_path(
    MPIR_INCLUDE_DIR
    NAMES mpirxx.h
    HINTS ${PC_MPIR_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include /usr/local/include /usr/include)
set(MPIR_INCLUDE_DIRS ${MPIR_INCLUDE_DIR})
set(MPIR_PC_ADD_CFLAGS "-I${MPIR_INCLUDE_DIR}")

find_library(
    MPIRXX_LIBRARY
    # mpirxx is bundled into mpir.lib with MSVC
    NAMES mpirxx mpir.lib
    HINTS ${PC_MPIR_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64 /usr/local/lib
          /usr/local/lib64 /usr/lib /usr/lib64)

find_library(
    MPIR_LIBRARY
    NAMES mpir
    HINTS ${PC_MPIR_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64 /usr/local/lib
          /usr/local/lib64 /usr/lib /usr/lib64)
set(MPIR_LIBRARIES ${MPIRXX_LIBRARY} ${MPIR_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPIR DEFAULT_MSG MPIRXX_LIBRARY MPIR_LIBRARY
                                  MPIR_INCLUDE_DIR)
mark_as_advanced(MPIRXX_LIBRARY MPIR_LIBRARY MPIR_INCLUDE_DIR)
