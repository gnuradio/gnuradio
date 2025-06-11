# Copyright 2010-2011,2013,2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrPython)

# Note: gr-qtgui requires Qt5.
gr_find_package(Qt5
    COMPONENTS Widgets
    OPTIONAL_COMPONENTS OpenGL
    GLOBAL
)

if(TARGET Qt5::Widgets)
    set(QT_FOUND TRUE)
else()
    set(QT_FOUND FALSE)
endif()

gr_python_check_module(
    DESC "PyQt5"
    MODULE PyQt5
    VAR PYQT5_FOUND
)
gr_find_package(qwt GLOBAL)
if(qwt_FOUND)
    set(QWT_FOUND TRUE)
endif()

########################################################################
# Register component
########################################################################
include(GrComponent)
if(NOT CMAKE_CROSSCOMPILING)
    set(qt_gui_python_deps PYQT5_FOUND)
endif(NOT CMAKE_CROSSCOMPILING)

gr_register_component(
    NAME "gr-qtgui"
    VAR ENABLE_GR_QTGUI
    REQUIRED_TPLS
        QT_FOUND
        QWT_FOUND
        PYTHONLIBS_FOUND
        ${qt_gui_python_deps}
    REQUIRED_COMPONENTS
        gnuradio-runtime
        gr-fft
        gr-filter
)

set(GR_PKG_QTGUI_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/qt-gui)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_QTGUI)
    if (Qt5OpenGL_FOUND)
        set(ENABLE_GR_QTGUI_OPENGL TRUE)
        set(OpenGL_GL_PREFERENCE GLVND)
    endif()
    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/qtgui)
    add_subdirectory(lib)
    add_subdirectory(docs)
    if(ENABLE_GR_ANALOG AND ENABLE_EXAMPLES)
        add_subdirectory(examples/c++)
    endif(ENABLE_GR_ANALOG AND ENABLE_EXAMPLES)
    if(ENABLE_PYTHON)
        add_subdirectory(python/qtgui)
        if(ENABLE_EXAMPLES)
            add_subdirectory(examples)
        endif(ENABLE_EXAMPLES)
        add_subdirectory(apps)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    set(PC_LIBS "${QWT_LIBRARIES} ${QT_LIBRARIES}")
    string(REPLACE ";" " " PC_LIBS_STR "${PC_LIBS}")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-qtgui.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

    ########################################################################
    # Install the conf file
    ########################################################################
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/gr-qtgui.conf DESTINATION ${GR_PREFSDIR_relative_to_prefix})

    ########################################################################
    # Install the themes (QSS) files
    ########################################################################
    install(FILES
            themes/alt.qss
            themes/dark.qss
            themes/nsf.qss
            themes/plain.qss
            themes/projector.qss
            DESTINATION ${GR_THEMES_DIR})

    if(WIN32)
        install(DIRECTORY ${qt_Qt5_QWindowsIntegrationPlugin_LIB_DIRS_RELEASE}
                DESTINATION Python)
    elseif(APPLE)
        install(DIRECTORY ${Qt5_DIR}/plugins/platforms DESTINATION python311/python.framework/)
    endif()

endif(ENABLE_GR_QTGUI)
