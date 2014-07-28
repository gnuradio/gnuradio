#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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

from gnuradio.filter import filter_design
from gnuradio import gr, filter
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
    from gnuradio import blocks
except ImportError:
    sys.stderr.write("Error: Program requires gr-blocks.\n")
    sys.exit(1)

try:
    from gnuradio import channels
except ImportError:
    sys.stderr.write("Error: Program requires gr-channels.\n")
    sys.exit(1)

class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        Rs = 8000
        f1 = 1000
        f2 = 2000

        npts = 2048

        self.qapp = QtGui.QApplication(sys.argv)

        self.filt_taps = [1,]

        src1 = analog.sig_source_c(Rs, analog.GR_SIN_WAVE, f1, 0.1, 0)
        src2 = analog.sig_source_c(Rs, analog.GR_SIN_WAVE, f2, 0.1, 0)
        src  = blocks.add_cc()
        channel = channels.channel_model(0.01)
        self.filt = filter.fft_filter_ccc(1, self.filt_taps)
        thr = blocks.throttle(gr.sizeof_gr_complex, 100*npts)
        self.snk1 = qtgui.freq_sink_c(npts, filter.firdes.WIN_BLACKMAN_hARRIS,
                                      0, Rs,
                                      "Complex Freq Example", 1)

        self.connect(src1, (src,0))
        self.connect(src2, (src,1))
        self.connect(src,  channel, thr, self.filt, (self.snk1, 0))

        # Get the reference pointer to the SpectrumDisplayForm QWidget
        pyQt  = self.snk1.pyqwidget()

        # Wrap the pointer as a PyQt SIP object
        # This can now be manipulated as a PyQt4.QtGui.QWidget
        pyWin = sip.wrapinstance(pyQt, QtGui.QWidget)
        pyWin.show()

    def update_filter(self, filtobj):
        print "Filter type:", filtobj.get_restype()
        print "Filter params", filtobj.get_params()
        self.filt.set_taps(filtobj.get_taps())

if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    mw = filter_design.launch(sys.argv, tb.update_filter)
    mw.show()
    tb.qapp.exec_()
    tb.stop()


