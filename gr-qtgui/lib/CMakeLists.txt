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
# Setup the QT file generations stuff
########################################################################
SET(qtgui_moc_hdrs
    spectrumdisplayform.h
    timedisplayform.h
    FrequencyDisplayPlot.h
    TimeDomainDisplayPlot.h
    WaterfallDisplayPlot.h
    ConstellationDisplayPlot.h
)
QT4_WRAP_CPP(qtgui_moc_srcs ${qtgui_moc_hdrs})
QT4_WRAP_UI(qtgui_ui_hdrs spectrumdisplayform.ui)

#FIXME the sources expect <foo>.ui.h, but the macros generate ui_foo.h
#avoid changing the sources by generating the header with the include
SET(spectrum_ui_hdr ${CMAKE_CURRENT_BINARY_DIR}/spectrumdisplayform.ui.h)
IF(NOT EXISTS ${spectrum_ui_hdr})
    FILE(WRITE ${spectrum_ui_hdr} "#include <ui_spectrumdisplayform.h>\n")
ENDIF(NOT EXISTS ${spectrum_ui_hdr})

SET(qtgui_srcs
    ${qtgui_moc_srcs}
    ${qtgui_ui_hdrs}
    FrequencyDisplayPlot.cc
    TimeDomainDisplayPlot.cc
    WaterfallDisplayPlot.cc
    waterfallGlobalData.cc
    ConstellationDisplayPlot.cc
    spectrumdisplayform.cc
    timedisplayform.cc
    SpectrumGUIClass.cc
    spectrumUpdateEvents.cc
    plot_waterfall.cc
    qtgui_sink_c.cc
    qtgui_sink_f.cc
    qtgui_time_sink_c.cc
    qtgui_time_sink_f.cc
    qtgui_util.cc
)

########################################################################
# Setup the include and linker paths
########################################################################
INCLUDE_DIRECTORIES(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(${QWT_INCLUDE_DIRS})
LINK_DIRECTORIES(${QWT_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})

########################################################################
# Setup library
########################################################################
LIST(APPEND qtgui_libs
    gnuradio-core
    ${QT_LIBRARIES}
    ${QWT_LIBRARIES}
    ${PYTHON_LIBRARIES}
)

ADD_DEFINITIONS(-DQWT_DLL) #setup QWT library linkage
ADD_LIBRARY(gnuradio-qtgui SHARED ${qtgui_srcs})
TARGET_LINK_LIBRARIES(gnuradio-qtgui ${qtgui_libs})
SET_TARGET_PROPERTIES(gnuradio-qtgui PROPERTIES DEFINE_SYMBOL "libgnuradio_qtgui_EXPORTS")
SET_TARGET_PROPERTIES(gnuradio-qtgui PROPERTIES SOVERSION ${LIBVER})

INSTALL(TARGETS gnuradio-qtgui
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "qtgui_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "qtgui_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "qtgui_runtime" # .dll file
)

########################################################################
# Install the header files
########################################################################
INSTALL(FILES
    FrequencyDisplayPlot.h
    TimeDomainDisplayPlot.h
    WaterfallDisplayPlot.h
    waterfallGlobalData.h
    ConstellationDisplayPlot.h
    highResTimeFunctions.h
    plot_waterfall.h
    spectrumdisplayform.h
    SpectrumGUIClass.h
    spectrumUpdateEvents.h
    qtgui_sink_c.h
    qtgui_sink_f.h
    qtgui_util.h
    gr_qtgui_api.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "qtgui_devel"
)
