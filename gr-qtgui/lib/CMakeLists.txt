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
# Setup the QT file generations stuff
########################################################################
set(qtgui_moc_hdrs
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
set(spectrum_ui_hdr ${CMAKE_CURRENT_BINARY_DIR}/spectrumdisplayform.ui.h)
if(NOT EXISTS ${spectrum_ui_hdr})
    file(WRITE ${spectrum_ui_hdr} "#include <ui_spectrumdisplayform.h>\n")
endif(NOT EXISTS ${spectrum_ui_hdr})

set(qtgui_srcs
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
include_directories(
    ${GR_QTGUI_INCLUDE_DIRS}
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GRUEL_INCLUDE_DIRS}
    ${QWT_INCLUDE_DIRS}
    ${QT_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
)

link_directories(
	${QWT_LIBRARY_DIRS}
	${Boost_LIBRARY_DIRS}
)

include_directories(${PYTHON_INCLUDE_PATH}) #deprecated for dirs (cmake 2.6)

########################################################################
# Setup library
########################################################################
list(APPEND qtgui_libs
    gnuradio-core
    ${QWT_LIBRARIES}
    ${QT_LIBRARIES}
    ${PYTHON_LIBRARIES}
)

add_definitions(-DQWT_DLL) #setup QWT library linkage
add_library(gnuradio-qtgui SHARED ${qtgui_srcs})
target_link_libraries(gnuradio-qtgui ${qtgui_libs})
GR_LIBRARY_FOO(gnuradio-qtgui RUNTIME_COMPONENT "qtgui_runtime" DEVEL_COMPONENT "qtgui_devel")

########################################################################
# Install the header files
########################################################################
install(FILES
    FrequencyDisplayPlot.h
    TimeDomainDisplayPlot.h
    WaterfallDisplayPlot.h
    waterfallGlobalData.h
    ConstellationDisplayPlot.h
    plot_waterfall.h
    spectrumdisplayform.h
    timedisplayform.h
    SpectrumGUIClass.h
    spectrumUpdateEvents.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "qtgui_devel"
)
