#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

###############################################################################
# Imports
###############################################################################
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
import gui
import sys
import os
from PyQt5 import Qt, QtGui, QtCore, uic
import PyQt5.Qwt5 as Qwt
from gnuradio import zeromq
import signal

class gui(QtGui.QMainWindow):
    def __init__(self, window_name, options, parent=None):
        QtGui.QMainWindow.__init__(self, parent)

        # give Ctrl+C back to system
        signal.signal(signal.SIGINT, signal.SIG_DFL)

        self.gui = uic.loadUi(os.path.join(os.path.dirname(__file__),'main_window.ui'), self)

        self.update_timer = Qt.QTimer()

        # socket addresses
        rpc_adr_server = "tcp://"+options.servername+":6666"
        rpc_adr_client = "tcp://"+options.clientname+":6667"
        probe_adr_server = "tcp://"+options.servername+":5556"
        probe_adr_client = "tcp://"+options.clientname+":5557"

        # ZeroMQ
        self.probe_manager = zeromq.probe_manager()
        self.probe_manager.add_socket(probe_adr_server, 'float32', self.plot_data_server)
        self.probe_manager.add_socket(probe_adr_client, 'float32', self.plot_data_client)

        self.rpc_mgr_server = zeromq.rpc_manager()
        self.rpc_mgr_server.set_request_socket(rpc_adr_server)
        self.rpc_mgr_client = zeromq.rpc_manager()
        self.rpc_mgr_client.set_request_socket(rpc_adr_client)

        self.gui.setWindowTitle(window_name)
        self.gui.qwtPlotServer.setTitle("Signal Scope")
        self.gui.qwtPlotServer.setAxisTitle(Qwt.QwtPlot.xBottom, "Samples")
        self.gui.qwtPlotServer.setAxisTitle(Qwt.QwtPlot.yLeft, "Amplitude")
        self.gui.qwtPlotServer.setAxisScale(Qwt.QwtPlot.xBottom, 0, 100)
        self.gui.qwtPlotServer.setAxisScale(Qwt.QwtPlot.yLeft, -2, 2)
        self.gui.qwtPlotClient.setTitle("Signal Scope")
        self.gui.qwtPlotClient.setAxisTitle(Qwt.QwtPlot.xBottom, "Samples")
        self.gui.qwtPlotClient.setAxisTitle(Qwt.QwtPlot.yLeft, "Amplitude")
        self.gui.qwtPlotClient.setAxisScale(Qwt.QwtPlot.xBottom, 0, 100)
        self.gui.qwtPlotClient.setAxisScale(Qwt.QwtPlot.yLeft, -2, 2)

        # Grid
        pen = Qt.QPen(Qt.Qt.DotLine)
        pen.setColor(Qt.Qt.black)
        pen.setWidth(0)
        grid_server = Qwt.QwtPlotGrid()
        grid_client = Qwt.QwtPlotGrid()
        grid_server.setPen(pen)
        grid_client.setPen(pen)
        grid_server.attach(self.gui.qwtPlotServer)
        grid_client.attach(self.gui.qwtPlotClient)

        #Signals
        self.connect(self.update_timer, QtCore.SIGNAL("timeout()"), self.probe_manager.watcher)
        self.connect(self.gui.pushButtonRunServer, QtCore.SIGNAL("clicked()"), self.start_fg_server)
        self.connect(self.gui.pushButtonStopServer, QtCore.SIGNAL("clicked()"), self.stop_fg_server)
        self.connect(self.gui.pushButtonRunClient, QtCore.SIGNAL("clicked()"), self.start_fg_client)
        self.connect(self.gui.pushButtonStopClient, QtCore.SIGNAL("clicked()"), self.stop_fg_client)
        self.connect(self.gui.comboBox, QtCore.SIGNAL("currentIndexChanged(QString)"), self.set_waveform)
        self.connect(self.gui.spinBox, QtCore.SIGNAL("valueChanged(int)"), self.set_gain)
        self.shortcut_start = QtGui.QShortcut(Qt.QKeySequence("Ctrl+S"), self.gui)
        self.shortcut_stop = QtGui.QShortcut(Qt.QKeySequence("Ctrl+C"), self.gui)
        self.shortcut_exit = QtGui.QShortcut(Qt.QKeySequence("Ctrl+D"), self.gui)
        self.connect(self.shortcut_exit, QtCore.SIGNAL("activated()"), self.gui.close)

        # start update timer
        self.update_timer.start(30)

    def start_fg_server(self):
        self.rpc_mgr_server.request("start_fg")

    def stop_fg_server(self):
        self.rpc_mgr_server.request("stop_fg")

    def start_fg_client(self):
        self.rpc_mgr_client.request("start_fg")

    def stop_fg_client(self):
        self.rpc_mgr_client.request("stop_fg")

    # plot the data from the queues
    def plot_data(self, plot, samples):
        self.x = list(range(0,len(samples),1))
        self.y = samples
        # clear the previous points from the plot
        plot.clear()
        # draw curve with new points and plot
        curve = Qwt.QwtPlotCurve()
        curve.setPen(Qt.QPen(Qt.Qt.blue, 2))
        curve.attach(plot)
        curve.setData(self.x, self.y)
        plot.replot()

    def plot_data_server(self, samples):
        self.plot_data(self.gui.qwtPlotServer, samples)

    def plot_data_client(self, samples):
        self.plot_data(self.gui.qwtPlotClient, samples)

    def set_waveform(self, waveform_str):
        self.rpc_mgr_server.request("set_waveform",[str(waveform_str)])

    def set_gain(self, gain):
        self.rpc_set_gain(gain)

    def rpc_set_gain(self, gain):
        self.rpc_mgr_server.request("set_k",[gain])

###############################################################################
# Options Parser
###############################################################################
def parse_args():
    """Options parser."""
    parser = ArgumentParser()
    parser.add_argument("-s", "--servername", default="localhost",
                      help="Server hostname")
    parser.add_argument("-c", "--clientname", default="localhost",
                      help="Server hostname")
    args = parser.parse_args()
    return args


###############################################################################
# Main
###############################################################################
if __name__ == "__main__":
    args = parse_args()
    qapp = Qt.QApplication(sys.argv)
    qapp.main_window = gui("Remote GNU Radio GUI", args)
    qapp.main_window.show()
    qapp.exec_()

