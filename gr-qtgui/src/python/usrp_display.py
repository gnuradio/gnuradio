#!/usr/bin/env python

from gnuradio import gr
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from gnuradio.qtgui import qtgui
from optparse import OptionParser
from PyQt4 import QtGui, QtCore
import sys, sip

class dialog_box(QtGui.QWidget):
    def __init__(self, display, control):
        QtGui.QWidget.__init__(self, None)
        self.setWindowTitle('USRP FFT')

        self.boxlayout = QtGui.QBoxLayout(QtGui.QBoxLayout.LeftToRight, self)
        self.boxlayout.addWidget(display, 1)
        self.boxlayout.addWidget(control)

        self.resize(800, 500)

class control_panel(QtGui.QWidget):
    def __init__(self, usrp, subdev, qtsink, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.setWindowTitle('USRP Control Panel')

        self.usrp = usrp
        self.subdev = subdev
        self.qtsink = qtsink
        self.adc_rate = self.usrp.converter_rate()

        self.freq = 0
        self.decim = 0
        self.bw = 0
        self.gain = 0

        self.setToolTip('Set the values of the USRP')
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
            self.usrp.tune(0, self.subdev, freq)
            self.freq = freq
            self.qtsink.set_frequency_range(self.freq, self.freq-self.bw/2.0, self.freq+self.bw/2.0)
        except RuntimeError:
            pass

        #self.set_frequency(self.freq)

    def gainEditText(self):
        try:
            gain = float(self.gainEdit.text())
            self.subdev.set_gain(gain)
            self.gain = gain
        except ValueError:
            pass
        
        #self.set_gain(gain)
        
    def decimEditText(self):
        try:
            decim = int(self.decimEdit.text())
            self.usrp.set_decim_rate(decim)

            self.decim = decim
            self.bw = self.adc_rate / self.decim
            self.qtsink.set_frequency_range(-self.bw/2.0, self.bw/2.0, self.freq)           

        except ValueError:
            pass

        #self.set_decim(decim)
        

class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        parser = OptionParser(option_class=eng_option)
        parser.add_option("-w", "--which", type="int", default=0,
                          help="select which USRP (0, 1, ...) default is %default",
			  metavar="NUM")
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B (default=first one with a daughterboard)")
        parser.add_option("-A", "--antenna", default=None,
                          help="select Rx Antenna (only on RFX-series boards)")
        parser.add_option("-d", "--decim", type="int", default=16,
                          help="set fgpa decimation rate to DECIM [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB [default is midpoint]")
        parser.add_option("-W", "--waterfall", action="store_true", default=False,
                          help="Enable waterfall display")
        parser.add_option("-8", "--width-8", action="store_true", default=False,
                          help="Enable 8-bit samples across USB")
        parser.add_option( "--no-hb", action="store_true", default=False,
                          help="don't use halfband filter in usrp")
        parser.add_option("-S", "--oscilloscope", action="store_true", default=False,
                          help="Enable oscilloscope display")
	parser.add_option("", "--avg-alpha", type="eng_float", default=1e-1,
			  help="Set fftsink averaging factor, [default=%default]")
	parser.add_option("", "--ref-scale", type="eng_float", default=13490.0,
			  help="Set dBFS=0dB input value, [default=%default]")
        parser.add_option("", "--fft-size", type="int", default=1024,
                          help="Set FFT frame size, [default=%default]");

        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)
	self.options = options
        self.show_debug_info = True

        # Call this before creating the Qt sink
        self.qapp = QtGui.QApplication(sys.argv)

        self.u = usrp.source_c(which=options.which, decim_rate=options.decim)
        rx_subdev_spec = (0,0)
        self.u.set_mux(usrp.determine_rx_mux_value(self.u, rx_subdev_spec))
        self.subdev = usrp.selected_subdev(self.u, rx_subdev_spec)

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.subdev.gain_range()
            options.gain = float(g[0]+g[1])/2
        self.subdev.set_gain(options.gain)

        if options.freq is None:
            # if no frequency was specified, use the mid-point of the subdev
            f = self.subdev.freq_range()
            options.freq = float(f[0]+f[1])/2
        self.u.tune(0, self.subdev, options.freq)

        fftsize = 2048
        input_rate = self.u.converter_rate() / self.u.decim_rate()
        self.snk = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS,
                                -input_rate/2, input_rate/2,
                                "USRP Display",
                                True, True, False, True, False)

        amp = gr.multiply_const_cc(0.001)
        self.connect(self.u, amp, self.snk)

        self.ctrl_win = control_panel(self.u, self.subdev, self.snk)

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
       
if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    tb.qapp.exec_()
