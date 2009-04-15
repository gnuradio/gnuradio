#!/usr/bin/env python
#
# Copyright 2009 Free Software Foundation, Inc.
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

from gnuradio import gr, gru
from gnuradio import usrp2
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from gnuradio.qtgui import qtgui
from optparse import OptionParser
from PyQt4 import QtGui, QtCore
import sys, sip

class dialog_box(QtGui.QWidget):
    def __init__(self, display, control):
        QtGui.QWidget.__init__(self, None)
        self.setWindowTitle('USRP2 Display')

        self.boxlayout = QtGui.QBoxLayout(QtGui.QBoxLayout.LeftToRight, self)
        self.boxlayout.addWidget(display, 1)
        self.boxlayout.addWidget(control)

        self.resize(800, 500)

class control_panel(QtGui.QWidget):
    def __init__(self, usrp, qtsink, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.setWindowTitle('USRP2 Control Panel')

        self.usrp = usrp
        self.qtsink = qtsink
        self.adc_rate = self.usrp.adc_rate()

        self.freq = 0
        self.decim = 0
        self.bw = 0
        self.gain = 0
         
        self.setToolTip('Set the values of the USRP2')
        QtGui.QToolTip.setFont(QtGui.QFont('OldEnglish', 10))

        self.layout = QtGui.QFormLayout(self)

        # Received frequency
        self.freqEdit = QtGui.QLineEdit(self)
        self.layout.addRow("Frequency:", self.freqEdit)
        self.connect(self.freqEdit, QtCore.SIGNAL("editingFinished()"),
                     self.freqEditText)

        # Receiver gain
        self.gainEdit = QtGui.QLineEdit(self)
        self.layout.addRow("Gain:", self.gainEdit)
        self.connect(self.gainEdit, QtCore.SIGNAL("editingFinished()"),
                     self.gainEditText)


        # Decim / Bandwidth
        self.decimEdit = QtGui.QLineEdit(self)
        self.layout.addRow("Decim Rate:", self.decimEdit)
        self.connect(self.decimEdit, QtCore.SIGNAL("editingFinished()"),
                     self.decimEditText)

        self.quit = QtGui.QPushButton('Close', self)
        self.layout.addRow(self.quit)

        self.connect(self.quit, QtCore.SIGNAL('clicked()'),
                     QtGui.qApp, QtCore.SLOT('quit()'))

    def set_frequency(self, freq):
        self.freq = freq
        sfreq = eng_notation.num_to_str(self.freq)
        self.freqEdit.setText(QtCore.QString("%1").arg(sfreq))
        
    def set_gain(self, gain):
        self.gain = gain
        self.gainEdit.setText(QtCore.QString("%1").arg(self.gain))

    def set_decim(self, decim):
        self.decim = decim
        self.bw = self.adc_rate / float(self.decim) / 1000.0
        self.decimEdit.setText(QtCore.QString("%1").arg(self.decim))

    def freqEditText(self):
        try:
            freq = eng_notation.str_to_num(self.freqEdit.text().toAscii())
            r = self.usrp.set_center_freq(freq)
            self.freq = freq
            self.qtsink.set_frequency_range(self.freq, self.freq-self.bw/2.0, self.freq+self.bw/2.0)
        except RuntimeError:
            pass

        #self.set_frequency(self.freq)

    def gainEditText(self):
        try:
            gain = float(self.gainEdit.text())
            self.usrp.set_gain(gain)
            self.gain = gain
        except ValueError:
            pass
        
        #self.set_gain(gain)
        
    def decimEditText(self):
        try:
            decim = int(self.decimEdit.text())
            self.usrp.set_decim(decim)

            self.decim = decim
            self.bw = self.adc_rate / self.decim
            self.qtsink.set_frequency_range(-self.bw/2.0, self.bw/2.0, self.freq)           
            
        except ValueError:
            pass

        #self.set_decim(decim)
        
class app_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        parser = OptionParser(option_class=eng_option)
        parser.add_option("-e", "--interface", type="string", default="eth0",
                          help="select Ethernet interface, default is eth0")
        parser.add_option("-m", "--mac-addr", type="string", default="",
                          help="select USRP by MAC address, default is auto-select")
        parser.add_option("-d", "--decim", type="int", default=16,
                          help="set fgpa decimation rate to DECIM [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("--fft-size", type="int", default=1024,
                          help="Set number of FFT bins [default=%default]")
        (options, args) = parser.parse_args()

        if len(args) != 0:
            parser.print_help()
            sys.exit(1)
	self.options = options
        self.show_debug_info = True
        
        self.qapp = QtGui.QApplication(sys.argv)

        self.u = usrp2.source_32fc(options.interface, options.mac_addr)
        self.u.set_decim(options.decim)
        
        input_rate = self.u.adc_rate() / self.u.decim()
        
        self.snk = qtgui.sink_c(options.fft_size, gr.firdes.WIN_BLACKMAN_hARRIS,
                                -input_rate/2, input_rate/2,
                                "USRP2 Display",
                                True, True, False, True, False)

        self.connect(self.u, self.snk)

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.gain_range()
            options.gain = float(g[0]+g[1])/2

        if options.freq is None:
            # if no freq was specified, use the mid-point
            r = self.u.freq_range()
            options.freq = float(r[0]+r[1])/2
            
        self.set_gain(options.gain)
        r = self.u.set_center_freq(options.freq)

        if self.show_debug_info:
            print "Decimation rate: ", self.u.decim()
            print "Bandwidth: ", input_rate
            print "D'board: ", self.u.daughterboard_id()


        self.ctrl_win = control_panel(self.u, self.snk)

        self.ctrl_win.set_frequency(options.freq)
        self.ctrl_win.set_gain(options.gain)
        self.ctrl_win.set_decim(options.decim)

        # Get the reference pointer to the SpectrumDisplayForm QWidget
        pyQt  = self.snk.pyqwidget()

        # Wrap the pointer as a PyQt SIP object
        # This can now be manipulated as a PyQt4.QtGui.QWidget
        pyWin = sip.wrapinstance(pyQt, QtGui.QWidget)

        self.main_box = dialog_box(pyWin, self.ctrl_win)

        self.main_box.show()

    def set_gain(self, gain):
        self.u.set_gain(gain)

    def set_decim(self, decim):
        ok = self.u.set_decim(decim)
        if not ok:
            print "set_decim failed"
        input_rate = self.u.adc_rate() / self.u.decim()
        return ok

def main ():
    tb = app_top_block()
    tb.start()
    tb.snk.exec_();

if __name__ == '__main__':
    try:
        main ()
    except KeyboardInterrupt:
        pass
    
