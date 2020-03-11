# - Try to find jack-2.6
# Once done this will define
#
#  JACK_FOUND - system has jack
#  JACK_INCLUDE_DIRS - the jack include directory
#  JACK_LIBRARIES - Link these to use jack
#
#  Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
#  Modified for other libraries by Lasse Kärkkäinen <tronic>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (JACK_LIBRARIES AND JACK_INCLUDE_DIRS)
  # in cache already
  set(JACK_FOUND TRUE)
else (JACK_LIBRARIES AND JACK_INCLUDE_DIRS)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  if (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    include(UsePkgConfig)
    pkgconfig(jack _JACK_INCLUDEDIR _JACK_LIBDIR _JACK_LDFLAGS _JACK_CFLAGS)
  else (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
      pkg_check_modules(_JACK jack)
    endif (PKG_CONFIG_FOUND)
  endif (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
  find_path(JACK_INCLUDE_DIR
    NAMES
      jack/jack.h
    PATHS
      ${_JACK_INCLUDEDIR}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

  find_library(JACK_LIBRARY
    NAMES
      jack
    PATHS
      ${_JACK_LIBDIR}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (JACK_LIBRARY AND JACK_INCLUDE_DIR)
    set(JACK_FOUND TRUE)

    set(JACK_INCLUDE_DIRS
      ${JACK_INCLUDE_DIR}
    )

    set(JACK_LIBRARIES
      ${JACK_LIBRARIES}
      ${JACK_LIBRARY}
    )

  endif (JACK_LIBRARY AND JACK_INCLUDE_DIR)

  if (JACK_FOUND)
    if (NOT JACK_FIND_QUIETLY)
      message(STATUS "Found jack: ${JACK_LIBRARY}")
    endif (NOT JACK_FIND_QUIETLY)
  else (JACK_FOUND)
    if (JACK_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find JACK")
    endif (JACK_FIND_REQUIRED)
  endif (JACK_FOUND)

  # show the JACK_INCLUDE_DIRS and JACK_LIBRARIES variables only in the advanced view
  mark_as_advanced(JACK_INCLUDE_DIRS JACK_LIBRARIES)

  if (JACK_FOUND AND NOT TARGET JACK::JACK)
    add_library(JACK::JACK INTERFACE IMPORTED)
    set_target_properties(JACK::JACK PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${JACK_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${JACK_LIBRARIES}"
      )
  endif()


endif (JACK_LIBRARIES AND JACK_INCLUDE_DIRS)
