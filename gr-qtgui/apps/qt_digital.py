#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
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

from gnuradio import gr, blks2
from gnuradio import eng_notation
import sys

try:
    from gnuradio import qtgui
    from PyQt4 import QtGui, QtCore
    import sip
except ImportError:
    print "Error: Program requires PyQt4 and gr-qtgui."
    sys.exit(1)

try:
    import scipy
except ImportError:
    print "Error: Program requires scipy (see: www.scipy.org)."
    sys.exit(1)

try:
    from qt_digital_window import Ui_DigitalWindow
except ImportError:
    print "Error: could not find qt_digital_window.py:"
    print "\t\"Please run: pyuic4 qt_digital_window.ui -o qt_digital_window.py\""
    sys.exit(1)

class dialog_box(QtGui.QMainWindow):
    def __init__(self, snkTx, snkRx, fg, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.gui = Ui_DigitalWindow()
        self.gui.setupUi(self)

        self.fg = fg

        self.set_sample_rate(self.fg.sample_rate())

        self.set_snr(self.fg.snr())
        self.set_frequency(self.fg.frequency_offset())
        self.set_time_offset(self.fg.timing_offset())

        self.set_gain_mu(self.fg.rx_gain_mu())
        self.set_alpha(self.fg.rx_alpha())

        # Add the qtsnk widgets to the hlayout box
        self.gui.sinkLayout.addWidget(snkTx)
        self.gui.sinkLayout.addWidget(snkRx)


        # Connect up some signals
        self.connect(self.gui.pauseButton, QtCore.SIGNAL("clicked()"),
                     self.pauseFg)

        self.connect(self.gui.sampleRateEdit, QtCore.SIGNAL("editingFinished()"),
                     self.sampleRateEditText)

        self.connect(self.gui.snrEdit, QtCore.SIGNAL("editingFinished()"),
                     self.snrEditText)
        self.connect(self.gui.freqEdit, QtCore.SIGNAL("editingFinished()"),
                     self.freqEditText)
        self.connect(self.gui.timeEdit, QtCore.SIGNAL("editingFinished()"),
                     self.timeEditText)

        self.connect(self.gui.gainMuEdit, QtCore.SIGNAL("editingFinished()"),
                     self.gainMuEditText)
        self.connect(self.gui.alphaEdit, QtCore.SIGNAL("editingFinished()"),
                     self.alphaEditText)


    def pauseFg(self):
        if(self.gui.pauseButton.text() == "Pause"):
            self.fg.stop()
            self.fg.wait()
            self.gui.pauseButton.setText("Unpause")
        else:
            self.fg.start()
            self.gui.pauseButton.setText("Pause")

    # Accessor functions for Gui to manipulate system parameters
    def set_sample_rate(self, sr):
        ssr = eng_notation.num_to_str(sr)
        self.gui.sampleRateEdit.setText(QtCore.QString("%1").arg(ssr))

    def sampleRateEditText(self):
        try:
            rate = self.gui.sampleRateEdit.text().toAscii()
            srate = eng_notation.str_to_num(rate)
            self.fg.set_sample_rate(srate)
        except RuntimeError:
            pass


    # Accessor functions for Gui to manipulate channel model
    def set_snr(self, snr):
        self.gui.snrEdit.setText(QtCore.QString("%1").arg(snr))

    def set_frequency(self, fo):
        self.gui.freqEdit.setText(QtCore.QString("%1").arg(fo))

    def set_time_offset(self, to):
        self.gui.timeEdit.setText(QtCore.QString("%1").arg(to))

    def snrEditText(self):
        try:
            snr = self.gui.snrEdit.text().toDouble()[0]
            self.fg.set_snr(snr)
        except RuntimeError:
            pass

    def freqEditText(self):
        try:
            freq = self.gui.freqEdit.text().toDouble()[0]
            self.fg.set_frequency_offset(freq)
        except RuntimeError:
            pass

    def timeEditText(self):
        try:
            to = self.gui.timeEdit.text().toDouble()[0]
            self.fg.set_timing_offset(to)
        except RuntimeError:
            pass


    # Accessor functions for Gui to manipulate receiver parameters
    def set_gain_mu(self, gain):
        self.gui.gainMuEdit.setText(QtCore.QString("%1").arg(gain))

    def set_alpha(self, alpha):
        self.gui.alphaEdit.setText(QtCore.QString("%1").arg(alpha))

    def alphaEditText(self):
        try:
            alpha = self.gui.alphaEdit.text().toDouble()[0]
            self.fg.set_rx_alpha(alpha)
        except RuntimeError:
            pass

    def gainMuEditText(self):
        try:
            gain = self.gui.gainMuEdit.text().toDouble()[0]
            self.fg.set_rx_gain_mu(gain)
        except RuntimeError:
            pass


class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        self.qapp = QtGui.QApplication(sys.argv)

        self._sample_rate = 2000e3

        self.sps = 2
        self.excess_bw = 0.35
        self.gray_code = True
        
        fftsize = 2048

        self.data = scipy.random.randint(0, 255, 1000)
        self.src = gr.vector_source_b(self.data.tolist(), True)
        self.mod = blks2.dqpsk_mod(self.sps, self.excess_bw, self.gray_code, False, False)

        self.rrctaps = gr.firdes.root_raised_cosine(1, self.sps, 1, self.excess_bw, 21)
        self.rx_rrc = gr.fir_filter_ccf(1, self.rrctaps)


        # Set up the carrier & clock recovery parameters
        self.arity = 4
        self.mu = 0.5
        self.gain_mu = 0.05
        self.omega = self.sps
        self.gain_omega = .25 * self.gain_mu * self.gain_mu
        self.omega_rel_lim = 0.05
        
        self.alpha = 0.15
        self.beta  = 0.25 * self.alpha * self.alpha
        self.fmin = -1000/self.sample_rate()
        self.fmax = 1000/self.sample_rate()
        
        self.receiver = gr.mpsk_receiver_cc(self.arity, 0,
                                            self.alpha, self.beta,
                                            self.fmin, self.fmax,
                                            self.mu, self.gain_mu,
                                            self.omega, self.gain_omega,
                                            self.omega_rel_lim)
        
        
        self.snr_dB = 15
        noise = self.get_noise_voltage(self.snr_dB)
        self.fo = 100/self.sample_rate()
        self.to = 1.0
        self.channel = gr.channel_model(noise, self.fo, self.to)

        self.thr = gr.throttle(gr.sizeof_char, self._sample_rate)
        self.snk_tx = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS, 
                                   0, self._sample_rate*self.sps,
                                   "Tx", True, True, True, True)

        self.snk_rx = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS,
                                   0, self._sample_rate,
                                   "Rx", True, True, True, True)

        self.connect(self.src, self.thr, self.mod, self.channel, self.snk_tx)
        self.connect(self.channel, self.rx_rrc, self.receiver, self.snk_rx)
        
        pyTxQt  = self.snk_tx.pyqwidget()
        pyTx = sip.wrapinstance(pyTxQt, QtGui.QWidget)

        pyRxQt  = self.snk_rx.pyqwidget()
        pyRx = sip.wrapinstance(pyRxQt, QtGui.QWidget)

        self.main_box = dialog_box(pyTx, pyRx, self);
        self.main_box.show()


    def get_noise_voltage(self, SNR):
        S = 0                            # dBm, assuming signal power normalized
        N = S - SNR                      # dBm
        npwr = pow(10.0, N/10.0)         # ratio
        nv = scipy.sqrt(npwr * self.sps) # convert the noise voltage
        return nv


    # System Parameters
    def sample_rate(self):
        return self._sample_rate
    
    def set_sample_rate(self, sr):
        self._sample_rate = sr


    # Channel Model Parameters
    def snr(self):
        return self.snr_dB
    
    def set_snr(self, snr):
        self.snr_dB = snr
        noise = self.get_noise_voltage(self.snr_dB)
        self.channel.set_noise_voltage(noise)

    def frequency_offset(self):
        return self.fo * self.sample_rate()

    def set_frequency_offset(self, fo):
        self.fo = fo / self.sample_rate()
        self.channel.set_frequency_offset(self.fo)

    def timing_offset(self):
        return self.to
    
    def set_timing_offset(self, to):
        self.to = to
        self.channel.set_timing_offset(self.to)


    # Receiver Parameters
    def rx_gain_mu(self):
        return self.gain_mu

    def rx_gain_omega(self):
        return self.gain_omega
    
    def set_rx_gain_mu(self, gain):
        self.gain_mu = gain
        self.gain_omega = .25 * self.gain_mu * self.gain_mu
        self.receiver.set_gain_mu(self.gain_mu)
        self.receiver.set_gain_omega(self.gain_omega)

    def rx_alpha(self):
        return self.alpha

    def rx_beta(self):
        return self.beta
    
    def set_rx_alpha(self, alpha):
        self.alpha = alpha
        self.beta = .25 * self.alpha * self.alpha
        self.receiver.set_alpha(self.alpha)
        self.receiver.set_beta(self.beta)

    
if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    tb.qapp.exec_()
    tb.stop()
