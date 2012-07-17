# Copyright 2011 Free Software Foundation, Inc.
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
include(GrPython)

GR_PYTHON_CHECK_MODULE("wx >= 2.8"  wx           "wx.version().split()[0] >= '2.8'"    WX_FOUND)
GR_PYTHON_CHECK_MODULE("numpy"      numpy        True                                  NUMPY_FOUND)

########################################################################
# Register component
########################################################################
include(GrComponent)
if(NOT CMAKE_CROSSCOMPILING)
    set(wxgui_python_deps
        NUMPY_FOUND
        WX_FOUND
    )
endif(NOT CMAKE_CROSSCOMPILING)

GR_REGISTER_COMPONENT("gr-wxgui" ENABLE_GR_WXGUI
    ENABLE_GR_CORE
    ENABLE_GR_FFT
    ENABLE_GR_FILTER
    ENABLE_PYTHON
    ${wxgui_python_deps}
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_WXGUI)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_COMPONENT("wxgui"
    DISPLAY_NAME "WxGUI"
    DESCRIPTION  "Wx GUI plotter widgets and grc wrappers"
    DEPENDS      "core_python"
)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gr-wxgui.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gr-wxgui.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gr-wxgui.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "wxgui"
)

########################################################################
# Install the conf file
########################################################################
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/gr-wxgui.conf
    DESTINATION ${GR_PREFSDIR}
    COMPONENT "wxgui"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(grc)
add_subdirectory(src/python)

endif(ENABLE_GR_WXGUI)
