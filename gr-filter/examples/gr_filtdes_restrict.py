#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio.filter import filter_design
import sys
try:
    from PyQt5 import Qt, QtCore, QtGui
except ImportError:
    print("Please install PyQt5 to run this script (http://www.riverbankcomputing.co.uk/software/pyqt/download)")
    raise SystemExit(1)


'''
Callback with restrict example
Function called when "design" button is pressed
or pole-zero plot is changed
'''
def print_params(filtobj):
    print("Filter Count:", filtobj.get_filtercount())
    print("Filter type:", filtobj.get_restype())
    print("Filter params", filtobj.get_params())
    print("Filter Coefficients", filtobj.get_taps())

app = Qt.QApplication(sys.argv)
main_win = filter_design.launch(sys.argv, callback = print_params, restype = "iir")
main_win.show()
app.exec_()
