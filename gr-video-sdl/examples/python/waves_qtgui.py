#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: SDL Demo with Qt GUI
# Author: Marcus Müller, Oleg Nikitin
# GNU Radio version: g935569cc9

from PyQt5 import Qt
from gnuradio import qtgui
from gnuradio import analog
from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
from PyQt5 import Qt
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import video_sdl
import sip
import threading


class waves_qtgui(gr.top_block, Qt.QWidget):
    def __init__(self, num_rows=512, periods_per_row=(1 - 1 / 255), update_rate=25, vector_length=512):
        gr.top_block.__init__(self, "SDL Demo with Qt GUI", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("SDL Demo with Qt GUI")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except BaseException as exc:
            print(f"Qt GUI: Could not set Icon: {str(exc)}", file=sys.stderr)
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

        self.settings = Qt.QSettings("gnuradio/flowgraphs", "waves_qtgui")

        try:
            geometry = self.settings.value("geometry")
            if geometry:
                self.restoreGeometry(geometry)
        except BaseException as exc:
            print(f"Qt GUI: Could not restore geometry: {str(exc)}", file=sys.stderr)
        self.flowgraph_started = threading.Event()

        ##################################################
        # Parameters
        ##################################################
        self.num_rows = num_rows
        self.periods_per_row = periods_per_row
        self.update_rate = update_rate
        self.vector_length = vector_length

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = vector_length * num_rows * update_rate

        ##################################################
        # Blocks
        ##################################################

        self.video_sdl_sink_0 = video_sdl.sink_uc(0, vector_length, num_rows, vector_length, num_rows)
        self.qtgui_time_sink_x_0 = qtgui.time_sink_f((vector_length * num_rows), samp_rate, "", 1, None)
        self.qtgui_time_sink_x_0.set_update_time(0.5 / update_rate)
        self.qtgui_time_sink_x_0.set_y_axis(0, 255)

        self.qtgui_time_sink_x_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_0.enable_tags(True)
        self.qtgui_time_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0.enable_autoscale(False)
        self.qtgui_time_sink_x_0.enable_grid(False)
        self.qtgui_time_sink_x_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0.enable_control_panel(False)
        self.qtgui_time_sink_x_0.enable_stem_plot(False)

        labels = ['Signal 1', 'Signal 2', 'Signal 3', 'Signal 4', 'Signal 5',
                  'Signal 6', 'Signal 7', 'Signal 8', 'Signal 9', 'Signal 10']
        widths = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        colors = ['blue', 'red', 'green', 'black', 'cyan',
                  'magenta', 'yellow', 'dark red', 'dark green', 'dark blue']
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
        styles = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1, -1, -1, -1, -1, -1]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_time_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0.qwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_0_win)
        self.blocks_throttle2_0 = blocks.throttle(
            gr.sizeof_float * 1,
            (update_rate * vector_length * num_rows),
            True,
            0
            if "time" == "auto"
            else max(
                int(float((0.25 / update_rate)) * (update_rate * vector_length * num_rows))
                if "time" == "time"
                else int((0.25 / update_rate)),
                1,
            ),
        )
        self.blocks_multiply_const_xx_0 = blocks.multiply_const_ff(2**6, 1)
        self.blocks_float_to_uchar_0 = blocks.float_to_uchar(1, 1, 0)
        self.analog_sig_source_x_0 = analog.sig_source_f(
            (update_rate * vector_length * num_rows),
            analog.GR_COS_WAVE,
            (update_rate * periods_per_row * num_rows),
            1,
            1,
            0,
        )

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_throttle2_0, 0))
        self.connect((self.blocks_float_to_uchar_0, 0), (self.video_sdl_sink_0, 0))
        self.connect((self.blocks_multiply_const_xx_0, 0), (self.blocks_float_to_uchar_0, 0))
        self.connect((self.blocks_multiply_const_xx_0, 0), (self.qtgui_time_sink_x_0, 0))
        self.connect((self.blocks_throttle2_0, 0), (self.blocks_multiply_const_xx_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("gnuradio/flowgraphs", "waves_qtgui")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_num_rows(self):
        return self.num_rows

    def set_num_rows(self, num_rows):
        self.num_rows = num_rows
        self.set_samp_rate(self.vector_length * self.num_rows * self.update_rate)
        self.analog_sig_source_x_0.set_sampling_freq((self.update_rate * self.vector_length * self.num_rows))
        self.analog_sig_source_x_0.set_frequency((self.update_rate * self.periods_per_row * self.num_rows))
        self.blocks_throttle2_0.set_sample_rate((self.update_rate * self.vector_length * self.num_rows))

    def get_periods_per_row(self):
        return self.periods_per_row

    def set_periods_per_row(self, periods_per_row):
        self.periods_per_row = periods_per_row
        self.analog_sig_source_x_0.set_frequency((self.update_rate * self.periods_per_row * self.num_rows))

    def get_update_rate(self):
        return self.update_rate

    def set_update_rate(self, update_rate):
        self.update_rate = update_rate
        self.set_samp_rate(self.vector_length * self.num_rows * self.update_rate)
        self.analog_sig_source_x_0.set_sampling_freq((self.update_rate * self.vector_length * self.num_rows))
        self.analog_sig_source_x_0.set_frequency((self.update_rate * self.periods_per_row * self.num_rows))
        self.blocks_throttle2_0.set_sample_rate((self.update_rate * self.vector_length * self.num_rows))
        self.qtgui_time_sink_x_0.set_update_time(0.5 / self.update_rate)

    def get_vector_length(self):
        return self.vector_length

    def set_vector_length(self, vector_length):
        self.vector_length = vector_length
        self.set_samp_rate(self.vector_length * self.num_rows * self.update_rate)
        self.analog_sig_source_x_0.set_sampling_freq((self.update_rate * self.vector_length * self.num_rows))
        self.blocks_throttle2_0.set_sample_rate((self.update_rate * self.vector_length * self.num_rows))

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.qtgui_time_sink_x_0.set_samp_rate(self.samp_rate)


def argument_parser():
    parser = ArgumentParser()
    parser.add_argument(
        "-n", "--num-rows", dest="num_rows", type=intx, default=512,
        help="Set Number of Rows [default=%(default)r]")
    parser.add_argument(
        "-p", "--periods-per-row", dest="periods_per_row",
        type=eng_float, default=eng_notation.num_to_str(float((1 - 1 / 255))),
        help="Set Periods per Row [default=%(default)r]")
    parser.add_argument(
        "-r", "--update-rate", dest="update_rate", type=intx, default=25,
        help="Set Update Rate [default=%(default)r]")
    parser.add_argument(
        "-l", "--vector-length", dest="vector_length", type=intx, default=512,
        help="Set Vector Length [default=%(default)r]")
    return parser


def main(top_block_cls=waves_qtgui, options=None):
    if options is None:
        options = argument_parser().parse_args()

    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(
        num_rows=options.num_rows,
        periods_per_row=options.periods_per_row,
        update_rate=options.update_rate,
        vector_length=options.vector_length,
    )

    tb.start()
    tb.flowgraph_started.set()

    tb.show()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    qapp.exec_()


if __name__ == '__main__':
    main()
