# - try to find Qwt libraries and include files
# QWT_INCLUDE_DIR where to find qwt_global.h, etc.
# QWT_LIBRARIES libraries to link against
# QWT_FOUND If false, do not try to use Qwt
# qwt_global.h holds a string with the QWT version;
#   test to make sure it's at least 5.2

pkg_check_modules(PC_QWT "Qt5Qwt6")
set(QWT_QT_VERSION qt5)

find_path(
    QWT_INCLUDE_DIRS
    NAMES qwt_global.h
    HINTS ${PC_QWT_INCLUDEDIR} ${CMAKE_INSTALL_PREFIX}/include /include
    PATHS /usr/local/include /usr/include /opt/local/include /sw/include
          /usr/local/lib/qwt.framework/Headers
    PATH_SUFFIXES qwt-${QWT_QT_VERSION} qwt qwt6 qwt5 ${QWT_QT_VERSION}/qwt)

find_library(
    QWT_LIBRARIES
    NAMES ${PC_QWT_LIBRARIES} qwt6-${QWT_QT_VERSION} qwt-${QWT_QT_VERSION} qwt
    HINTS ${PC_QWT_LIBDIR} ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64 /lib
    PATHS /usr/local/lib /usr/lib /opt/local/lib /sw/lib /usr/local/lib/qwt.framework)

set(QWT_FOUND FALSE)
if(QWT_INCLUDE_DIRS)
    file(STRINGS "${QWT_INCLUDE_DIRS}/qwt_global.h" QWT_STRING_VERSION
         REGEX "QWT_VERSION_STR")
    set(QWT_WRONG_VERSION False)
    set(QWT_VERSION "No Version")
    string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" QWT_VERSION ${QWT_STRING_VERSION})
    if(QWT_VERSION VERSION_LESS "6.1")
        set(QWT_WRONG_VERSION True)
    endif()

    message(STATUS "QWT Version: ${QWT_VERSION}")
    if(NOT QWT_WRONG_VERSION)
        set(QWT_FOUND TRUE)
    else(NOT QWT_WRONG_VERSION)
        message(STATUS "QWT Version must be >= 6.1, Found ${QWT_VERSION}")
    endif(NOT QWT_WRONG_VERSION)

endif(QWT_INCLUDE_DIRS)

if(QWT_FOUND)
    # handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE if
    # all listed variables are TRUE
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Qwt DEFAULT_MSG QWT_LIBRARIES QWT_INCLUDE_DIRS)
    mark_as_advanced(QWT_LIBRARIES QWT_INCLUDE_DIRS)
    if(Qwt_FOUND AND NOT TARGET qwt::qwt)
        add_library(qwt::qwt INTERFACE IMPORTED)
        set_target_properties(
            qwt::qwt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${QWT_INCLUDE_DIRS}"
                                INTERFACE_LINK_LIBRARIES "${QWT_LIBRARIES}")
    endif()
endif(QWT_FOUND)
