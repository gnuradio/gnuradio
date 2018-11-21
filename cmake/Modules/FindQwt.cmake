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
  NAMES ${PC_QWT_LIBRARIES} qwt6-${QWT_QT_VERSION} qwt-${QWT_QT_VERSION}
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
if(QWT_INCLUDE_DIRS)
  file(STRINGS "${QWT_INCLUDE_DIRS}/qwt_global.h"
    QWT_STRING_VERSION REGEX "QWT_VERSION_STR")
  set(QWT_WRONG_VERSION True)
  set(QWT_VERSION "No Version")
  string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" QWT_VERSION ${QWT_STRING_VERSION})
  string(COMPARE LESS ${QWT_VERSION} "5.2.0" QWT_WRONG_VERSION)
  string(COMPARE GREATER ${QWT_VERSION} "6.2.0" QWT_WRONG_VERSION)

  message(STATUS "QWT Version: ${QWT_VERSION}")
  if(NOT QWT_WRONG_VERSION)
    set(QWT_FOUND TRUE)
  else(NOT QWT_WRONG_VERSION)
    message(STATUS "QWT Version must be >= 5.2 and <= 6.2.0, Found ${QWT_VERSION}")
  endif(NOT QWT_WRONG_VERSION)

endif(QWT_INCLUDE_DIRS)

if(QWT_FOUND)
  # handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE if
  # all listed variables are TRUE
  include ( FindPackageHandleStandardArgs )
  find_package_handle_standard_args( Qwt DEFAULT_MSG QWT_LIBRARIES QWT_INCLUDE_DIRS )
  MARK_AS_ADVANCED(QWT_LIBRARIES QWT_INCLUDE_DIRS)
endif(QWT_FOUND)
