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

from gnuradio import gr
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from gnuradio.qtgui import qtgui
from optparse import OptionParser
import sys

try:
    from gnuradio.qtgui import qtgui
    from PyQt4 import QtGui, QtCore
    import sip
except ImportError:
    print "Please install gr-qtgui."
    sys.exit(1)

try:
    from usrp_display_qtgui import Ui_MainWindow
except ImportError:
    print "Error: could not find usrp_display_qtgui.py:"
    print "\t\"pyuic4 usrp_display_qtgui.ui -o usrp_display_qtgui.py\""
    sys.exit(1)



# ////////////////////////////////////////////////////////////////////
#        Define the QT Interface and Control Dialog
# ////////////////////////////////////////////////////////////////////


class main_window(QtGui.QMainWindow):
    def __init__(self, snk, fg, parent=None):

        QtGui.QWidget.__init__(self, parent)
        self.gui = Ui_MainWindow()
        self.gui.setupUi(self)

        self.fg = fg

        # Add the qtsnk widgets to the layout box
        self.gui.sinkLayout.addWidget(snk)

        # Connect up some signals
        self.connect(self.gui.pauseButton, QtCore.SIGNAL("clicked()"),
                     self.pauseFg)
        self.connect(self.gui.frequencyEdit, QtCore.SIGNAL("editingFinished()"),
                     self.frequencyEditText)
        self.connect(self.gui.gainEdit, QtCore.SIGNAL("editingFinished()"),
                     self.gainEditText)
        self.connect(self.gui.bandwidthEdit, QtCore.SIGNAL("editingFinished()"),
                     self.bandwidthEditText)
        self.connect(self.gui.amplifierEdit, QtCore.SIGNAL("editingFinished()"),
                     self.amplifierEditText)

        self.connect(self.gui.actionSaveData, QtCore.SIGNAL("activated()"),
                     self.saveData)
        self.gui.actionSaveData.setShortcut(QtGui.QKeySequence.Save)

    def pauseFg(self):
        if(self.gui.pauseButton.text() == "Pause"):
            self.fg.stop()
            self.fg.wait()
            self.gui.pauseButton.setText("Unpause")
        else:
            self.fg.start()
            self.gui.pauseButton.setText("Pause")
      

    # Functions to set the values in the GUI
    def set_frequency(self, freq):
        self.freq = freq
        sfreq = eng_notation.num_to_str(self.freq)
        self.gui.frequencyEdit.setText(QtCore.QString("%1").arg(sfreq))
        
    def set_gain(self, gain):
        self.gain = gain
        self.gui.gainEdit.setText(QtCore.QString("%1").arg(self.gain))

    def set_bandwidth(self, bw):
        self.bw = bw
        sbw = eng_notation.num_to_str(self.bw)
        self.gui.bandwidthEdit.setText(QtCore.QString("%1").arg(sbw))

    def set_amplifier(self, amp):
        self.amp = amp
        self.gui.amplifierEdit.setText(QtCore.QString("%1").arg(self.amp))


    # Functions called when signals are triggered in the GUI
    def frequencyEditText(self):
        try:
            freq = eng_notation.str_to_num(self.gui.frequencyEdit.text().toAscii()) 
            self.fg.set_frequency(freq)
            self.freq = freq
        except RuntimeError:
            pass

    def gainEditText(self):
        try:
            gain = float(self.gui.gainEdit.text())
            self.fg.set_gain(gain)
            self.gain = gain
        except ValueError:
            pass
                
    def bandwidthEditText(self):
        try:
            bw = eng_notation.str_to_num(self.gui.bandwidthEdit.text().toAscii())
            self.fg.set_bandwidth(bw)
            self.bw = bw
        except ValueError:
            pass
        
    def amplifierEditText(self):
        try:
            amp = float(self.gui.amplifierEdit.text())
            self.fg.set_amplifier_gain(amp)
            self.amp = amp
        except ValueError:
            pass

    def saveData(self):
        fileName = QtGui.QFileDialog.getSaveFileName(self, "Save data to file", ".");
        if(len(fileName)):
            self.fg.save_to_file(str(fileName))

        
