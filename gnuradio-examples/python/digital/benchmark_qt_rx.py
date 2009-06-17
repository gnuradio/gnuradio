#!/usr/bin/env python
#
# Copyright 2005,2006,2007,2009 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, modulation_utils
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import random
import struct
import sys

# from current dir
from receive_path import receive_path
from pick_bitrate import pick_rx_bitrate
import usrp_options

try:
    from gnuradio.qtgui import qtgui
    from PyQt4 import QtGui, QtCore
    import sip
except ImportError:
    print "Please install gr-qtgui."
    sys.exit(1)
    
try:
    from qt_rx_window import Ui_DigitalWindow
except ImportError:
    print "Error: could not find qt_rx_window.py:"
    print "\t\"pyuic4 qt_rx_window.ui -o qt_rx_window.py\""
    sys.exit(1)

#import os
#print os.getpid()
#raw_input('Attach and press enter: ')

# ////////////////////////////////////////////////////////////////////
#        Define the QT Interface and Control Dialog
# ////////////////////////////////////////////////////////////////////


class dialog_box(QtGui.QMainWindow):
    def __init__(self, snkRxIn, snkRx, fg, parent=None):

        QtGui.QWidget.__init__(self, parent)
        self.gui = Ui_DigitalWindow()
        self.gui.setupUi(self)

        self.fg = fg

        self.set_frequency(self.fg.frequency())
        self.set_gain(self.fg.gain())
        self.set_decim(self.fg.decim())
        self.set_gain_mu(self.fg.rx_gain_mu())
        self.set_alpha(self.fg.rx_alpha())

        # Add the qtsnk widgets to the hlayout box
        self.gui.sinkLayout.addWidget(snkRxIn)
        self.gui.sinkLayout.addWidget(snkRx)


        # Connect up some signals
        self.connect(self.gui.freqEdit, QtCore.SIGNAL("editingFinished()"),
                     self.freqEditText)
        self.connect(self.gui.gainEdit, QtCore.SIGNAL("editingFinished()"),
                     self.gainEditText)
        self.connect(self.gui.decimEdit, QtCore.SIGNAL("editingFinished()"),
                     self.decimEditText)
        self.connect(self.gui.gainMuEdit, QtCore.SIGNAL("editingFinished()"),
                     self.gainMuEditText)
        self.connect(self.gui.alphaEdit, QtCore.SIGNAL("editingFinished()"),
                     self.alphaEditText)

        # Build a timer to update the packet number and PER fields
        self.update_delay = 250  # time between updating packet rate fields
        self.pkt_timer = QtCore.QTimer(self)
        self.connect(self.pkt_timer, QtCore.SIGNAL("timeout()"),
                     self.updatePacketInfo)
        self.pkt_timer.start(self.update_delay)


    # Accessor functions for Gui to manipulate receiver parameters
    def set_frequency(self, fo):
        self.gui.freqEdit.setText(QtCore.QString("%1").arg(fo))

    def set_gain(self, gain):
        self.gui.gainEdit.setText(QtCore.QString("%1").arg(gain))

    def set_decim(self, decim):
        self.gui.decimEdit.setText(QtCore.QString("%1").arg(decim))

    def set_gain_mu(self, gain):
        self.gui.gainMuEdit.setText(QtCore.QString("%1").arg(gain))

    def set_alpha(self, alpha):
        self.gui.alphaEdit.setText(QtCore.QString("%1").arg(alpha))
        
    def freqEditText(self):
        try:
            freq = self.gui.freqEdit.text().toDouble()[0]
            self.fg.set_freq(freq)
        except RuntimeError:
            pass

    def gainEditText(self):
        try:
            gain = self.gui.gainEdit.text().toDouble()[0]
            self.fg.set_gain(gain)
        except RuntimeError:
            pass

    def decimEditText(self):
        try:
            decim = self.gui.decimEdit.text().toInt()[0]
            self.fg.set_decim(decim)
        except RuntimeError:
            pass

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


    # Accessor function for packet error reporting
    def updatePacketInfo(self):
        # Pull these globals in from the main thread
        global n_rcvd, n_right, pktno

        per = float(n_rcvd - n_right)/float(pktno)
        self.gui.pktsRcvdEdit.setText(QtCore.QString("%1").arg(n_rcvd))
        self.gui.pktsCorrectEdit.setText(QtCore.QString("%1").arg(n_right))
        self.gui.perEdit.setText(QtCore.QString("%1").arg(per))



