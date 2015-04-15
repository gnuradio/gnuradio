#!/usr/bin/env python
#
# Copyright 2011,2012 Free Software Foundation, Inc.
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

from gnuradio import gr
from gnuradio import blocks
import sys

try:
    from gnuradio import qtgui
    from PyQt4 import QtGui, QtCore
    import sip
except ImportError:
    sys.stderr.write("Error: Program requires PyQt4 and gr-qtgui.\n")
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

class dialog_box(QtGui.QWidget):
    def __init__(self, display, control):
        QtGui.QWidget.__init__(self, None)
        self.setWindowTitle('PyQt Test GUI')

        self.boxlayout = QtGui.QBoxLayout(QtGui.QBoxLayout.LeftToRight, self)
        self.boxlayout.addWidget(display, 1)
        self.boxlayout.addWidget(control)

        self.resize(800, 500)

class control_box(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.setWindowTitle('Control Panel')

        self.setToolTip('Control the signals')
        QtGui.QToolTip.setFont(QtGui.QFont('OldEnglish', 10))

        self.layout = QtGui.QFormLayout(self)

        # Control the first signal
        self.freq1Edit = QtGui.QLineEdit(self)
        self.freq1Edit.setMinimumWidth(100)
        self.layout.addRow("Signal 1 Frequency:", self.freq1Edit)
        self.connect(self.freq1Edit, QtCore.SIGNAL("editingFinished()"),
                     self.freq1EditText)

        self.amp1Edit = QtGui.QLineEdit(self)
        self.amp1Edit.setMinimumWidth(100)
        self.layout.addRow("Signal 1 Amplitude:", self.amp1Edit)
        self.connect(self.amp1Edit, QtCore.SIGNAL("editingFinished()"),
                     self.amp1EditText)


        # Control the second signal
        self.freq2Edit = QtGui.QLineEdit(self)
        self.freq2Edit.setMinimumWidth(100)
        self.layout.addRow("Signal 2 Frequency:", self.freq2Edit)
        self.connect(self.freq2Edit, QtCore.SIGNAL("editingFinished()"),
                     self.freq2EditText)


        self.amp2Edit = QtGui.QLineEdit(self)
        self.amp2Edit.setMinimumWidth(100)
        self.layout.addRow("Signal 2 Amplitude:", self.amp2Edit)
        self.connect(self.amp2Edit, QtCore.SIGNAL("editingFinished()"),
                     self.amp2EditText)

        self.quit = QtGui.QPushButton('Close', self)
        self.quit.setMinimumWidth(100)
        self.layout.addWidget(self.quit)

        self.connect(self.quit, QtCore.SIGNAL('clicked()'),
                     QtGui.qApp, QtCore.SLOT('quit()'))

    def attach_signal1(self, signal):
        self.signal1 = signal
        self.freq1Edit.setText(QtCore.QString("%1").arg(self.signal1.frequency()))
        self.amp1Edit.setText(QtCore.QString("%1").arg(self.signal1.amplitude()))

    def attach_signal2(self, signal):
        self.signal2 = signal
        self.freq2Edit.setText(QtCore.QString("%1").arg(self.signal2.frequency()))
        self.amp2Edit.setText(QtCore.QString("%1").arg(self.signal2.amplitude()))

    def freq1EditText(self):
        try:
            newfreq = float(self.freq1Edit.text())
            self.signal1.set_frequency(newfreq)
        except ValueError:
            print "Bad frequency value entered"

    def amp1EditText(self):
        try:
            newamp = float(self.amp1Edit.text())
            self.signal1.set_amplitude(newamp)
        except ValueError:
            print "Bad amplitude value entered"


    def freq2EditText(self):
        try:
            newfreq = float(self.freq2Edit.text())
            self.signal2.set_frequency(newfreq)
        except ValueError:
            print "Bad frequency value entered"

    def amp2EditText(self):
        try:
            newamp = float(self.amp2Edit.text())
            self.signal2.set_amplitude(newamp)
        except ValueError:
            print "Bad amplitude value entered"


class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        Rs = 8000
        f1 = 100
        f2 = 200

        npts = 2048

        self.qapp = QtGui.QApplication(sys.argv)
        ss = open(gr.prefix() + '/share/gnuradio/themes/dark.qss')
        sstext = ss.read()
        ss.close()
        self.qapp.setStyleSheet(sstext)

        src1 = analog.sig_source_c(Rs, analog.GR_SIN_WAVE, f1, 0.1, 0)
        src2 = analog.sig_source_c(Rs, analog.GR_SIN_WAVE, f2, 0.1, 0)
        src  = blocks.add_cc()
        channel = channels.channel_model(0.01)
        thr = blocks.throttle(gr.sizeof_gr_complex, 100*npts)
        self.snk1 = qtgui.time_sink_c(npts, Rs,
                                      "Complex Time Example", 1)

        self.connect(src1, (src,0))
        self.connect(src2, (src,1))
        self.connect(src,  channel, thr, (self.snk1, 0))
        #self.connect(src1, (self.snk1, 1))
        #self.connect(src2, (self.snk1, 2))

        self.ctrl_win = control_box()
        self.ctrl_win.attach_signal1(src1)
        self.ctrl_win.attach_signal2(src2)

        # Get the reference pointer to the SpectrumDisplayForm QWidget
        pyQt  = self.snk1.pyqwidget()

        # Wrap the pointer as a PyQt SIP object
        # This can now be manipulated as a PyQt4.QtGui.QWidget
        pyWin = sip.wrapinstance(pyQt, QtGui.QWidget)

        # Example of using signal/slot to set the title of a curve
        pyWin.connect(pyWin, QtCore.SIGNAL("setLineLabel(int, QString)"),
                      pyWin, QtCore.SLOT("setLineLabel(int, QString)"))
        pyWin.emit(QtCore.SIGNAL("setLineLabel(int, QString)"), 0, "Re{sum}")
        self.snk1.set_line_label(1, "Im{Sum}")
        #self.snk1.set_line_label(2, "Re{src1}")
        #self.snk1.set_line_label(3, "Im{src1}")
        #self.snk1.set_line_label(4, "Re{src2}")
        #self.snk1.set_line_label(5, "Im{src2}")

        # Can also set the color of a curve
        #self.snk1.set_color(5, "blue")

        self.snk1.set_update_time(0.5)

        #pyWin.show()
        self.main_box = dialog_box(pyWin, self.ctrl_win)
        self.main_box.show()

if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    tb.qapp.exec_()
    tb.stop()
