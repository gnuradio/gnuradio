# - try to find Qwt libraries and include files
# QWT_INCLUDE_DIR where to find qwt_global.h, etc.
# QWT_LIBRARIES libraries to link against
# QWT_FOUND If false, do not try to use Qwt
# qwt_global.h holds a string with the QWT version;
#   test to make sure it's at least 5.2

pkg_check_modules(PC_QWT "Qt5Qwt6")
set(QWT_QT_VERSION qt5)

find_path(QWT_INCLUDE_DIRS
  NAMES qwt_global.h
  HINTS
  ${PC_QWT_INCLUDEDIR}
  ${CMAKE_INSTALL_PREFIX}/include/qwt
  ${CMAKE_PREFIX_PATH}/include/qwt
  PATHS
  /usr/local/include/qwt-${QWT_QT_VERSION}
  /usr/local/include/qwt
  /usr/include/qwt6
  /usr/include/qt5/qwt
  /usr/include/qwt-${QWT_QT_VERSION}
  /usr/include/qwt
  /usr/include/${QWT_QT_VERSION}/qwt
  /usr/include/qwt5
  /opt/local/include/qwt
  /sw/include/qwt
  /usr/local/lib/qwt.framework/Headers
)

find_library (QWT_LIBRARIES
  NAMES qwt ${PC_QWT_LIBRARIES} qwt6-${QWT_QT_VERSION} qwt-${QWT_QT_VERSION}
  HINTS
  ${PC_QWT_LIBDIR}
  ${CMAKE_INSTALL_PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib64
  ${CMAKE_PREFIX_PATH}/lib
  PATHS
  /usr/local/lib
  /usr/lib
  /opt/local/lib
  /sw/lib
  /usr/local/lib/qwt.framework
)

set(QWT_FOUND FALSE)
set(QWT_VERSION "No Version")
set(QWT_WRONG_VERSION True)
if(QWT_INCLUDE_DIRS)
  file(STRINGS "${QWT_INCLUDE_DIRS}/qwt_global.h"
    QWT_STRING_VERSION REGEX "QWT_VERSION_STR")
  string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" QWT_INCLUDE_VERSION ${QWT_STRING_VERSION})
  string(COMPARE LESS ${QWT_INCLUDE_VERSION} "5.2.0" QWT_WRONG_VERSION)
  string(COMPARE GREATER ${QWT_INCLUDE_VERSION} "6.2.0" QWT_WRONG_VERSION)

  message(STATUS "Qwt headers version: ${QWT_INCLUDE_VERSION}")
endif(QWT_INCLUDE_DIRS)

if(QWT_LIBRARIES)
  get_filename_component(QWT_LIBRARY_FILENAME "${QWT_LIBRARIES}" REALPATH)
  message(STATUS "qwt: ${QWT_LIBRARY_FILENAME}")
  string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" QWT_LIBRARY_VERSION ${QWT_LIBRARY_FILENAME})
  string(COMPARE LESS ${QWT_LIBRARY_VERSION} "5.2.0" QWT_WRONG_VERSION)
  string(COMPARE GREATER ${QWT_LIBRARY_VERSION} "6.2.0" QWT_WRONG_VERSION)
  message(STATUS "Qwt library version: ${QWT_LIBRARY_VERSION}")
endif(QWT_LIBRARIES)

string(COMPARE EQUAL ${QWT_LIBRARY_VERSION} ${QWT_INCLUDE_VERSION} QWT_INCLUDE_AND_LIBRARY_SAME_VERSION)

if(NOT QWT_WRONG_VERSION)
  set(QWT_FOUND TRUE)
else(NOT QWT_WRONG_VERSION)
  message(STATUS "QWT Version must be >= 5.2 and <= 6.2.0, Found ${QWT_VERSION}")
endif(NOT QWT_WRONG_VERSION)

if(NOT QWT_INCLUDE_AND_LIBRARY_SAME_VERSION)
  set(QWT_FOUND FALSE)
  message(WARNING "Qwt include version and library version differ")
endif(NOT QWT_INCLUDE_AND_LIBRARY_SAME_VERSION)

if(QWT_FOUND)
  # handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE if
  # all listed variables are TRUE
  include ( FindPackageHandleStandardArgs )
  find_package_handle_standard_args( Qwt DEFAULT_MSG QWT_LIBRARIES QWT_INCLUDE_DIRS )
  MARK_AS_ADVANCED(QWT_LIBRARIES QWT_INCLUDE_DIRS)
  if (Qwt_FOUND AND NOT TARGET qwt::qwt)
    add_library(qwt::qwt INTERFACE IMPORTED)
    set_target_properties(qwt::qwt PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${QWT_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${QWT_LIBRARIES}"
      )
  endif()
endif(QWT_FOUND)
