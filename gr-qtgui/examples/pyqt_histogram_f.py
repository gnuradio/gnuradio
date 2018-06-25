#!/usr/bin/env python
#
# Copyright 2013,2015 Free Software Foundation, Inc.
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
    sys.stderr.write("Error: Program requires PyQt5 and gr-qtgui.\n")
    sys.exit(1)

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

class dialog_box(QtWidgets.QWidget):
    def __init__(self, display, control):
        QtWidgets.QWidget.__init__(self, None)
        self.setWindowTitle('PyQt Test GUI')

        self.boxlayout = QtWidgets.QBoxLayout(QtWidgets.QBoxLayout.LeftToRight, self)
        self.boxlayout.addWidget(display, 1)
        self.boxlayout.addWidget(control)

        self.resize(800, 500)

class control_box(QtWidgets.QWidget):
    def __init__(self, snk, parent=None):
        QtWidgets.QWidget.__init__(self, parent)
        self.setWindowTitle('Control Panel')
        self.snk = snk

        self.setToolTip('Control the signals')
        QtWidgets.QToolTip.setFont(Qt.QFont('OldEnglish', 10))

        self.layout = QtWidgets.QFormLayout(self)

        # Control the first signal
        self.freq1Edit = QtWidgets.QLineEdit(self)
        self.freq1Edit.setMinimumWidth(100)
        self.layout.addRow("Sine Frequency:", self.freq1Edit)
        self.freq1Edit.editingFinished.connect(self.freq1EditText)

        self.amp1Edit = QtWidgets.QLineEdit(self)
        self.amp1Edit.setMinimumWidth(100)
        self.layout.addRow("Sine Amplitude:", self.amp1Edit)
        self.amp1Edit.editingFinished.connect(self.amp1EditText)


        # Control the second signal
        self.amp2Edit = QtWidgets.QLineEdit(self)
        self.amp2Edit.setMinimumWidth(100)
        self.layout.addRow("Noise Amplitude:", self.amp2Edit)
        self.amp2Edit.editingFinished.connect(self.amp2EditText)

        # Control the histogram
        self.hist_npts = QtWidgets.QLineEdit(self)
        self.hist_npts.setMinimumWidth(100)
        self.hist_npts.setValidator(Qt.QIntValidator(0, 8191))
        self.hist_npts.setText("{0}".format(self.snk.nsamps()))
        self.layout.addRow("Number of Points:", self.hist_npts)
        self.hist_npts.editingFinished.connect(self.set_nsamps)

        self.hist_bins = QtWidgets.QLineEdit(self)
        self.hist_bins.setMinimumWidth(100)
        self.hist_bins.setValidator(Qt.QIntValidator(0, 1000))
        self.hist_bins.setText("{0}".format(self.snk.bins()))
        self.layout.addRow("Number of Bins:", self.hist_bins)
        self.hist_bins.editingFinished.connect(self.set_bins)

        self.hist_auto = QtWidgets.QPushButton("scale", self)
        self.layout.addRow("Autoscale X:", self.hist_auto)
        self.hist_auto.pressed.connect(self.autoscalex)

        self.quit = QtWidgets.QPushButton('Close', self)
        self.quit.setMinimumWidth(100)
        self.layout.addWidget(self.quit)

        self.quit.clicked.connect(QtWidgets.qApp.quit)


    def attach_signal1(self, signal):
        self.signal1 = signal
        self.freq1Edit.setText(("{0}").format(self.signal1.frequency()))
        self.amp1Edit.setText(("{0}").format(self.signal1.amplitude()))

    def attach_signal2(self, signal):
        self.signal2 = signal
        self.amp2Edit.setText(("{0}").format(self.signal2.amplitude()))

    def freq1EditText(self):
        try:
            newfreq = float(self.freq1Edit.text())
            self.signal1.set_frequency(newfreq)
        except ValueError:
            print("Bad frequency value entered")

    def amp1EditText(self):
        try:
            newamp = float(self.amp1Edit.text())
            self.signal1.set_amplitude(newamp)
        except ValueError:
            print("Bad amplitude value entered")

    def amp2EditText(self):
        try:
            newamp = float(self.amp2Edit.text())
            self.signal2.set_amplitude(newamp)
        except ValueError:
            print("Bad amplitude value entered")

    def set_nsamps(self):
        res = self.hist_npts.text().toInt()
        if(res[1]):
            self.snk.set_nsamps(res[0])

    def set_bins(self):
        res = self.hist_bins.text().toInt()
        if(res[1]):
            self.snk.set_bins(res[0])

    def autoscalex(self):
        self.snk.autoscalex()


class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        Rs = 8000
        f1 = 100

        npts = 2048

        self.qapp = QtWidgets.QApplication(sys.argv)

        src1 = analog.sig_source_f(Rs, analog.GR_SIN_WAVE, f1, 0, 0)
        src2 = analog.noise_source_f(analog.GR_GAUSSIAN, 1)
        src  = blocks.add_ff()
        thr = blocks.throttle(gr.sizeof_float, 100*npts)
        self.snk1 = qtgui.histogram_sink_f(npts, 200, -5, 5,
                                           "Histogram")
        self.snk1.disable_legend()

        self.connect(src1, (src,0))
        self.connect(src2, (src,1))
        self.connect(src, thr, self.snk1)

        self.ctrl_win = control_box(self.snk1)
        self.ctrl_win.attach_signal1(src1)
        self.ctrl_win.attach_signal2(src2)

        # Get the reference pointer to the SpectrumDisplayForm QWidget
        pyQt  = self.snk1.pyqwidget()

        # Wrap the pointer as a PyQt SIP object
        # This can now be manipulated as a PyQt5.QtWidgets.QWidget
        pyWin = sip.wrapinstance(pyQt, QtWidgets.QWidget)

        #pyWin.show()
        self.main_box = dialog_box(pyWin, self.ctrl_win)
        self.main_box.show()

if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    tb.qapp.exec_()
    tb.stop()
