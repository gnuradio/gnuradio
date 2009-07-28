#!/usr/bin/env python
#
# Copyright 2005,2006,2007,2008,2009 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, eng_notation, optfir
from gnuradio import audio
from gnuradio import usrp2
from gnuradio import blks2
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys
import math


try:
    from gnuradio.qtgui import qtgui
    from PyQt4 import QtGui, QtCore
    import sip
except ImportError:
    print "Please install gr-qtgui."
    sys.exit(1)
    
try:
    from qt_wfm_interface import Ui_InterfaceWindow
except ImportError:
    print "Error: could not find qt_wfm_interface.py:"
    print "\tPlease run: \"pyuic4 qt_wfm_interface.ui -o qt_wfm_interface.py\""
    sys.exit(1)

print "This program is not in a proper working state. Comment this out if you want to play."
sys.exit(1)


# ////////////////////////////////////////////////////////////////////
#        Define the QT Interface and Control Dialog
# ////////////////////////////////////////////////////////////////////


class dialog_box(QtGui.QMainWindow):
    def __init__(self, snk_usrp, snk_vol, fg, parent=None):

        QtGui.QWidget.__init__(self, parent)
        self.gui = Ui_InterfaceWindow()
        self.gui.setupUi(self)

        self.fg = fg

        # Set USRP parameters
        self.set_bw(self.fg.usrp_bw())
        self.set_freq(self.fg.freq())
        self.set_gain(self.fg.gain())
        self.set_volume(self.fg.volume())

        # Add the qtsnk widgets to the hlayout box
        self.gui.sinkLayout.addWidget(snk_usrp)
        self.gui.sinkLayout.addWidget(snk_vol)


        # Connect up some signals
        self.connect(self.gui.pauseButton, QtCore.SIGNAL("clicked()"),
                     self.pauseFg)

        self.connect(self.gui.bandwidthEdit, QtCore.SIGNAL("editingFinished()"),
                     self.bwEditText)
        self.connect(self.gui.freqEdit, QtCore.SIGNAL("editingFinished()"),
                     self.freqEditText)
        self.connect(self.gui.gainEdit, QtCore.SIGNAL("editingFinished()"),
                     self.gainEditText)

        self.connect(self.gui.volumeEdit, QtCore.SIGNAL("editingFinished()"),
                     self.volumeEditText)


    def pauseFg(self):
        if(self.gui.pauseButton.text() == "Pause"):
            self.fg.stop()
            self.fg.wait()
            self.gui.pauseButton.setText("Unpause")
        else:
            self.fg.start()
            self.gui.pauseButton.setText("Pause")


    # Accessor functions for Gui to manipulate USRP
    def set_bw(self, bw):
        self.gui.bandwidthEdit.setText(QtCore.QString("%1").arg(bw))

    def set_freq(self, freq):
        self.gui.freqEdit.setText(QtCore.QString("%1").arg(freq))

    def set_gain(self, gain):
        self.gui.gainEdit.setText(QtCore.QString("%1").arg(gain))

    def set_volume(self, vol):
        self.gui.volumeEdit.setText(QtCore.QString("%1").arg(vol))

    def bwEditText(self):
        try:
            bw = self.gui.bandwidthEdit.text().toDouble()[0]
            self.fg.set_usrp_bw(bw)
        except RuntimeError:
            pass

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

    def volumeEditText(self):
        try:
            vol = self.gui.volumeEdit.text().toDouble()[0]
            self.fg.set_volume(vol)
        except RuntimeError:
            pass




# ////////////////////////////////////////////////////////////////////
#        Define the GNU Radio Top Block
# ////////////////////////////////////////////////////////////////////


