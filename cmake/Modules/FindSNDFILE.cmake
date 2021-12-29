find_package(PkgConfig)
pkg_check_modules(PC_SNDFILE "sndfile")

find_path(
    SNDFILE_INCLUDE_DIRS
    NAMES sndfile.h
    HINTS ${PC_SNDFILE_INCLUDE_DIR} ${CMAKE_INSTALL_PREFIX}/include
    PATHS /usr/local/include /usr/include)

find_library(
    SNDFILE_LIBRARIES
    NAMES sndfile ${SNDFILE_LIBRARY_NAME}
    HINTS ${PC_SNDFILE_LIBDIR} ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS ${SNDFILE_INCLUDE_DIRS}/../lib /usr/local/lib /usr/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SNDFILE DEFAULT_MSG SNDFILE_LIBRARIES
                                  SNDFILE_INCLUDE_DIRS)
mark_as_advanced(SNDFILE_LIBRARIES SNDFILE_INCLUDE_DIRS)

if(SNDFILE_FOUND AND NOT TARGET sndfile::sndfile)
    add_library(sndfile::sndfile INTERFACE IMPORTED)
    set_target_properties(
        sndfile::sndfile
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SNDFILE_INCLUDE_DIRS}"
                   INTERFACE_LINK_LIBRARIES "${SNDFILE_LIBRARIES}")
endif()
