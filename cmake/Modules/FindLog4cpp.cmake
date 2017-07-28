# - Find Log4cpp
# Find the native LOG4CPP includes and library
#
# Defines the following variables on exit:
#  LOG4CPP_INCLUDE_DIRS - where to find LOG4CPP.h, etc.
#  LOG4CPP_LIBRARIES    - List of libraries when using LOG4CPP.
#  LOG4CPP_FOUND        - True if LOG4CPP found.

# if LOG4CPP_FOUND is already defined, just return;
# assume that whatever argument(s) is/are provided to CMake are valid.
if(DEFINED LOG4CPP_FOUND)
  return()
endif()

if (LOG4CPP_INCLUDE_DIR)
  # Already in cache, be silent
  set(LOG4CPP_FIND_QUIETLY TRUE)
endif ()

find_path(LOG4CPP_INCLUDE_DIR log4cpp/Category.hh
  HINTS ${LOG4CPP_INCLUDE_DIRS} ${LOG4CPP_INCLUDE_DIR}
  PATHS /opt/local/include
    /usr/local/include
    /usr/include
)

set(LOG4CPP_NAMES log4cpp)
find_library(LOG4CPP_LIBRARY
  NAMES ${LOG4CPP_NAMES}
  HINTS ${LOG4CPP_LIBRARIES} ${LOG4CPP_LIBRARY}
  PATHS /usr/lib /usr/local/lib /opt/local/lib
)

if (LOG4CPP_INCLUDE_DIR AND LOG4CPP_LIBRARY)
  set(LOG4CPP_FOUND TRUE)
  set(LOG4CPP_LIBRARIES ${LOG4CPP_LIBRARY} CACHE INTERNAL "" FORCE)
  set(LOG4CPP_INCLUDE_DIRS ${LOG4CPP_INCLUDE_DIR} CACHE INTERNAL "" FORCE)
else ()
  set(LOG4CPP_FOUND FALSE CACHE INTERNAL "" FORCE)
  set(LOG4CPP_LIBRARY "" CACHE INTERNAL "" FORCE)
  set(LOG4CPP_LIBRARIES "" CACHE INTERNAL "" FORCE)
  set(LOG4CPP_INCLUDE_DIR "" CACHE INTERNAL "" FORCE)
  set(LOG4CPP_INCLUDE_DIRS "" CACHE INTERNAL "" FORCE)
endif ()

if (LOG4CPP_FOUND)
  if (NOT LOG4CPP_FIND_QUIETLY)
    message(STATUS "Found LOG4CPP: ${LOG4CPP_LIBRARIES}")
  endif ()
else ()
  if (LOG4CPP_FIND_REQUIRED)
    message(STATUS "Looked for LOG4CPP libraries named ${LOG4CPPS_NAMES}.")
    message(FATAL_ERROR "Could NOT find LOG4CPP library")
  endif ()
endif ()

mark_as_advanced(
  LOG4CPP_LIBRARIES
  LOG4CPP_INCLUDE_DIRS
)
