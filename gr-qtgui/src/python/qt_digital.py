#!/usr/bin/env python

from gnuradio import gr, blks2
from gnuradio.qtgui import qtgui
from PyQt4 import QtGui, QtCore
import sys, sip
import scipy

try:
    from qt_digital_window import Ui_DigitalWindow
except ImportError:
    print "Error: could not find qt_digital_window.py:"
    print "\t\"pyuic4 qt_digital_window.ui -o qt_digital_window.py\""
    sys.exit(1)

class dialog_box(QtGui.QMainWindow):
    def __init__(self, snkTx, snkRx, channel, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.gui = Ui_DigitalWindow()
        self.gui.setupUi(self)

        self.channel = channel

        # Add the qtsnk widgets to the hlayout box
        self.gui.sinkLayout.addWidget(snkTx)
        self.gui.sinkLayout.addWidget(snkRx)

        # Connect up some signals
        self.connect(self.gui.noiseEdit, QtCore.SIGNAL("editingFinished()"),
                     self.noiseEditText)
        self.connect(self.gui.freqEdit, QtCore.SIGNAL("editingFinished()"),
                     self.freqEditText)
        self.connect(self.gui.timeEdit, QtCore.SIGNAL("editingFinished()"),
                     self.timeEditText)
        
    def set_noise(self, noise):
        self.noise = noise
        self.gui.noiseEdit.setText(QtCore.QString("%1").arg(self.noise))

    def set_frequency(self, freq):
        self.freq = freq
        self.gui.freqEdit.setText(QtCore.QString("%1").arg(self.freq))

    def set_time_offset(self, to):
        self.timing_offset = to
        self.gui.timeEdit.setText(QtCore.QString("%1").arg(self.timing_offset))

    def noiseEditText(self):
        try:
            noise = self.gui.noiseEdit.text().toDouble()[0]
            self.channel.set_noise_voltage(noise)

            self.noise = noise
        except RuntimeError:
            pass

    def freqEditText(self):
        try:
            freq = self.gui.freqEdit.text().toDouble()[0]
            self.channel.set_frequency_offset(freq)

            self.freq = freq
        except RuntimeError:
            pass

    def timeEditText(self):
        try:
            to = self.gui.timeEdit.text().toDouble()[0]
            self.channel.set_timing_offset(to)

            self.timing_offset = to
        except RuntimeError:
            pass


class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        self.qapp = QtGui.QApplication(sys.argv)

        sps = 2
        excess_bw = 0.35
        gray_code = True
        
        fftsize = 2048

        data = scipy.random.randint(0, 255, 1000)
        src = gr.vector_source_b(data, True)
        mod = blks2.dqpsk_mod(sps, excess_bw, gray_code, False, False)

        rrctaps = gr.firdes.root_raised_cosine(1, sps, 1, excess_bw, 21)
        rx_rrc = gr.fir_filter_ccf(sps, rrctaps)

        noise = 1e-7
        fo = 1e-6
        to = 1.0
        channel = gr.channel_model(noise, fo, to)

        thr = gr.throttle(gr.sizeof_gr_complex, 10*fftsize)
        self.snk_tx = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS, -1/2, 1/2,
                                   "Tx", True, True, False, True, True)

        self.snk_rx = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS, -1/2, 1/2,
                                   "Rx", True, True, False, True, True)

        self.connect(src, mod, channel, self.snk_tx)
        self.connect(channel, rx_rrc, thr, self.snk_rx)
        
        pyTxQt  = self.snk_tx.pyqwidget()
        pyTx = sip.wrapinstance(pyTxQt, QtGui.QWidget)

        pyRxQt  = self.snk_rx.pyqwidget()
        pyRx = sip.wrapinstance(pyRxQt, QtGui.QWidget)

        self.main_box = dialog_box(pyTx, pyRx, channel);
        self.main_box.set_noise(noise)
        self.main_box.set_frequency(fo)
        self.main_box.set_time_offset(to)
        self.main_box.show()


    
if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    tb.qapp.exec_()
