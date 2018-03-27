# Copyright 2010-2011,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup dependencies
########################################################################
include(GrBoost)

find_package(Qt4 4.2.0 COMPONENTS QtCore QtGui)

find_package(Qwt)

find_package(PythonLibs 2)

include(GrPython)
GR_PYTHON_CHECK_MODULE("PyQt4" PyQt4 True PYQT4_FOUND)

########################################################################
# Register component
########################################################################
include(GrComponent)
if(NOT CMAKE_CROSSCOMPILING)
    set(qt_gui_python_deps
        PYQT4_FOUND
    )
endif(NOT CMAKE_CROSSCOMPILING)

GR_REGISTER_COMPONENT("gr-qtgui" ENABLE_GR_QTGUI
    Boost_FOUND
    QT4_FOUND
    QWT_FOUND
    ENABLE_VOLK
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_FFT
    ENABLE_GR_FILTER
    PYTHONLIBS_FOUND
    ${qt_gui_python_deps}
)

GR_SET_GLOBAL(GR_QTGUI_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_BINARY_DIR}/lib
)

SET(GR_PKG_QTGUI_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/qt-gui)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_QTGUI)

# populate the environment with QT variables
include(GrSetupQt4)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_QTGUI_DESCRIPTION "GNU Radio QtGUI Blocks")

CPACK_COMPONENT("qtgui_runtime"
    GROUP        "QtGUI"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("qtgui_devel"
    GROUP        "QtGUI"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("qtgui_python"
    GROUP        "QtGUI"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "runtime_python;qtgui_runtime"
)

CPACK_COMPONENT("qtgui_swig"
    GROUP        "QtGUI"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;qtgui_python;qtgui_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/qtgui)
add_subdirectory(lib)
add_subdirectory(docs)
add_subdirectory(examples/c++)
if(ENABLE_PYTHON)
    add_subdirectory(grc)
    add_subdirectory(swig)
    add_subdirectory(python/qtgui)
    add_subdirectory(examples)
    add_subdirectory(apps)
endif(ENABLE_PYTHON)

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
    COMPONENT "qtgui_devel"
)

########################################################################
# Install the conf file
########################################################################
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/gr-qtgui.conf
    DESTINATION ${GR_PREFSDIR}
    COMPONENT "qtgui"
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
    COMPONENT "qtgui"
)

endif(ENABLE_GR_QTGUI)