class wfm_rx_block (gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)
        
        parser = OptionParser(option_class=eng_option)
        parser.add_option("-e", "--interface", type="string", default="eth0",
                          help="select Ethernet interface, default is eth0")
        parser.add_option("-m", "--mac-addr", type="string", default="",
                          help="select USRP by MAC address, default is auto-select")
        #parser.add_option("-A", "--antenna", default=None,
        #                  help="select Rx Antenna (only on RFX-series boards)")
        parser.add_option("-f", "--freq", type="eng_float", default=100.1,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-V", "--volume", type="eng_float", default=None,
                          help="set volume (default is midpoint)")
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm device name.  E.g., hw:0,0 or surround51 or /dev/dsp")

        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)
        
        self._volume = options.volume
        self._usrp_freq = options.freq
        self._usrp_gain = options.gain
        self._audio_rate = int(32e3)

        # build graph
     
        self.u = usrp2.source_32fc(options.interface, options.mac_addr)

        # calculate decimation values to get USRP BW at 320 kHz
        self.calculate_usrp_bw(320e3)

        self.set_decim(self._usrp_decim)

        #FIXME: need named constants and text descriptions available to (gr-)usrp2 even
        #when usrp(1) module is not built.  A usrp_common module, perhaps?
        dbid = self.u.daughterboard_id()
        print "Using RX d'board 0x%04X" % (dbid,)
        #if not (dbid == 0x0001 or #usrp_dbid.BASIC_RX
        #        dbid == 0x0003 or #usrp_dbid.TV_RX
        #        dbid == 0x000c or #usrp_dbid.TV_RX_REV_2
        #        dbid == 0x0040 or #usrp_dbid.TV_RX_REV_3
        #        dbid == 0x0043 or #usrp_dbid.TV_RX_MIMO
        #        dbid == 0x0044 or #usrp_dbid.TV_RX_REV_2_MIMO
        #        dbid == 0x0045 ): #usrp_dbid.TV_RX_REV_3_MIMO
        #    print "This daughterboard does not cover the required frequency range"
        #    print "for this application.  Please use a BasicRX or TVRX daughterboard."
        #    raw_input("Press ENTER to continue anyway, or Ctrl-C to exit.")

        chan_filt_coeffs = optfir.low_pass (1,                 # gain
                                            self._usrp_rate,   # sampling rate
                                            80e3,        # passband cutoff
                                            115e3,       # stopband cutoff
                                            0.1,         # passband ripple
                                            60)          # stopband attenuation
        #print len(chan_filt_coeffs)
        chan_filt = gr.fir_filter_ccf (self._chanfilt_decim, chan_filt_coeffs)

        self.guts = blks2.wfm_rcv (self._demod_rate, self._audio_decim)

        self.volume_control = gr.multiply_const_ff(1)

        # sound card as final sink
        #audio_sink = audio.sink (int (audio_rate),
        #                         options.audio_output,
        #                         False)  # ok_to_block
        audio_sink = audio.sink (self._audio_rate,
                                 options.audio_output)

        
        if self._usrp_gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.gain_range()
            print "Gain range: ", g
            self._usrp_gain = float(g[0]+g[1])/2

        if self._volume is None:
            g = self.volume_range()
            self._volume = float(g[0]+g[1])/2
            
        if abs(self._usrp_freq) < 1e6:
            self._usrp_freq *= 1e6

        # set initial values
        self.set_gain(self._usrp_gain)
        self.set_volume(self._volume)
        if not(self.set_freq(self._usrp_freq)):
            print ("Failed to set initial frequency")


        # Define a GUI sink to display the received signal
        self.qapp = QtGui.QApplication(sys.argv)
        fftsize = 2048
        
        self.usrp_rx = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS,
                                    -self._usrp_rate/2.0, self._usrp_rate/2.0,
                                    "Received Signal", True, True, False, True, False,
                                    use_openGL=False)
        self.usrp_rx2 = qtgui.sink_f(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS,
                                    -self._usrp_rate/2.0, self._usrp_rate/2.0,
                                    "Received Signal", True, True, False, True, False)
        
        # now wire it all together
        self.connect (self.u, chan_filt, self.guts, self.volume_control, audio_sink)
        self.connect (self.u, self.usrp_rx)
        self.connect (self.volume_control, self.usrp_rx2)
        
        usrp_rx_widget = sip.wrapinstance(self.usrp_rx.pyqwidget(), QtGui.QWidget)
        usrp_rx2_widget = sip.wrapinstance(self.usrp_rx2.pyqwidget(), QtGui.QWidget)
        
        self.main_box = dialog_box(usrp_rx_widget, usrp_rx2_widget, self)
        self.main_box.show()


    def calculate_usrp_bw(self, bw):
        """
        Calculate the different decimation rates that make the USRP BW equal to the
        input bandwidth parameter 'bw' and the audio bandwidth equal to the system-
        wide bandwidth 'self._audio_rate'
        """
        
        adc_rate = self.u.adc_rate()
        d_usrp = int(adc_rate/bw)
        bw_real = adc_rate / float(d_usrp)

        d_chan = 1
        demod_rate = bw_real / d_chan

        d_audio = int(bw_real / self._audio_rate)
        audio_rate = demod_rate / d_audio

        self._usrp_decim  = d_usrp
        self._chanfilt_decim = d_chan
        self._audio_decim = d_audio
        self._demod_rate = demod_rate
        self._usrp_rate = bw_real

        print "USRP Decimation:  ", self._usrp_decim
        print "USRP Bandwidth:   ", bw_real
        print "Audio Decimation: ", self._audio_decim
        print "Audio Bandwidth:  ", audio_rate

    def set_volume (self, vol):
        g = self.volume_range()
        self._volume = max(g[0], min(g[1], vol))
        self.volume_control.set_k(10**(self._volume/10))
                                        
    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter.
        """
        r = self.u.set_center_freq(target_freq)
        if r:
            self._usrp_freq = target_freq
            return True
        return False

    def set_usrp_bw(self, bw):
        self.calculate_usrp_bw(bw)

    def set_gain(self, gain):
        self._usrp_gain = gain
        self.u.set_gain(gain)

    def set_decim(self, decim):
        self._usrp_decim = int(decim)
        self.u.set_decim(self._usrp_decim)

    def volume(self):
        return self._volume
    
    def freq(self):
        return self._usrp_freq

    def usrp_bw(self):
        return self._usrp_rate

    def gain(self):
        return self._usrp_gain

    def decim(self):
        return self._usrp_decim

    def volume_range(self):
        return (-20.0, 0.0, 0.5)
        

if __name__ == '__main__':
    tb = wfm_rx_block()
    tb.start()
    tb.qapp.exec_()
    
