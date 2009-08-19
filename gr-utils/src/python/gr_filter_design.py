#!/usr/bin/env python

try:
    import scipy
    from scipy import fftpack
except ImportError:
    print "Please install SciPy to run this script (http://www.scipy.org/)"
    raise SystemExit, 1

import sys, os
from PyQt4 import Qt, QtCore, QtGui
import PyQt4.Qwt5 as Qwt
from optparse import OptionParser
from gnuradio import gr, eng_notation
from scipy import fftpack

from pyqt_filter import Ui_MainWindow
from pyqt_filter_firlpf import Ui_firlpfForm
from pyqt_filter_firhpf import Ui_firhpfForm

class gr_plot_filter(QtGui.QMainWindow):
    def __init__(self, qapp, options):
        QtGui.QWidget.__init__(self, None)
        self.gui = Ui_MainWindow()
        self.gui.setupUi(self)

        self.connect(self.gui.filterTypeComboBox,
                     Qt.SIGNAL("currentIndexChanged(const QString&)"),
                     self.changed_filter_type)
        self.connect(self.gui.filterDesignTypeComboBox,
                     Qt.SIGNAL("currentIndexChanged(const QString&)"),
                     self.changed_filter_design_type)

        self.connect(self.gui.designButton,
                     Qt.SIGNAL("released()"),
                     self.design)
        
        self.fltdeslpf = Ui_firlpfForm()
        self.fltdeshpf = Ui_firhpfForm()

        self.fltdeslpf.setupUi(self.gui.firlpfPage)
        self.fltdeshpf.setupUi(self.gui.firhpfPage)

        # Initialize to LPF
        self.gui.filterTypeWidget.setCurrentWidget(self.gui.firlpfPage)

        # Set up plot curves
        self.rcurve = Qwt.QwtPlotCurve("Real")
        self.rcurve.attach(self.gui.timePlot)

        self.freqcurve = Qwt.QwtPlotCurve("PSD")
        self.freqcurve.attach(self.gui.freqPlot)

        # Create zoom functionality for the plots
        self.timeZoomer = Qwt.QwtPlotZoomer(self.gui.timePlot.xBottom,
                                            self.gui.timePlot.yLeft,
                                            Qwt.QwtPicker.PointSelection,
                                            Qwt.QwtPicker.AlwaysOn,
                                            self.gui.timePlot.canvas())

        self.freqZoomer = Qwt.QwtPlotZoomer(self.gui.freqPlot.xBottom,
                                            self.gui.freqPlot.yLeft,
                                            Qwt.QwtPicker.PointSelection,
                                            Qwt.QwtPicker.AlwaysOn,
                                            self.gui.freqPlot.canvas())

        # Set up pen for colors and line width
        blue = QtGui.qRgb(0x00, 0x00, 0xFF)
        blueBrush = Qt.QBrush(Qt.QColor(blue))
        self.freqcurve.setPen(Qt.QPen(blueBrush, 2))
        self.rcurve.setPen(Qt.QPen(blueBrush, 2))

        self.show()

    def changed_filter_type(self, ftype):
        strftype = str(ftype.toAscii())
        if(ftype == "Low Pass"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firlpfPage)
        elif(ftype == "High Pass"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firhpfPage)
        
    def changed_filter_design_type(self, design):
        print design

    def design(self):
        fs = self.gui.sampleRateEdit.text().toDouble()[0]
        gain = self.gui.filterGainEdit.text().toDouble()[0]
        
        ftype = self.gui.filterTypeComboBox.currentText()
        if(ftype == "Low Pass"):
            taps = self.design_win_lpf(fs, gain)
        elif(ftype == "High Pass"):
            taps = self.design_win_hpf(fs, gain)

        self.update_time_curves(taps)
        self.update_freq_curves(taps)
        
    def design_win_lpf(self, fs, gain):
        pb = self.fltdeslpf.endofPassBandEdit.text().toDouble()[0]
        sb = self.fltdeslpf.startofStopBandEdit.text().toDouble()[0]
        atten = self.fltdeslpf.stopBandAttenEdit.text().toDouble()[0]
        tb = sb - pb

        taps = gr.firdes.low_pass_2(gain, fs, pb, tb, atten,
                                    gr.firdes.WIN_BLACKMAN_hARRIS)
        return taps
    
    def design_win_hpf(self, fs, gain):
        print fs
        print widget

    def update_time_curves(self, taps):
        ntaps = len(taps)
        self.rcurve.setData(scipy.arange(ntaps), taps)

        # Reset the x-axis to the new time scale
        ymax = 1.5 * max(taps)
        ymin = 1.5 * min(taps)
        self.gui.timePlot.setAxisScale(self.gui.timePlot.xBottom,
                                       0, ntaps)
        self.gui.timePlot.setAxisScale(self.gui.timePlot.yLeft,
                                       ymin, ymax)

        # Set the zoomer base to unzoom to the new axis
        self.timeZoomer.setZoomBase()
    
        self.gui.timePlot.replot()
        
    def update_freq_curves(self, taps, Npts=1000):
        fftpts = fftpack.fft(taps, Npts)
        freq = scipy.arange(0, Npts)

        fftdB = 20.0*scipy.log10(abs(fftpts))
        
        self.freqcurve.setData(freq, fftdB)

        self.gui.freqPlot.setAxisScale(self.gui.freqPlot.xBottom,
                                       0, Npts/2)

        # Set the zoomer base to unzoom to the new axis
        self.freqZoomer.setZoomBase()

        self.gui.freqPlot.replot()


def setup_options():
    usage="%prog: [options] (input_filename)"
    description = ""

    parser = OptionParser(conflict_handler="resolve",
                          usage=usage, description=description)
    return parser

def main(args):
    parser = setup_options()
    (options, args) = parser.parse_args ()

    app = Qt.QApplication(args)
    gplt = gr_plot_filter(app, options)
    app.exec_()

if __name__ == '__main__':
    main(sys.argv)

