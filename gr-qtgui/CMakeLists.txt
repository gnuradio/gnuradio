# Copyright 2010-2011 Free Software Foundation, Inc.
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
INCLUDE(GrBoost)

FIND_PACKAGE(Qt4 4.2.0 COMPONENTS QtCore QtGui)
INCLUDE(${QT_USE_FILE}) #sets up the environment

FIND_PACKAGE(Qwt)

FIND_PACKAGE(PythonLibs)

INCLUDE(GrPython)
GR_PYTHON_CHECK_MODULE("PyQt4" PyQt4 True PYQT4_FOUND)

########################################################################
# Register component
########################################################################
INCLUDE(GrComponent)
GR_REGISTER_COMPONENT("gr-qtgui" ENABLE_GR_QTGUI
    Boost_FOUND
    QT4_FOUND
    QWT_FOUND
    ENABLE_GR_CORE_
    PYTHONLIBS_FOUND
    PYQT4_FOUND
)

GR_SET_GLOBAL(GR_QTGUI_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_BINARY_DIR}/lib
)

########################################################################
# Begin conditional configuration
########################################################################
IF(ENABLE_GR_QTGUI)

########################################################################
# Setup CPack components
########################################################################
INCLUDE(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_QTGUI_DESCRIPTION "The GNU Radio QtGUI Blocks")

CPACK_COMPONENT("qtgui_runtime"
    GROUP        "QtGUI"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "core_runtime"
)

CPACK_COMPONENT("qtgui_devel"
    GROUP        "QtGUI"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "core_devel"
)

CPACK_COMPONENT("qtgui_python"
    GROUP        "QtGUI"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "core_python;qtgui_runtime"
)

CPACK_COMPONENT("qtgui_swig"
    GROUP        "QtGUI"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "core_swig;qtgui_python;qtgui_devel"
)

########################################################################
# Add subdirectories
########################################################################
ADD_SUBDIRECTORY(lib)
IF(ENABLE_PYTHON)
    ADD_SUBDIRECTORY(grc)
    ADD_SUBDIRECTORY(swig)
    ADD_SUBDIRECTORY(python)
ENDIF(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-qtgui.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.pc
@ONLY)

INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "qtgui_devel"
)

ENDIF(ENABLE_GR_QTGUI)
