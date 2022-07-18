#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Not titled yet
# Author: josh
# GNU Radio version: 0.2.0

from packaging.version import Version as StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

from PyQt5 import Qt
from gnuradio import qtgui
from gnuradio.kernel.filter import firdes
import sip
from gnuradio import analog
from gnuradio import blocks
from gnuradio import gr
#from gnuradio.filter import firdes
#from gnuradio.fft import window
import sys
import signal
from argparse import ArgumentParser
#from gnuradio.eng_arg import eng_float, intx
#from gnuradio import eng_notation
import math



from gnuradio import qtgui

class fm_rx(Qt.QWidget):
    def start(self):
        self.fg.start()

    def stop(self):
        self.fg.stop()

    def wait(self):
        self.fg.wait()

    def connect(self,*args):
        return self.fg.connect(*args)

    def msg_connect(self,*args):
        return self.fg.connect(*args)

    def __init__(self):
        self.fg = gr.flowgraph("Not titled yet")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Not titled yet")
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "fm_rx")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 4000000
        self.in_rate = in_rate = samp_rate
        self.freq = freq = 90500000
        self.fm_deviation_hz = fm_deviation_hz = 75e3

        ##################################################
        # Blocks
        ##################################################
        self.qtgui_time_sink_0 = qtgui.time_sink_f(10240,samp_rate,"hello",1)
        self._qtgui_time_sink_0_win = sip.wrapinstance(self.qtgui_time_sink_0.qwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_0_win)
        self.blocks_null_source_0 = blocks.null_source( 1,0, impl=blocks.null_source.cpu)
        self.analog_quadrature_demod_0 = analog.quadrature_demod( in_rate/(2*math.pi*fm_deviation_hz), impl=analog.quadrature_demod.cpu)
        self.analog_fm_deemph_0 = analog.fm_deemph( 400000,75e-6, impl=analog.fm_deemph.cpu)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_fm_deemph_0, 0), (self.qtgui_time_sink_0, 0))
        self.connect((self.analog_quadrature_demod_0, 0), (self.analog_fm_deemph_0, 0))
        self.connect((self.blocks_null_source_0, 0), (self.analog_quadrature_demod_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "fm_rx")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.set_in_rate(self.samp_rate)

    def get_in_rate(self):
        return self.in_rate

    def set_in_rate(self, in_rate):
        self.in_rate = in_rate

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq

    def get_fm_deviation_hz(self):
        return self.fm_deviation_hz

    def set_fm_deviation_hz(self, fm_deviation_hz):
        self.fm_deviation_hz = fm_deviation_hz




def main(flowgraph_cls=fm_rx, options=None):

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    fg = flowgraph_cls()
    rt = gr.runtime()

    rt.initialize(fg.fg)
    rt.start()

    fg.show()

    def sig_handler(sig=None, frame=None):
        rt.stop()
        rt.wait()

        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    qapp.exec_()

if __name__ == '__main__':
    main()
