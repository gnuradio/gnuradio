#!/usr/bin/env python
#
# Copyright 2012,2013,2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import print_function
from __future__ import unicode_literals
from gnuradio import gr
from gnuradio import blocks
import sys

try:
    from gnuradio import qtgui
    from PyQt5 import QtWidgets, Qt
    import sip
except ImportError:
    print("Error: Program requires PyQt5 and gr-qtgui.")
    sys.exit(1)

class dialog_box(QtWidgets.QWidget):
    def __init__(self, display):
        QtWidgets.QWidget.__init__(self, None)
        self.setWindowTitle('PyQt Test GUI')

        self.boxlayout = QtWidgets.QBoxLayout(QtWidgets.QBoxLayout.LeftToRight, self)
        self.boxlayout.addWidget(display, 1)

        self.resize(800, 500)

class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        self.qapp = QtWidgets.QApplication(sys.argv)

        data0  = 10*[0,] + 40*[1,0] + 10*[0,]
        data0 += 10*[0,] + 40*[0,1] + 10*[0,]
        data1 = 20*[0,] + [0,0,0,1,1,1,0,0,0,0] + 70*[0,]

        # Adjust these to change the layout of the plot.
        # Can be set to fractions.
        ncols = 100.25
        nrows = 100

        fs = 200
        src0 = blocks.vector_source_f(data0, True)
        src1 = blocks.vector_source_f(data1, True)
        thr = blocks.throttle(gr.sizeof_float, 50000)
        hed = blocks.head(gr.sizeof_float, 10000000)
        self.snk1 = qtgui.time_raster_sink_f(fs, nrows, ncols, [], [],
                                             "Float Time Raster Example", 2)

        self.connect(src0, thr, (self.snk1, 0))
        self.connect(src1, (self.snk1, 1))

        # Get the reference pointer to the SpectrumDisplayForm QWidget
        pyQt = self.snk1.pyqwidget()

        # Wrap the pointer as a PyQt SIP object
        # This can now be manipulated as a PyQt5.QtWidgets.QWidget
        pyWin = sip.wrapinstance(pyQt, QtWidgets.QWidget)

        self.main_box = dialog_box(pyWin)
        self.main_box.show()

if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    tb.qapp.exec_()
    tb.stop()