def pick_subdevice(u):
    """
    The user didn't specify a subdevice on the command line.
    If there's a daughterboard on A, select A.
    If there's a daughterboard on B, select B.
    Otherwise, select A.
    """
    if u.db(0, 0).dbid() >= 0:       # dbid is < 0 if there's no d'board or a problem
        return (0, 0)
    if u.db(0, 0).dbid() >= 0:
        return (1, 0)
    return (0, 0)

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
        parser.add_option("-W", "--bw", type="float", default=1e6,
                          help="set bandwidth of receiver [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB [default is midpoint]")
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
        parser.add_option("", "--fft-size", type="int", default=2048,
                          help="Set FFT frame size, [default=%default]");

        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)
	self.options = options
        self.show_debug_info = True

        # Call this before creating the Qt sink
        self.qapp = QtGui.QApplication(sys.argv)

        self._fftsize = options.fft_size

        self.u = usrp.source_c(which=options.which)
        self._adc_rate = self.u.converter_rate()
        self.set_bandwidth(options.bw)

        if options.rx_subdev_spec is None:
            options.rx_subdev_spec = pick_subdevice(self.u)
        self._rx_subdev_spec = options.rx_subdev_spec
        self.u.set_mux(usrp.determine_rx_mux_value(self.u, self._rx_subdev_spec))
        self.subdev = usrp.selected_subdev(self.u, self._rx_subdev_spec)

        self._gain_range = self.subdev.gain_range()
        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self._gain_range
            options.gain = float(g[0]+g[1])/2
        self.set_gain(options.gain)

        if options.freq is None:
            # if no frequency was specified, use the mid-point of the subdev
            f = self.subdev.freq_range()
            options.freq = float(f[0]+f[1])/2
        self.set_frequency(options.freq)

        self.snk = qtgui.sink_c(self._fftsize, gr.firdes.WIN_BLACKMAN_hARRIS,
                                self._freq, self._bandwidth,
                                "USRP Display",
                                True, True, False, True, False)

        # Set up internal amplifier
        self.amp = gr.multiply_const_cc(0.0)
        self.set_amplifier_gain(0.001)

        # Connect the flow graph
        self.connect(self.u, self.amp, self.snk)


        # Get the reference pointer to the SpectrumDisplayForm QWidget
        # Wrap the pointer as a PyQt SIP object
        #     This can now be manipulated as a PyQt4.QtGui.QWidget
        self.pysink = sip.wrapinstance(self.snk.pyqwidget(), QtGui.QWidget)

        self.main_win = main_window(self.pysink, self)

        self.main_win.set_frequency(self._freq)
        self.main_win.set_gain(self._gain)
        self.main_win.set_bandwidth(self._bandwidth)
        self.main_win.set_amplifier(self._amp_value)

        self.main_win.show()

    def save_to_file(self, name):
        # Pause the flow graph
        self.stop()
        self.wait()

        # Add file sink to save data
        self.file_sink = gr.file_sink(gr.sizeof_gr_complex, name)
        self.connect(self.amp, self.file_sink)

        # Restart flow graph
        self.start()

    def set_gain(self, gain):
        self._gain = gain
        self.subdev.set_gain(self._gain)

    def set_frequency(self, freq):
        self._freq = freq
        self.u.tune(0, self.subdev, self._freq)

        try:
            self.snk.set_frequency_range(self._freq, self._bandwidth)
        except:
            pass

    def set_bandwidth(self, bw):
        self._bandwidth = bw
        self._decim = int(self._adc_rate / self._bandwidth)
        self.u.set_decim_rate(self._decim)

        try:
            self.snk.set_frequency_range(self._freq, self._bandwidth)
        except:
            pass

    def set_amplifier_gain(self, amp):
            self._amp_value = amp
            self.amp.set_k(self._amp_value)

       
if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    tb.qapp.exec_()
