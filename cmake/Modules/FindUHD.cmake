########################################################################
#
# Find the library for the USRP Hardware Driver
# Priorty for prefix search is:
# 1) ENV(UHD_DIR)
# 2) pkg-config results, if available;
# 3) CMAKE_INSTALL_PREFIX
# 4) /usr/local/
# 5) /usr/
#
# If a version is provided, then:
#   If EXACT is set, then look for a specific version of UHD.
#   Otherwise, look for the provided version of newer of UHD.
#
# If no version is provided, then look for any version of UHD.
#
# If REQUIRED is set, then error out when the desired UHD version is
# not found.
#
########################################################################

SET(UHD_FOUND TRUE)
SET(UHD_INCLUDE_HINTS)
SET(UHD_LIBDIR_HINTS)
SET(UHD_DIR $ENV{UHD_DIR})

IF(UHD_DIR)
    LIST(APPEND UHD_INCLUDE_HINTS ${UHD_DIR}/include)
    LIST(APPEND UHD_LIBDIR_HINTS ${UHD_DIR}/lib)
ENDIF()

INCLUDE(FindPkgConfig)
IF(PKG_CONFIG_FOUND)
  IF(NOT ${CMAKE_VERSION} VERSION_LESS "2.8.0")
    SET(UHD_QUIET "QUIET")
  ENDIF()
  IF(UHD_FIND_VERSION)
    IF(UHD_FIND_VERSION_EXACT)
      PKG_CHECK_MODULES(PC_UHD ${UHD_QUIET} uhd=${UHD_FIND_VERSION})
    ELSE()
      PKG_CHECK_MODULES(PC_UHD ${UHD_QUIET} uhd>=${UHD_FIND_VERSION})
    ENDIF()
  ELSE()
    PKG_CHECK_MODULES(PC_UHD ${UHD_QUIET} uhd)
  ENDIF()
  IF(PC_UHD_FOUND)
    LIST(APPEND UHD_INCLUDE_HINTS ${PC_UHD_INCLUDEDIR})
    LIST(APPEND UHD_LIBDIR_HINTS ${PC_UHD_LIBDIR})
  ENDIF()
ENDIF()

LIST(APPEND UHD_INCLUDE_HINTS ${CMAKE_INSTALL_PREFIX}/include)
LIST(APPEND UHD_LIBDIR_HINTS ${CMAKE_INSTALL_PREFIX}/lib)

# Verify that <uhd/config.hpp> and libuhd are available, and, if a
# version is provided, that UHD meets the version requirements -- no
# matter what pkg-config might think.

FIND_PATH(
    UHD_INCLUDE_DIRS
    NAMES uhd/config.hpp
    HINTS ${UHD_INCLUDE_HINTS}
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    UHD_LIBRARIES
    NAMES uhd
    HINTS ${UHD_LIBDIR_HINTS}
    PATHS /usr/local/lib
          /usr/lib
)

IF(UHD_INCLUDE_DIRS AND UHD_LIBRARIES AND UHD_FIND_VERSION)

  # extract the actual UHD version from the installed header, and
  # compare with the value provided to this function.

  FILE(STRINGS "${UHD_INCLUDE_DIRS}/uhd/version.hpp"
   UHD_STRING_VERSION REGEX "UHD_VERSION_ABI_STRING")
  STRING(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+"
    UHD_VERSION_CONCISE ${UHD_STRING_VERSION})

  # convert UHD_FIND_VERSION into concise #.#.# format for comparison

  STRING(REGEX REPLACE "([^\\.]*)\\.([^\\.]*)\\.([^\\.]*)"
    "\\1.\\2.\\3" UHD_FIND_VERSION_TMP ${UHD_FIND_VERSION})

  STRING(REPLACE "0" "" UHD_FIND_MAJOR ${CMAKE_MATCH_1})
  STRING(REPLACE "0" "" UHD_FIND_MINOR ${CMAKE_MATCH_2})
  STRING(REPLACE "0" "" UHD_FIND_PATCH ${CMAKE_MATCH_3})

  # fix the case where the version number is "000"

  IF(NOT UHD_FIND_MAJOR)
    SET(UHD_FIND_MAJOR "0")
  ENDIF()
  IF(NOT UHD_FIND_MINOR)
    SET(UHD_FIND_MINOR "0")
  ENDIF()
  IF(NOT UHD_FIND_PATCH)
    SET(UHD_FIND_PATCH "0")
  ENDIF()

  SET(UHD_FIND_VERSION_CONCISE ${UHD_FIND_MAJOR}.${UHD_FIND_MINOR}.${UHD_FIND_PATCH})
  SET(UHD_WRONG_VERSION TRUE)

  IF(UHD_FIND_VERSION_EXACT)
    STRING(COMPARE NOTEQUAL ${UHD_VERSION_CONCISE}
      ${UHD_FIND_VERSION_CONCISE} UHD_WRONG_VERSION)
  ELSE()
    STRING(COMPARE LESS ${UHD_VERSION_CONCISE}
      ${UHD_FIND_VERSION_CONCISE} UHD_WRONG_VERSION)
  ENDIF()

  IF(UHD_WRONG_VERSION)
    SET(UHD_FOUND FALSE)
  ENDIF()
ENDIF()

IF(UHD_FOUND)
  INCLUDE(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(UHD DEFAULT_MSG UHD_LIBRARIES UHD_INCLUDE_DIRS)
  MARK_AS_ADVANCED(UHD_LIBRARIES UHD_INCLUDE_DIRS)
ELSEIF(UHD_FIND_REQUIRED)
  IF(UHD_FIND_VERSION)
    IF(UHD_FIND_VERSION_EXACT)
      MESSAGE(FATAL_ERROR "UHD version ${UHD_FIND_VERSION} is required, but was not found.")
    ELSE()
      MESSAGE(FATAL_ERROR "UHD version ${UHD_FIND_VERSION} or newer is required, but was not found.")
    ENDIF()
  ELSE()
    MESSAGE(FATAL_ERROR "UHD is required, but was not found.")
  ENDIF()
ENDIF()
