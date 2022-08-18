# - Try to find Portaudio
# Once done this will define
#
#  PORTAUDIO_FOUND - system has Portaudio
#  PORTAUDIO_INCLUDE_DIRS - the Portaudio include directory
#  PORTAUDIO_LIBRARIES - Link these to use Portaudio

find_package(PkgConfig)
pkg_check_modules(PC_PORTAUDIO portaudio-2.0)

find_path(
    PORTAUDIO_INCLUDE_DIRS
    NAMES portaudio.h
    PATHS /usr/local/include /usr/include
    HINTS ${PC_PORTAUDIO_INCLUDEDIR})

find_library(
    PORTAUDIO_LIBRARIES
    NAMES portaudio
    PATHS /usr/local/lib /usr/lib /usr/lib64
    HINTS ${PC_PORTAUDIO_LIBDIR})

mark_as_advanced(PORTAUDIO_INCLUDE_DIRS PORTAUDIO_LIBRARIES)

# Found PORTAUDIO, but it may be version 18 which is not acceptable.
if(EXISTS ${PORTAUDIO_INCLUDE_DIRS}/portaudio.h)
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_INCLUDES_SAVED ${CMAKE_REQUIRED_INCLUDES})
    set(CMAKE_REQUIRED_INCLUDES ${PORTAUDIO_INCLUDE_DIRS})
    check_cxx_source_compiles(
        "#include <portaudio.h>\nPaDeviceIndex pa_find_device_by_name(const char *name); int main () {return 0;}"
        PORTAUDIO2_FOUND)
    set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVED})
    unset(CMAKE_REQUIRED_INCLUDES_SAVED)
    if(PORTAUDIO2_FOUND)
        include(FindPackageHandleStandardArgs)
        find_package_handle_standard_args(PORTAUDIO DEFAULT_MSG PORTAUDIO_INCLUDE_DIRS
                                          PORTAUDIO_LIBRARIES)
        if(PORTAUDIO_FOUND AND NOT TARGET Portaudio::Portaudio)
            add_library(Portaudio::Portaudio INTERFACE IMPORTED)
            set_target_properties(
                Portaudio::Portaudio
                PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${PORTAUDIO_INCLUDE_DIRS}"
                           INTERFACE_LINK_LIBRARIES "${PORTAUDIO_LIBRARIES}")
        endif()
    else(PORTAUDIO2_FOUND)
        message(STATUS "  portaudio.h not compatible (requires API 2.0)")
        set(PORTAUDIO_FOUND FALSE)
    endif(PORTAUDIO2_FOUND)
endif()
