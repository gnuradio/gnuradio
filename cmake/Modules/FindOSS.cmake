# - Find Oss
# Find Oss headers and libraries.
#
#  OSS_INCLUDE_DIR  -  where to find soundcard.h, etc.
#  OSS_FOUND        - True if Oss found.

# OSS is not for APPLE or WINDOWS

if(APPLE OR WIN32)
    return()
endif()

find_path(LINUX_OSS_INCLUDE_DIR "linux/soundcard.h" "/usr/include" "/usr/local/include")

find_path(SYS_OSS_INCLUDE_DIR "sys/soundcard.h" "/usr/include" "/usr/local/include")

find_path(MACHINE_OSS_INCLUDE_DIR "machine/soundcard.h" "/usr/include"
          "/usr/local/include")

set(OSS_FOUND FALSE)

if(LINUX_OSS_INCLUDE_DIR)
    set(OSS_FOUND TRUE)
    set(OSS_INCLUDE_DIR ${LINUX_OSS_INCLUDE_DIR})
    set(HAVE_LINUX_SOUNDCARD_H 1)
endif()

if(SYS_OSS_INCLUDE_DIR)
    set(OSS_FOUND TRUE)
    set(OSS_INCLUDE_DIR ${SYS_OSS_INCLUDE_DIR})
    set(HAVE_SYS_SOUNDCARD_H 1)
endif()

if(MACHINE_OSS_INCLUDE_DIR)
    set(OSS_FOUND TRUE)
    set(OSS_INCLUDE_DIR ${MACHINE_OSS_INCLUDE_DIR})
    set(HAVE_MACHINE_SOUNDCARD_H 1)
endif()

mark_as_advanced(OSS_FOUND OSS_INCLUDE_DIR LINUX_OSS_INCLUDE_DIR SYS_OSS_INCLUDE_DIR
                 MACHINE_OSS_INCLUDE_DIR)

if(OSS_FOUND AND NOT TARGET OSS::OSS)
    add_library(OSS::OSS INTERFACE IMPORTED)
    set_target_properties(OSS::OSS PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                                              "${OSS_INCLUDE_DIRS}")
endif()
