#!/usr/bin/env python
#
# Copyright 2012,2015 Free Software Foundation, Inc.
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
from gnuradio import gr, filter
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

try:
    from gnuradio import channels
except ImportError:
    sys.stderr.write("Error: Program requires gr-channels.\n")
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
    def __init__(self, parent=None):
        QtWidgets.QWidget.__init__(self, parent)
        self.setWindowTitle('Control Panel')

        self.setToolTip('Control the signals')
        QtWidgets.QToolTip.setFont(Qt.QFont('OldEnglish', 10))

        self.layout = QtWidgets.QFormLayout(self)

        # Control the first signal
        self.freq1Edit = QtWidgets.QLineEdit(self)
        self.freq1Edit.setMinimumWidth(100)
        self.layout.addRow("Signal 1 Frequency:", self.freq1Edit)
        self.freq1Edit.editingFinished.connect(self.freq1EditText)

        self.amp1Edit = QtWidgets.QLineEdit(self)
        self.amp1Edit.setMinimumWidth(100)
        self.layout.addRow("Signal 1 Amplitude:", self.amp1Edit)
        self.amp1Edit.editingFinished.connect(self.amp1EditText)


        # Control the second signal
        self.freq2Edit = QtWidgets.QLineEdit(self)
        self.freq2Edit.setMinimumWidth(100)
        self.layout.addRow("Signal 2 Frequency:", self.freq2Edit)
        self.freq2Edit.editingFinished.connect(self.freq2EditText)


        self.amp2Edit = QtWidgets.QLineEdit(self)
        self.amp2Edit.setMinimumWidth(100)
        self.layout.addRow("Signal 2 Amplitude:", self.amp2Edit)
        self.amp2Edit.editingFinished.connect(self.amp2EditText)

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
        self.freq2Edit.setText(("{0}").format(self.signal2.frequency()))
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


    def freq2EditText(self):
        try:
            newfreq = float(self.freq2Edit.text())
            self.signal2.set_frequency(newfreq)
        except ValueError:
            print("Bad frequency value entered")

    def amp2EditText(self):
        try:
            newamp = float(self.amp2Edit.text())
            self.signal2.set_amplitude(newamp)
        except ValueError:
            print("Bad amplitude value entered")


class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        Rs = 8000
        f1 = 100
        f2 = 200

        npts = 2048

        self.qapp = QtWidgets.QApplication(sys.argv)
        ss = open(gr.prefix() + '/share/gnuradio/themes/dark.qss')
        sstext = ss.read()
        ss.close()
        self.qapp.setStyleSheet(sstext)

        src1 = analog.sig_source_c(Rs, analog.GR_SIN_WAVE, f1, 0.1, 0)
        src2 = analog.sig_source_c(Rs, analog.GR_SIN_WAVE, f2, 0.1, 0)
        src  = blocks.add_cc()
        channel = channels.channel_model(0.01)
        thr = blocks.throttle(gr.sizeof_gr_complex, 100*npts)
        self.snk1 = qtgui.freq_sink_c(npts, filter.firdes.WIN_BLACKMAN_hARRIS,
                                      0, Rs,
                                      "Complex Freq Example", 3)

        self.connect(src1, (src,0))
        self.connect(src2, (src,1))
        self.connect(src,  channel, thr, (self.snk1, 0))
        self.connect(src1,  (self.snk1, 1))
        self.connect(src2,  (self.snk1, 2))

        self.ctrl_win = control_box()
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
