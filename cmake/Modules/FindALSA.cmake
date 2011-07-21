# - Try to find ALSA
# Once done, this will define
#
#  ALSA_FOUND - system has ALSA (GL and GLU)
#  ALSA_INCLUDE_DIRS - the ALSA include directories
#  ALSA_LIBRARIES - link these to use ALSA
#  ALSA_GL_LIBRARY - only GL
#  ALSA_GLU_LIBRARY - only GLU
#
# See documentation on how to write CMake scripts at
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries

include(LibFindMacros)

libfind_pkg_check_modules(ALSA_PKGCONF alsa)

find_path(ALSA_INCLUDE_DIR
  NAMES alsa/version.h
  PATHS ${ALSA_PKGCONF_INCLUDE_DIRS}
)

find_library(ALSA_LIBRARY
  NAMES asound
  PATHS ${ALSA_PKGCONF_LIBRARY_DIRS}
)

# Extract the version number
IF(ALSA_INCLUDE_DIR)
file(READ "${ALSA_INCLUDE_DIR}/alsa/version.h" _ALSA_VERSION_H_CONTENTS)
string(REGEX REPLACE ".*#define SND_LIB_VERSION_STR[ \t]*\"([^\n]*)\".*" "\\1" ALSA_VERSION "${_ALSA_VERSION_H_CONTENTS}")
ENDIF(ALSA_INCLUDE_DIR)

set(ALSA_PROCESS_INCLUDES ALSA_INCLUDE_DIR)
set(ALSA_PROCESS_LIBS ALSA_LIBRARY)
libfind_process(ALSA)