# ////////////////////////////////////////////////////////////////////
#        Define the GNU Radio Top Block
# ////////////////////////////////////////////////////////////////////


class my_top_block(gr.top_block):
    def __init__(self, demodulator, rx_callback, options):
        gr.top_block.__init__(self)

        self._rx_freq            = options.rx_freq         # receiver's center frequency
        self._rx_gain            = options.rx_gain         # receiver's gain
        self._rx_subdev_spec     = options.rx_subdev_spec  # daughterboard to use
        self._decim              = options.decim           # Decimating rate for the USRP (prelim)
        self._bitrate            = options.bitrate
        self._samples_per_symbol = options.samples_per_symbol
        self._demod_class        = demodulator
        self.gui_on              = options.gui
               
        if self._rx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --rx-freq FREQ must be specified\n")
            raise SystemExit

        # Set up USRP source
        self._setup_usrp_source(options)

        # copy the final answers back into options for use by demodulator
        options.samples_per_symbol = self._samples_per_symbol
        options.bitrate = self._bitrate
        options.decim = self._decim

        ok = self.set_freq(self._rx_freq)
        if not ok:
            print "Failed to set Rx frequency to %s" % (eng_notation.num_to_str(self._rx_freq))
            raise ValueError, eng_notation.num_to_str(self._rx_freq)

        self.set_gain(options.rx_gain)

        # Set up receive path
        self.rxpath = receive_path(demodulator, rx_callback, options) 

        # FIXME: do better exposure to lower issues for control
        self._gain_mu = self.rxpath.packet_receiver._demodulator._mm_gain_mu
        self._alpha = self.rxpath.packet_receiver._demodulator._costas_alpha

        self.connect(self.u, self.rxpath)

        if self.gui_on:
            self.qapp = QtGui.QApplication(sys.argv)
            fftsize = 2048
            
            self.snk_rxin = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS,
                                         -1/2, 1/2,
                                         "Received", True, True, False, True, True, False)
            self.snk_rx = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS,
                                       -1/2, 1/2,
                                       "Post-Synchronizer", True, True, False, True, True, False)

            self.snk_rxin.set_frequency_axis(-60, 60)
            self.snk_rx.set_frequency_axis(-60, 20)
            self.snk_rxin.set_time_domain_axis(-2000,2000)
            
            # Connect to the QT sinks
            # FIXME: make better exposure to receiver from rxpath
            self.receiver = self.rxpath.packet_receiver._demodulator.receiver
            self.connect(self.u, self.snk_rxin)
            self.connect(self.receiver, self.snk_rx)
            
            pyRxInQt  = self.snk_rxin.pyqwidget()
            pyRxIn = sip.wrapinstance(pyRxInQt, QtGui.QWidget)
            
            pyRxQt  = self.snk_rx.pyqwidget()
            pyRx = sip.wrapinstance(pyRxQt, QtGui.QWidget)
            
            self.main_box = dialog_box(pyRxIn, pyRx, self)
            self.main_box.show()

    def _setup_usrp_source(self, options):
        self.u = usrp_options.create_usrp_source(options)
        adc_rate = self.u.adc_rate()

        self.u.set_decim(self._decim)

        (self._bitrate, self._samples_per_symbol, self._decim) = \
                        pick_rx_bitrate(self._bitrate, self._demod_class.bits_per_symbol(), \
                                        self._samples_per_symbol, self._decim, adc_rate,  \
                                        self.u.get_decim_rates())

        self.u.set_decim(self._decim)
        self.set_auto_tr(True)                 # enable Auto Transmit/Receive switching

    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital up converter.
        """
        return self.u.set_center_freq(target_freq)

    def set_gain(self, gain):
        """
        Sets the analog gain in the USRP
        """
        if gain is None:
            r = self.u.gain_range()
            gain = (r[0] + r[1])/2               # set gain to midpoint
        self._rx_gain = gain
        return self.u.set_gain(self._rx_gain)

    def set_auto_tr(self, enable):
        return self.u.set_auto_tr(enable)

    def set_decim(self, decim):
        self._decim = decim
        self.u.set_decim(self._decim)

    def frequency(self):
        return self._rx_freq

    def gain(self):
        return self._rx_gain

    def decim(self):
        return self._decim

    def rx_gain_mu(self):
        return self._gain_mu

    def rx_gain_omega(self):
        return self.gain_omega
    
    def set_rx_gain_mu(self, gain):
        self._gain_mu = gain
        self.gain_omega = .25 * self._gain_mu * self._gain_mu
        self.receiver.set_gain_mu(self._gain_mu)
        self.receiver.set_gain_omega(self.gain_omega)

    def rx_alpha(self):
        return self._alpha

    def rx_beta(self):
        return self.beta
    
    def set_rx_alpha(self, alpha):
        self._alpha = alpha
        self.beta = .25 * self._alpha * self._alpha
        self.receiver.set_alpha(self._alpha)
        self.receiver.set_beta(self.beta)

    def add_options(normal, expert):
        """
        Adds usrp-specific options to the Options Parser
        """
        add_freq_option(normal)
        normal.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B")
        normal.add_option("", "--rx-gain", type="eng_float", default=None, metavar="GAIN",
                          help="set receiver gain in dB [default=midpoint].  See also --show-rx-gain-range")
        normal.add_option("", "--show-rx-gain-range", action="store_true", default=False, 
                          help="print min and max Rx gain available on selected daughterboard")
        normal.add_option("-v", "--verbose", action="store_true", default=False)
        normal.add_option("-G", "--gui", action="store_true", default=False,
                          help="Turn on the GUI [default=%default]")

        expert.add_option("", "--rx-freq", type="eng_float", default=None,
                          help="set Rx frequency to FREQ [default=%default]", metavar="FREQ")
        expert.add_option("-d", "--decim", type="intx", default=128,
                          help="set fpga decimation rate to DECIM [default=%default]")
        expert.add_option("", "--snr", type="eng_float", default=30,
                          help="set the SNR of the channel in dB [default=%default]")
   

    # Make a static method to call before instantiation
    add_options = staticmethod(add_options)


def add_freq_option(parser):
    """
    Hackery that has the -f / --freq option set both tx_freq and rx_freq
    """
    def freq_callback(option, opt_str, value, parser):
        parser.values.rx_freq = value
        parser.values.tx_freq = value

    if not parser.has_option('--freq'):
        parser.add_option('-f', '--freq', type="eng_float",
                          action="callback", callback=freq_callback,
                          help="set Tx and/or Rx frequency to FREQ [default=%default]",
                          metavar="FREQ")


# /////////////////////////////////////////////////////////////////////////////
#                                   main
# /////////////////////////////////////////////////////////////////////////////

global n_rcvd, n_right

def main():
    global n_rcvd, n_right, pktno

    n_rcvd = 0
    n_right = 0
    pktno = 1
    
    def rx_callback(ok, payload):
        global n_rcvd, n_right, pktno
        (pktno,) = struct.unpack('!H', payload[0:2])
        n_rcvd += 1
        if ok:
            n_right += 1

        if not options.gui:
            print "ok = %5s  pktno = %4d  n_rcvd = %4d  n_right = %4d" % (
                ok, pktno, n_rcvd, n_right)


    demods = modulation_utils.type_1_demods()

    # Create Options Parser:
    parser = OptionParser (option_class=eng_option, conflict_handler="resolve")
    expert_grp = parser.add_option_group("Expert")

    parser.add_option("-m", "--modulation", type="choice", choices=demods.keys(), 
                      default='dbpsk',
                      help="Select modulation from: %s [default=%%default]"
                            % (', '.join(demods.keys()),))

    my_top_block.add_options(parser, expert_grp)
    receive_path.add_options(parser, expert_grp)
    usrp_options.add_rx_options(parser)

    for mod in demods.values():
        mod.add_options(expert_grp)

    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help(sys.stderr)
        sys.exit(1)

    if options.rx_freq is None:
        sys.stderr.write("You must specify -f FREQ or --freq FREQ\n")
        parser.print_help(sys.stderr)
        sys.exit(1)


    # build the graph
    tb = my_top_block(demods[options.modulation], rx_callback, options)

    r = gr.enable_realtime_scheduling()
    if r != gr.RT_OK:
        print "Warning: Failed to enable realtime scheduling."

    tb.start()        # start flow graph

    if(options.gui):
        tb.qapp.exec_()
    else:
        tb.wait()         # wait for it to finish

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
