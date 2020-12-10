# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)
SET(PYUIC_PROG_NAMES pyuic5)

find_program(PYUIC_PROGRAM NAMES ${PYUIC_PROG_NAMES} )
if (NOT PYUIC_PROGRAM)
    message(ERROR "pyuic5 not found - aborting") 
endif (NOT PYUIC_PROGRAM)

# Build pyqt_filter_stacked.py from pyqt_filter_stacked.ui
# and put it in the build directory 
add_custom_command(
    DEPENDS pyqt_filter_stacked.ui OUTPUT pyqt_filter_stacked.py
    COMMAND ${PYUIC_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/pyqt_filter_stacked.ui -o pyqt_filter_stacked.py --import-from=.
)

GR_PYTHON_INSTALL(
    DEPENDS
        pyqt_filter_stacked.py
    FILES
	bandgraphicsview.py
	banditems.py
	icons_rc.py
	idealbanditems.py
	polezero_plot.py
	${CMAKE_CURRENT_BINARY_DIR}/pyqt_filter_stacked.py
	CustomViewBox.py
	GrFilterPlotWidget.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/filter
)

