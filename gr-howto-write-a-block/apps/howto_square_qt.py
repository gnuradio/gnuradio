#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Howto Square Qt
# Generated: Sun Sep 30 13:32:52 2012
##################################################

from PyQt4 import Qt
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import qtgui
from gnuradio import analog
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import howto
import sip
import sys

class howto_square_qt(gr.top_block, Qt.QWidget):

	def __init__(self):
		gr.top_block.__init__(self, "Howto Square Qt")
		Qt.QWidget.__init__(self)
		self.setWindowTitle("Howto Square Qt")
		self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
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

		self.settings = Qt.QSettings("GNU Radio", "howto_square_qt")
		self.restoreGeometry(self.settings.value("geometry").toByteArray())


		##################################################
		# Variables
		##################################################
		self.samp_rate = samp_rate = 32000

		##################################################
		# Blocks
		##################################################
		self.qtgui_time_sink_x_0 = qtgui.time_sink_f(
			1024, #size
			samp_rate, #bw
			"QT GUI Plot", #name
			3 #number of inputs
		)
		self._qtgui_time_sink_x_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0.pyqwidget(), Qt.QWidget)
		self.top_layout.addWidget(self._qtgui_time_sink_x_0_win)
		self.howto_square_ff_0 = howto.square_ff()
		self.howto_square2_ff_0 = howto.square2_ff()
		self.gr_throttle_0 = gr.throttle(gr.sizeof_float*1, samp_rate)
		self.gr_sig_source_x_0 = analog.sig_source_f(samp_rate, analog.GR_COS_WAVE, 1000, 1, 0)

		##################################################
		# Connections
		##################################################
		self.connect((self.howto_square2_ff_0, 0), (self.qtgui_time_sink_x_0, 0))
		self.connect((self.gr_throttle_0, 0), (self.howto_square2_ff_0, 0))
		self.connect((self.gr_throttle_0, 0), (self.howto_square_ff_0, 0))
		self.connect((self.gr_sig_source_x_0, 0), (self.gr_throttle_0, 0))
		self.connect((self.gr_throttle_0, 0), (self.qtgui_time_sink_x_0, 1))
		self.connect((self.howto_square_ff_0, 0), (self.qtgui_time_sink_x_0, 2))

# QT sink close method reimplementation
	def closeEvent(self, event):
		self.settings = Qt.QSettings("GNU Radio", "howto_square_qt")
		self.settings.setValue("geometry", self.saveGeometry())
		event.accept()

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate
		self.gr_sig_source_x_0.set_sampling_freq(self.samp_rate)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	qapp = Qt.QApplication(sys.argv)
	tb = howto_square_qt()
	tb.start()
	tb.show()
	qapp.exec_()
	tb.stop()
	tb = None #to clean up Qt widgets

