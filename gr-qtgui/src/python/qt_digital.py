#!/usr/bin/env python

from gnuradio import gr, blks2
from gnuradio.qtgui import qtgui
from PyQt4 import QtGui, QtCore
import sys, sip
import scipy

class dialog_box(QtGui.QWidget):
    def __init__(self, display_tx, display_rx, channel):
        QtGui.QWidget.__init__(self, None)
        self.setWindowTitle('Digital Signal Examples')

        self.control = control_panel(channel, self)

        hlayout = QtGui.QHBoxLayout()
        hlayout.addWidget(display_tx)
        hlayout.addWidget(display_rx)
        hlayout.setGeometry(QtCore.QRect(0,0,100,100))

        vlayout = QtGui.QVBoxLayout()
        vlayout.addLayout(hlayout)
        vlayout.addLayout(self.control.layout, -1)
        #vlayout.addStretch(-1)

        self.setLayout(vlayout)
        self.resize(1000, 1000)

class control_panel(QtGui.QWidget):
    def __init__(self, channel, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.setWindowTitle('Control Panel')

        self.channel = channel
        
        self.layout = QtGui.QFormLayout()

        # Set channel noise
        self.noiseEdit = QtGui.QLineEdit(self)
        self.layout.addRow("Noise Amplitude:", self.noiseEdit)
        self.connect(self.noiseEdit, QtCore.SIGNAL("editingFinished()"),
                     self.noiseEditText)

        # Set channel frequency offset
        self.freqEdit = QtGui.QLineEdit(self)
        self.layout.addRow("Frequency Offset:", self.freqEdit)
        self.connect(self.freqEdit, QtCore.SIGNAL("editingFinished()"),
                     self.freqEditText)

        # Set channel timing offset
        self.timeEdit = QtGui.QLineEdit(self)
        self.layout.addRow("Timing Offset:", self.timeEdit)
        self.connect(self.timeEdit, QtCore.SIGNAL("editingFinished()"),
                     self.timeEditText)

        self.quit = QtGui.QPushButton('Close', self)
        self.layout.addRow(self.quit)

        self.connect(self.quit, QtCore.SIGNAL('clicked()'),
                     QtGui.qApp, QtCore.SLOT('quit()'))

    def set_noise(self, noise):
        self.noise = noise
        self.noiseEdit.setText(QtCore.QString("%1").arg(self.noise))

    def set_frequency(self, freq):
        self.freq = freq
        self.freqEdit.setText(QtCore.QString("%1").arg(self.freq))

    def set_time_offset(self, to):
        self.timing_offset = to
        self.timeEdit.setText(QtCore.QString("%1").arg(self.timing_offset))

    def noiseEditText(self):
        try:
            noise = self.noiseEdit.text().toDouble()[0]
            self.channel.noise.set_amplitude(noise)

            self.noise = noise
        except RuntimeError:
            pass

    def freqEditText(self):
        try:
            freq = self.freqEdit.text().toDouble()[0]
            self.channel.freq_offset.set_frequency(freq)

            self.freq = freq
        except RuntimeError:
            pass

    def timeEditText(self):
        try:
            to = self.timeEdit.text().toDouble()[0]
            self.channel.timing_offset.set_interp_ratio(to)

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
        channel = blks2.channel_model(noise, fo, to)

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

        self.main_box = dialog_box(pyTx, pyRx, channel)
        self.main_box.control.set_noise(noise)
        self.main_box.control.set_frequency(fo)
        self.main_box.control.set_time_offset(to)

        self.main_box.show()
    
if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    tb.qapp.exec_()
