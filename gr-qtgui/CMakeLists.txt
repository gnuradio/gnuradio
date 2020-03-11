# Copyright 2010-2011,2013,2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrBoost)

include(GrPython)

# Note: gr-qtgui requires Qt5.

find_package(Qt5Widgets QUIET)
set(QT_FOUND ${Qt5Widgets_FOUND})

GR_PYTHON_CHECK_MODULE("PyQt5" PyQt5 True PYQT5_FOUND)

find_package(Qwt)

########################################################################
# Register component
########################################################################
include(GrComponent)
if(NOT CMAKE_CROSSCOMPILING)
    set(qt_gui_python_deps
        PYQT5_FOUND
    )
endif(NOT CMAKE_CROSSCOMPILING)

GR_REGISTER_COMPONENT("gr-qtgui" ENABLE_GR_QTGUI
    Boost_FOUND
    QT_FOUND
    QWT_FOUND
    ENABLE_VOLK
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_FFT
    ENABLE_GR_FILTER
    PYTHONLIBS_FOUND
    ${qt_gui_python_deps}
)

SET(GR_PKG_QTGUI_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/qt-gui)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_QTGUI)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/qtgui)
add_subdirectory(lib)
add_subdirectory(docs)
add_subdirectory(examples/c++)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/qtgui)
    add_subdirectory(examples)
    add_subdirectory(apps)
endif(ENABLE_PYTHON)
if(ENABLE_GRC)
    add_subdirectory(grc)
endif(ENABLE_GRC)

########################################################################
# Create Pkg Config File
########################################################################
set( PC_LIBS "${QWT_LIBRARIES} ${QT_LIBRARIES}" )
string (REPLACE ";" " " PC_LIBS_STR "${PC_LIBS}")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-qtgui.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

########################################################################
# Install the conf file
########################################################################
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/gr-qtgui.conf
    DESTINATION ${GR_PREFSDIR}
)


########################################################################
# Install the themes (QSS) files
########################################################################
install(
    FILES
    themes/plain.qss
    themes/dark.qss
    themes/alt.qss
    themes/projector.qss
    DESTINATION ${GR_THEMES_DIR}
)

endif(ENABLE_GR_QTGUI)
