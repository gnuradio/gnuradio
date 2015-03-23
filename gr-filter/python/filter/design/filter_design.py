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

import sys, os, re, csv, copy
import warnings
from optparse import OptionParser
from gnuradio import filter

try:
    import scipy
    from scipy import fftpack, poly1d, signal
except ImportError:
    print "Please install SciPy to run this script (http://www.scipy.org/)"
    raise SystemExit, 1

try:
    from PyQt4 import Qt, QtCore, QtGui
except ImportError:
    print "Please install PyQt4 to run this script (http://www.riverbankcomputing.co.uk/software/pyqt/download)"
    raise SystemExit, 1

try:
    import PyQt4.Qwt5 as Qwt
except ImportError:
    print "Please install PyQwt5 to run this script (http://pyqwt.sourceforge.net/)"
    raise SystemExit, 1

try:
    from gnuradio.filter.pyqt_filter_stacked import Ui_MainWindow
except ImportError:
    print "Could not import from pyqt_filter_stacked. Please build with \"pyuic4 pyqt_filter_stacked.ui -o pyqt_filter_stacked.py\""
    raise SystemExit, 1

try:
    from gnuradio.filter.banditems import *
except ImportError:
    print "Could not import from banditems. Please check whether banditems.py is in the library path"
    raise SystemExit, 1

try:
    from gnuradio.filter.polezero_plot import *
except ImportError:
    print "Could not import from polezero_plot. Please check whether polezero_plot.py is in the library path"
    raise SystemExit, 1

try:
    from gnuradio.filter.idealbanditems import *
except ImportError:
    print "Could not import from idealbanditems. Please check whether idealbanditems.py is in the library path"
    raise SystemExit, 1

try:
    from gnuradio.filter.api_object import *
except ImportError:
    print "Could not import from api_object. Please check whether api_object.py is in the library path"
    raise SystemExit, 1

try:
    from gnuradio.filter.fir_design import *
except ImportError:
    print "Could not import from fir_design. Please check whether fir_design.py is in the library path"
    raise SystemExit, 1

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s


# Gnuradio Filter design tool main window
class gr_plot_filter(QtGui.QMainWindow):
    def __init__(self, options, callback=None, restype=""):
        QtGui.QWidget.__init__(self, None)
        self.gui = Ui_MainWindow()
        self.callback = callback
        self.gui.setupUi(self)

        #Remove other filter combobox entry if
        #some restriction is specified
        if restype == "iir":
            ind = self.gui.fselectComboBox.findText("FIR")
            if ind != -1:
                self.gui.fselectComboBox.removeItem(ind)
        elif restype == "fir":
            ind = self.gui.fselectComboBox.findText("IIR(scipy)")
            if ind != -1:
                self.gui.fselectComboBox.removeItem(ind)

        self.connect(self.gui.action_save,
                     Qt.SIGNAL("activated()"),
                     self.action_save_dialog)
        self.connect(self.gui.action_open,
                     Qt.SIGNAL("activated()"),
                     self.action_open_dialog)


        self.connect(self.gui.filterTypeComboBox,
                     Qt.SIGNAL("currentIndexChanged(const QString&)"),
                     self.changed_filter_type)
        self.connect(self.gui.iirfilterBandComboBox,
                     Qt.SIGNAL("currentIndexChanged(const QString&)"),
                     self.changed_iirfilter_band)
        self.connect(self.gui.filterDesignTypeComboBox,
                     Qt.SIGNAL("currentIndexChanged(const QString&)"),
                     self.changed_filter_design_type)
        self.connect(self.gui.fselectComboBox,
                     Qt.SIGNAL("currentIndexChanged(const QString&)"),
                     self.changed_fselect)
        self.connect(self.gui.iirfilterTypeComboBox,
                     Qt.SIGNAL("currentIndexChanged(const QString&)"),
                     self.set_order)

        self.connect(self.gui.designButton,
                     Qt.SIGNAL("released()"),
                     self.design)

#        self.connect(self.gui.tabGroup,
#                     Qt.SIGNAL("currentChanged(int)"),
#                     self.tab_changed)

        self.connect(self.gui.nfftEdit,
                     Qt.SIGNAL("textEdited(QString)"),
                     self.nfft_edit_changed)

        self.connect(self.gui.actionQuick_Access,
					 Qt.SIGNAL("activated()"),
					 self.action_quick_access)

        self.connect(self.gui.actionSpec_Widget,
					 Qt.SIGNAL("activated()"),
					 self.action_spec_widget)

        self.connect(self.gui.actionResponse_Widget,
					 Qt.SIGNAL("activated()"),
					 self.action_response_widget)

        self.connect(self.gui.actionDesign_Widget,
					 Qt.SIGNAL("activated()"),
					 self.action_design_widget)

        self.connect(self.gui.actionMagnitude_Response,
					 Qt.SIGNAL("activated()"),
					 self.set_actmagresponse)

        self.connect(self.gui.actionGrid_2,
					 Qt.SIGNAL("activated()"),
					 self.set_actgrid)

        self.connect(self.gui.actionPhase_Respone,
					 Qt.SIGNAL("activated()"),
					 self.set_actphase)

        self.connect(self.gui.actionGroup_Delay,
					 Qt.SIGNAL("activated()"),
					 self.set_actgdelay)

        self.connect(self.gui.actionFilter_Coefficients,
					 Qt.SIGNAL("activated()"),
					 self.set_actfcoeff)

        self.connect(self.gui.actionBand_Diagram,
					 Qt.SIGNAL("activated()"),
					 self.set_actband)

        self.connect(self.gui.actionIdeal_Band,
					 Qt.SIGNAL("activated()"),
					 self.set_drawideal)

        self.connect(self.gui.actionPole_Zero_Plot_2,
					 Qt.SIGNAL("activated()"),
					 self.set_actpzplot)

        self.connect(self.gui.actionGridview,
					 Qt.SIGNAL("activated()"),
					 self.set_switchview)

        self.connect(self.gui.actionPlot_select,
					 Qt.SIGNAL("activated()"),
					 self.set_plotselect)

        self.connect(self.gui.actionPhase_Delay,
					 Qt.SIGNAL("activated()"),
					 self.set_actpdelay)

        self.connect(self.gui.actionImpulse_Response,
					 Qt.SIGNAL("activated()"),
					 self.set_actimpres)

        self.connect(self.gui.actionStep_Response,
					 Qt.SIGNAL("activated()"),
					 self.set_actstepres)

        self.connect(self.gui.mfmagPush,
                     Qt.SIGNAL("released()"),
                     self.set_mfmagresponse)

        self.connect(self.gui.mfphasePush,
                     Qt.SIGNAL("released()"),
                     self.set_mfphaseresponse)

        self.connect(self.gui.mfgpdlyPush,
                     Qt.SIGNAL("released()"),
                     self.set_mfgroupdelay)

        self.connect(self.gui.mfphdlyPush,
                     Qt.SIGNAL("released()"),
                     self.set_mfphasedelay)

        self.connect(self.gui.mfoverlayPush,
                     Qt.SIGNAL("clicked()"),
                     self.set_mfoverlay)

        self.connect(self.gui.conjPush,
                     Qt.SIGNAL("clicked()"),
                     self.set_conj)

        self.connect(self.gui.mconjPush,
                     Qt.SIGNAL("clicked()"),
                     self.set_mconj)

        self.connect(self.gui.addzeroPush,
                     Qt.SIGNAL("clicked()"),
                     self.set_zeroadd)

        self.connect(self.gui.maddzeroPush,
                     Qt.SIGNAL("clicked()"),
                     self.set_mzeroadd)

        self.connect(self.gui.addpolePush,
                     Qt.SIGNAL("clicked()"),
                     self.set_poleadd)

        self.connect(self.gui.maddpolePush,
                     Qt.SIGNAL("clicked()"),
                     self.set_mpoleadd)

        self.connect(self.gui.delPush,
                     Qt.SIGNAL("clicked()"),
                     self.set_delpz)

        self.connect(self.gui.mdelPush,
                     Qt.SIGNAL("clicked()"),
                     self.set_mdelpz)

        self.connect(self.gui.mttapsPush,
                     Qt.SIGNAL("clicked()"),
                     self.set_mttaps)

        self.connect(self.gui.mtstepPush,
                     Qt.SIGNAL("clicked()"),
                     self.set_mtstep)

        self.connect(self.gui.mtimpPush,
                     Qt.SIGNAL("clicked()"),
                     self.set_mtimpulse)

        self.connect(self.gui.checkKeepcur,
					 Qt.SIGNAL("stateChanged(int)"),
					 self.set_bufferplots)

        self.connect(self.gui.checkGrid,
					 Qt.SIGNAL("stateChanged(int)"),
					 self.set_grid)

        self.connect(self.gui.checkMagres,
					 Qt.SIGNAL("stateChanged(int)"),
					 self.set_magresponse)

        self.connect(self.gui.checkGdelay,
					 Qt.SIGNAL("stateChanged(int)"),
					 self.set_gdelay)

        self.connect(self.gui.checkPhase,
					 Qt.SIGNAL("stateChanged(int)"),
					 self.set_phase)

        self.connect(self.gui.checkFcoeff,
					 Qt.SIGNAL("stateChanged(int)"),
					 self.set_fcoeff)

        self.connect(self.gui.checkBand,
					 Qt.SIGNAL("stateChanged(int)"),
					 self.set_band)

        self.connect(self.gui.checkPzplot,
					 Qt.SIGNAL("stateChanged(int)"),
					 self.set_pzplot)

        self.connect(self.gui.checkPdelay,
					 Qt.SIGNAL("stateChanged(int)"),
					 self.set_pdelay)

        self.connect(self.gui.checkImpulse,
					 Qt.SIGNAL("stateChanged(int)"),
					 self.set_impres)

        self.connect(self.gui.checkStep,
					 Qt.SIGNAL("stateChanged(int)"),
					 self.set_stepres)

        self.gridenable = False
        self.mfoverlay  = False
        self.mtoverlay  = False
        self.iir        = False

        self.gui.designButton.setShortcut(QtCore.Qt.Key_Return)

        self.taps = []
        self.a = []
        self.b = []
        self.fftdB = []
        self.fftDeg = []
        self.groupDelay = []
        self.phaseDelay = []
        self.gridview = 0
        self.params = []
        self.nfftpts = int(10000)
        self.gui.nfftEdit.setText(Qt.QString("%1").arg(self.nfftpts))

        self.firFilters = ("Low Pass", "Band Pass", "Complex Band Pass", "Band Notch",
                           "High Pass", "Root Raised Cosine", "Gaussian", "Half Band")
        self.optFilters = ("Low Pass", "Band Pass", "Complex Band Pass",
                           "Band Notch", "High Pass", "Half Band")

        self.set_windowed()

        # Initialize to LPF
        self.gui.filterTypeWidget.setCurrentWidget(self.gui.firlpfPage)
        self.gui.iirfilterTypeComboBox.hide()
        self.gui.iirfilterBandComboBox.hide()
        self.gui.adComboBox.hide()
        self.gui.addpolePush.setEnabled(False)
        self.gui.maddpolePush.setEnabled(False)

        # Set Axis labels
        fxtitle = Qwt.QwtText("Frequency (Hz)")
        fxtitle.setFont(Qt.QFont("Helvetica", 11, Qt.QFont.Bold))
        fytitle = Qwt.QwtText("Magnitude (dB)")
        fytitle.setFont(Qt.QFont("Helvetica", 11, Qt.QFont.Bold))
        self.gui.freqPlot.setAxisTitle(self.gui.freqPlot.xBottom,
                                       fxtitle)
        self.gui.freqPlot.setAxisTitle(self.gui.freqPlot.yLeft,
                                       fytitle)

        txtitle = Qwt.QwtText("Tap number")
        txtitle.setFont(Qt.QFont("Helvetica", 11, Qt.QFont.Bold))
        tytitle = Qwt.QwtText("Amplitude")
        tytitle.setFont(Qt.QFont("Helvetica", 11, Qt.QFont.Bold))
        self.gui.timePlot.setAxisTitle(self.gui.timePlot.xBottom,
                                       txtitle)
        self.gui.timePlot.setAxisTitle(self.gui.timePlot.yLeft,
                                       tytitle)

        pytitle = Qwt.QwtText("Phase (Radians)")
        pytitle.setFont(Qt.QFont("Helvetica", 11, Qt.QFont.Bold))
        self.gui.phasePlot.setAxisTitle(self.gui.phasePlot.xBottom,
                                        fxtitle)
        self.gui.phasePlot.setAxisTitle(self.gui.phasePlot.yLeft,
                                        pytitle)

        gytitle = Qwt.QwtText("Delay (sec)")
        gytitle.setFont(Qt.QFont("Helvetica", 11, Qt.QFont.Bold))
        self.gui.groupPlot.setAxisTitle(self.gui.groupPlot.xBottom,
                                        fxtitle)
        self.gui.groupPlot.setAxisTitle(self.gui.groupPlot.yLeft,
                                        gytitle)

        impytitle = Qwt.QwtText("Amplitude")
        impytitle.setFont(Qt.QFont("Helvetica", 11, Qt.QFont.Bold))
        impxtitle = Qwt.QwtText("n (Samples)")
        impxtitle.setFont(Qt.QFont("Helvetica", 11, Qt.QFont.Bold))
        self.gui.impresPlot.setAxisTitle(self.gui.freqPlot.xBottom,
                                       impxtitle)
        self.gui.impresPlot.setAxisTitle(self.gui.freqPlot.yLeft,
                                       impytitle)
        self.gui.stepresPlot.setAxisTitle(self.gui.freqPlot.xBottom,
                                       impxtitle)
        self.gui.stepresPlot.setAxisTitle(self.gui.freqPlot.yLeft,
                                       impytitle)
        mtytitle = Qwt.QwtText("Amplitude")
        mtytitle.setFont(Qt.QFont("Helvetica", 9, Qt.QFont.Bold))
        mtxtitle = Qwt.QwtText("n (Samples/taps)")
        mtxtitle.setFont(Qt.QFont("Helvetica", 9, Qt.QFont.Bold))
        self.gui.mtimePlot.setAxisTitle(self.gui.freqPlot.xBottom,
                                       mtxtitle)
        self.gui.mtimePlot.setAxisTitle(self.gui.freqPlot.yLeft,
                                       mtytitle)

        phytitle = Qwt.QwtText("Phase Delay")
        phytitle.setFont(Qt.QFont("Helvetica", 11, Qt.QFont.Bold))
        self.gui.pdelayPlot.setAxisTitle(self.gui.groupPlot.xBottom,
                                        fxtitle)
        self.gui.pdelayPlot.setAxisTitle(self.gui.groupPlot.yLeft,
                                        phytitle)

        # Set up plot curves
        self.rcurve = Qwt.QwtPlotCurve("Real")
        self.rcurve.attach(self.gui.timePlot)
        self.icurve = Qwt.QwtPlotCurve("Imag")
        self.icurve.attach(self.gui.timePlot)

        self.freqcurve = Qwt.QwtPlotCurve("PSD")
        self.freqcurve.attach(self.gui.freqPlot)

        self.phasecurve = Qwt.QwtPlotCurve("Phase")
        self.phasecurve.attach(self.gui.phasePlot)

        self.groupcurve = Qwt.QwtPlotCurve("Group Delay")
        self.groupcurve.attach(self.gui.groupPlot)

        self.imprescurve = Qwt.QwtPlotCurve("Impulse Response")
        self.imprescurve.attach(self.gui.impresPlot)

        self.imprescurve_i = Qwt.QwtPlotCurve("Impulse Response Imag")
        self.imprescurve_i.attach(self.gui.impresPlot)

        self.steprescurve = Qwt.QwtPlotCurve("Step Response")
        self.steprescurve.attach(self.gui.stepresPlot)

        self.steprescurve_i = Qwt.QwtPlotCurve("Step Response Imag")
        self.steprescurve_i.attach(self.gui.stepresPlot)

        self.pdelaycurve = Qwt.QwtPlotCurve("Phase Delay")
        self.pdelaycurve.attach(self.gui.pdelayPlot)

        self.idbanditems = IdealBandItems()

        self.set_defaultpen()

        #Antialiasing
        self.rcurve.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
        self.icurve.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
        self.freqcurve.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
        self.phasecurve.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
        self.groupcurve.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
        self.imprescurve.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
        self.imprescurve_i.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
        self.steprescurve.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
        self.steprescurve_i.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
        self.pdelaycurve.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased);

        #Create grid for plots
        self.freqgrid = Qwt.QwtPlotGrid()
        self.phasegrid = Qwt.QwtPlotGrid()
        self.groupgrid = Qwt.QwtPlotGrid()
        self.impresgrid = Qwt.QwtPlotGrid()
        self.stepresgrid = Qwt.QwtPlotGrid()
        self.pdelaygrid = Qwt.QwtPlotGrid()
        self.ftapsgrid = Qwt.QwtPlotGrid()
        self.freqgrid.setPen(Qt.QPen(Qt.Qt.black, 0, Qt.Qt.DotLine))
        self.phasegrid.setPen(Qt.QPen(Qt.Qt.black, 0, Qt.Qt.DotLine))
        self.groupgrid.setPen(Qt.QPen(Qt.Qt.black, 0, Qt.Qt.DotLine))
        self.impresgrid.setPen(Qt.QPen(Qt.Qt.black, 0, Qt.Qt.DotLine))
        self.stepresgrid.setPen(Qt.QPen(Qt.Qt.black, 0, Qt.Qt.DotLine))
        self.pdelaygrid.setPen(Qt.QPen(Qt.Qt.black, 0, Qt.Qt.DotLine))
        self.ftapsgrid.setPen(Qt.QPen(Qt.Qt.black, 0, Qt.Qt.DotLine))

        # Create zoom functionality for the plots
        self.timeZoomer = Qwt.QwtPlotZoomer(self.gui.timePlot.xBottom,
                                            self.gui.timePlot.yLeft,
                                            Qwt.QwtPicker.PointSelection,
                                            Qwt.QwtPicker.AlwaysOn,
                                            self.gui.timePlot.canvas())

        self.mtimeZoomer = Qwt.QwtPlotZoomer(self.gui.mtimePlot.xBottom,
                                            self.gui.mtimePlot.yLeft,
                                            Qwt.QwtPicker.PointSelection,
                                            Qwt.QwtPicker.AlwaysOn,
                                            self.gui.mtimePlot.canvas())

        self.mtimeZoomer2 = Qwt.QwtPlotZoomer(self.gui.mtimePlot.xBottom,
                                            self.gui.mtimePlot.yRight,
                                            Qwt.QwtPicker.PointSelection,
                                            Qwt.QwtPicker.AlwaysOff,
                                            self.gui.mtimePlot.canvas())

        self.freqZoomer = Qwt.QwtPlotZoomer(self.gui.freqPlot.xBottom,
                                            self.gui.freqPlot.yLeft,
                                            Qwt.QwtPicker.PointSelection,
                                            Qwt.QwtPicker.AlwaysOn,
                                            self.gui.freqPlot.canvas())

        self.mfreqZoomer = Qwt.QwtPlotZoomer(self.gui.mfreqPlot.xBottom,
                                             self.gui.mfreqPlot.yLeft,
                                             Qwt.QwtPicker.PointSelection,
                                             Qwt.QwtPicker.AlwaysOn,
                                             self.gui.mfreqPlot.canvas())


        self.mfreqZoomer2 = Qwt.QwtPlotZoomer(self.gui.mfreqPlot.xBottom,
                                              self.gui.mfreqPlot.yRight,
                                              Qwt.QwtPicker.PointSelection,
                                              Qwt.QwtPicker.AlwaysOff,
                                              self.gui.mfreqPlot.canvas())

        self.phaseZoomer = Qwt.QwtPlotZoomer(self.gui.phasePlot.xBottom,
                                             self.gui.phasePlot.yLeft,
                                             Qwt.QwtPicker.PointSelection,
                                             Qwt.QwtPicker.AlwaysOn,
                                             self.gui.phasePlot.canvas())

        self.groupZoomer = Qwt.QwtPlotZoomer(self.gui.groupPlot.xBottom,
                                             self.gui.groupPlot.yLeft,
                                             Qwt.QwtPicker.PointSelection,
                                             Qwt.QwtPicker.AlwaysOn,
                                             self.gui.groupPlot.canvas())

        self.impresZoomer = Qwt.QwtPlotZoomer(self.gui.groupPlot.xBottom,
                                             self.gui.groupPlot.yLeft,
                                             Qwt.QwtPicker.PointSelection,
                                             Qwt.QwtPicker.AlwaysOn,
                                             self.gui.impresPlot.canvas())

        self.stepresZoomer = Qwt.QwtPlotZoomer(self.gui.groupPlot.xBottom,
                                             self.gui.groupPlot.yLeft,
                                             Qwt.QwtPicker.PointSelection,
                                             Qwt.QwtPicker.AlwaysOn,
                                             self.gui.stepresPlot.canvas())

        self.pdelayZoomer = Qwt.QwtPlotZoomer(self.gui.groupPlot.xBottom,
                                             self.gui.groupPlot.yLeft,
                                             Qwt.QwtPicker.PointSelection,
                                             Qwt.QwtPicker.AlwaysOn,
                                             self.gui.pdelayPlot.canvas())


		#Assigning items
        self.lpfitems = lpfItems
        self.hpfitems = hpfItems
        self.bpfitems = bpfItems
        self.bnfitems = bnfItems


        #Connect signals
        self.lpfitems[0].attenChanged.connect(self.set_fatten)
        self.hpfitems[0].attenChanged.connect(self.set_fatten)
        self.bpfitems[0].attenChanged.connect(self.set_fatten)
        self.bnfitems[0].attenChanged.connect(self.set_fatten)

        #Populate the Band-diagram scene
        self.scene = QtGui.QGraphicsScene()
        self.scene.setSceneRect(0,0,250,250)
        lightback = QtGui.qRgb(0xf8, 0xf8, 0xff)
        backbrush = Qt.QBrush(Qt.QColor(lightback))
        self.scene.setBackgroundBrush(backbrush)
        self.gui.bandView.setScene(self.scene)
        self.gui.mbandView.setScene(self.scene)

        #Install Canvas picker for pz-plot
        self.cpicker = CanvasPicker(self.gui.pzPlot)
        self.cpicker.curveChanged.connect(self.set_curvetaps)
        self.cpicker.mouseposChanged.connect(self.set_statusbar)

        self.cpicker2 = CanvasPicker(self.gui.mpzPlot)
        self.cpicker2.curveChanged.connect(self.set_mcurvetaps)
        self.cpicker2.mouseposChanged.connect(self.set_mstatusbar)
        #Edit boxes for band-diagrams (Not required as far as now)
        """
        self.lpfpassEdit=QtGui.QLineEdit()
        self.lpfpassEdit.setMaximumSize(QtCore.QSize(75,20))
        self.lpfpassEdit.setText(Qt.QString("Not set"))
        self.lpfstartproxy=QtGui.QGraphicsProxyWidget()
        self.lpfstartproxy.setWidget(self.lpfpassEdit)
        self.lpfstartproxy.setPos(400,30)

		self.lpfstopEdit=QtGui.QLineEdit()
        self.lpfstopEdit.setMaximumSize(QtCore.QSize(75,20))
        self.lpfstopEdit.setText(Qt.QString("Not set"))
        self.lpfstopproxy=QtGui.QGraphicsProxyWidget()
        self.lpfstopproxy.setWidget(self.lpfstopEdit)
        self.lpfstopproxy.setPos(400,50)
        self.lpfitems.append(self.lpfstartproxy)
        self.lpfitems.append(self.lpfstopproxy)
        """
        self.populate_bandview(self.lpfitems)

        # Set up validators for edit boxes
        self.intVal = Qt.QIntValidator(None)
        self.dblVal = Qt.QDoubleValidator(None)
        self.gui.nfftEdit.setValidator(self.intVal)
        self.gui.sampleRateEdit.setValidator(self.dblVal)
        self.gui.filterGainEdit.setValidator(self.dblVal)
        self.gui.endofLpfPassBandEdit.setValidator(self.dblVal)
        self.gui.startofLpfStopBandEdit.setValidator(self.dblVal)
        self.gui.lpfStopBandAttenEdit.setValidator(self.dblVal)
        self.gui.lpfPassBandRippleEdit.setValidator(self.dblVal)
        self.gui.startofBpfPassBandEdit.setValidator(self.dblVal)
        self.gui.endofBpfPassBandEdit.setValidator(self.dblVal)
        self.gui.bpfTransitionEdit.setValidator(self.dblVal)
        self.gui.bpfStopBandAttenEdit.setValidator(self.dblVal)
        self.gui.bpfPassBandRippleEdit.setValidator(self.dblVal)
        self.gui.startofBnfStopBandEdit.setValidator(self.dblVal)
        self.gui.endofBnfStopBandEdit.setValidator(self.dblVal)
        self.gui.bnfTransitionEdit.setValidator(self.dblVal)
        self.gui.bnfStopBandAttenEdit.setValidator(self.dblVal)
        self.gui.bnfPassBandRippleEdit.setValidator(self.dblVal)
        self.gui.endofHpfStopBandEdit.setValidator(self.dblVal)
        self.gui.startofHpfPassBandEdit.setValidator(self.dblVal)
        self.gui.hpfStopBandAttenEdit.setValidator(self.dblVal)
        self.gui.hpfPassBandRippleEdit.setValidator(self.dblVal)
        self.gui.rrcSymbolRateEdit.setValidator(self.dblVal)
        self.gui.rrcAlphaEdit.setValidator(self.dblVal)
        self.gui.rrcNumTapsEdit.setValidator(self.dblVal)
        self.gui.gausSymbolRateEdit.setValidator(self.dblVal)
        self.gui.gausBTEdit.setValidator(self.dblVal)
        self.gui.gausNumTapsEdit.setValidator(self.dblVal)

        self.gui.nTapsEdit.setText("0")

        self.filterWindows = {"Hamming Window" : filter.firdes.WIN_HAMMING,
                              "Hann Window" : filter.firdes.WIN_HANN,
                              "Blackman Window" : filter.firdes.WIN_BLACKMAN,
                              "Rectangular Window" : filter.firdes.WIN_RECTANGULAR,
                              "Kaiser Window" : filter.firdes.WIN_KAISER,
                              "Blackman-harris Window" : filter.firdes.WIN_BLACKMAN_hARRIS}
        self.EQUIRIPPLE_FILT = 6 # const for equiripple filter window types
        self.show()

    # Set up pen for colors and line width
    def set_defaultpen(self):
        blue = QtGui.qRgb(0x00, 0x00, 0xFF)
        blueBrush = Qt.QBrush(Qt.QColor(blue))
        red = QtGui.qRgb(0xFF, 0x00, 0x00)
        redBrush = Qt.QBrush(Qt.QColor(red))
        self.freqcurve.setPen(Qt.QPen(blueBrush, 1))
        self.rcurve.setPen(Qt.QPen(Qt.Qt.white, 0, Qt.Qt.NoPen))
        self.rcurve.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse,
                                      Qt.QBrush(Qt.Qt.gray),
                                      Qt.QPen(Qt.Qt.blue),
                                      Qt.QSize(8, 8)))

        self.icurve.setPen(Qt.QPen(Qt.Qt.white, 0, Qt.Qt.NoPen))
        self.icurve.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse,
                                      Qt.QBrush(Qt.Qt.gray),
                                      Qt.QPen(Qt.Qt.red),
                                      Qt.QSize(8, 8)))

        self.imprescurve.setPen(Qt.QPen(Qt.Qt.white, 0, Qt.Qt.NoPen))
        self.imprescurve.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse,
                                      Qt.QBrush(Qt.Qt.gray),
                                      Qt.QPen(Qt.Qt.blue),
                                      Qt.QSize(8, 8)))

        self.imprescurve_i.setPen(Qt.QPen(Qt.Qt.white, 0, Qt.Qt.NoPen))
        self.imprescurve_i.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse,
                                      Qt.QBrush(Qt.Qt.gray),
                                      Qt.QPen(Qt.Qt.red),
                                      Qt.QSize(8, 8)))

        self.steprescurve.setPen(Qt.QPen(Qt.Qt.white, 0, Qt.Qt.NoPen))
        self.steprescurve.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse,
                                      Qt.QBrush(Qt.Qt.gray),
                                      Qt.QPen(Qt.Qt.blue),
                                      Qt.QSize(8, 8)))

        self.steprescurve_i.setPen(Qt.QPen(Qt.Qt.white, 0, Qt.Qt.NoPen))
        self.steprescurve_i.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse,
                                      Qt.QBrush(Qt.Qt.gray),
                                      Qt.QPen(Qt.Qt.red),
                                      Qt.QSize(8, 8)))

        self.phasecurve.setPen(Qt.QPen(blueBrush, 1))
        self.groupcurve.setPen(Qt.QPen(blueBrush, 1))
        self.pdelaycurve.setPen(Qt.QPen(blueBrush, 1))
        self.idbanditems.setLinetype()

    def changed_fselect(self, ftype):
        strftype = str(ftype.toAscii())
        if(ftype == "FIR"):
            self.gui.iirfilterTypeComboBox.hide()
            self.gui.iirfilterBandComboBox.hide()
            self.gui.adComboBox.hide()
            self.gui.filterDesignTypeComboBox.show()
            self.gui.globalParamsBox.show()
            self.gui.filterTypeComboBox.show()
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firlpfPage)
            self.gui.tabGroup.addTab(self.gui.timeTab, _fromUtf8("Filter Taps"))
            self.gui.mttapsPush.setEnabled(True)
            self.gui.addpolePush.setEnabled(False)
            self.gui.maddpolePush.setEnabled(False)
        elif(ftype == "IIR(scipy)"):
            self.gui.filterDesignTypeComboBox.hide()
            self.gui.globalParamsBox.hide()
            self.gui.filterTypeComboBox.hide()
            self.gui.iirfilterTypeComboBox.show()
            self.gui.adComboBox.show()
            self.gui.iirfilterBandComboBox.show()
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.iirlpfPage)
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.timeTab))
            self.gui.mttapsPush.setEnabled(False)
            self.gui.addpolePush.setEnabled(True)
            self.gui.maddpolePush.setEnabled(True)

#self.design()

    def set_order(self, ftype):
        strftype = str(ftype.toAscii())
        if(ftype == "Bessel"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.iirbesselPage)
        else:
            self.changed_iirfilter_band(self.gui.iirfilterBandComboBox.currentText())

#self.design()

    def changed_iirfilter_band(self, ftype):
        strftype = str(ftype.toAscii())
        iirftype = str(self.gui.iirfilterTypeComboBox.currentText().toAscii())
        if(ftype == "Low Pass"):
            if(iirftype == "Bessel"):
                self.gui.filterTypeWidget.setCurrentWidget(self.gui.iirbesselPage)
            else:
                self.gui.filterTypeWidget.setCurrentWidget(self.gui.iirlpfPage)
        elif(ftype == "Band Pass"):
            if(iirftype == "Bessel"):
                self.gui.filterTypeWidget.setCurrentWidget(self.gui.iirbesselPage)
            else:
                self.gui.filterTypeWidget.setCurrentWidget(self.gui.iirbpfPage)
        elif(ftype == "Band Stop"):
            if(iirftype == "Bessel"):
                self.gui.filterTypeWidget.setCurrentWidget(self.gui.iirbesselPage)
            else:
                self.gui.filterTypeWidget.setCurrentWidget(self.gui.iirbsfPage)
        elif(ftype == "High Pass"):
            if(iirftype == "Bessel"):
                self.gui.filterTypeWidget.setCurrentWidget(self.gui.iirbesselPage)
            else:
                self.gui.filterTypeWidget.setCurrentWidget(self.gui.iirhpfPage)

#self.design()

    def changed_filter_type(self, ftype):
        strftype = str(ftype.toAscii())
        if(ftype == "Low Pass"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firlpfPage)
            self.remove_bandview()
            self.populate_bandview(self.lpfitems)
        elif(ftype == "Band Pass"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firbpfPage)
            self.remove_bandview()
            self.populate_bandview(self.bpfitems)
        elif(ftype == "Complex Band Pass"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firbpfPage)
            self.remove_bandview()
            self.populate_bandview(self.bpfitems)
        elif(ftype == "Band Notch"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firbnfPage)
            self.remove_bandview()
            self.populate_bandview(self.bnfitems)
        elif(ftype == "High Pass"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firhpfPage)
            self.remove_bandview()
            self.populate_bandview(self.hpfitems)
        elif(ftype == "Root Raised Cosine"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.rrcPage)
        elif(ftype == "Gaussian"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.gausPage)
        elif(ftype == "Half Band"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firhbPage)

#self.design()

    def changed_filter_design_type(self, design):
        if(design == "Equiripple"):
            self.set_equiripple()
        else:
            self.set_windowed()

#self.design()

    def set_equiripple(self):
        # Stop sending the signal for this function
        self.gui.filterTypeComboBox.blockSignals(True)

        self.equiripple = True
        self.gui.lpfPassBandRippleLabel.setVisible(True)
        self.gui.lpfPassBandRippleEdit.setVisible(True)
        self.gui.bpfPassBandRippleLabel.setVisible(True)
        self.gui.bpfPassBandRippleEdit.setVisible(True)
        self.gui.bnfPassBandRippleLabel.setVisible(True)
        self.gui.bnfPassBandRippleEdit.setVisible(True)
        self.gui.hpfPassBandRippleLabel.setVisible(True)
        self.gui.hpfPassBandRippleEdit.setVisible(True)

        # Save current type and repopulate the combo box for
        # filters this window type can handle
        currenttype = self.gui.filterTypeComboBox.currentText()
        items = self.gui.filterTypeComboBox.count()
        for i in xrange(items):
            self.gui.filterTypeComboBox.removeItem(0)
        self.gui.filterTypeComboBox.addItems(self.optFilters)

        # If the last filter type was valid for this window type,
        # go back to it; otherwise, reset
        try:
            index = self.optFilters.index(currenttype)
            self.gui.filterTypeComboBox.setCurrentIndex(index)
        except ValueError:
            pass

        # Tell gui its ok to start sending this signal again
        self.gui.filterTypeComboBox.blockSignals(False)

    def set_windowed(self):
        # Stop sending the signal for this function
        self.gui.filterTypeComboBox.blockSignals(True)

        self.equiripple = False
        self.gui.lpfPassBandRippleLabel.setVisible(False)
        self.gui.lpfPassBandRippleEdit.setVisible(False)
        self.gui.bpfPassBandRippleLabel.setVisible(False)
        self.gui.bpfPassBandRippleEdit.setVisible(False)
        self.gui.bnfPassBandRippleLabel.setVisible(False)
        self.gui.bnfPassBandRippleEdit.setVisible(False)
        self.gui.hpfPassBandRippleLabel.setVisible(False)
        self.gui.hpfPassBandRippleEdit.setVisible(False)

        # Save current type and repopulate the combo box for
        # filters this window type can handle
        currenttype = self.gui.filterTypeComboBox.currentText()
        items = self.gui.filterTypeComboBox.count()
        for i in xrange(items):
            self.gui.filterTypeComboBox.removeItem(0)
        self.gui.filterTypeComboBox.addItems(self.firFilters)

        # If the last filter type was valid for this window type,
        # go back to it; otherwise, reset
        try:
            index = self.optFilters.index(currenttype)
            self.gui.filterTypeComboBox.setCurrentIndex(index)
        except ValueError:
            pass

        # Tell gui its ok to start sending this signal again
        self.gui.filterTypeComboBox.blockSignals(False)

    def design(self):
        ret = True
        fs,r = self.gui.sampleRateEdit.text().toDouble()
        ret = r and ret
        gain,r = self.gui.filterGainEdit.text().toDouble()
        ret = r and ret

        winstr = str(self.gui.filterDesignTypeComboBox.currentText().toAscii())
        ftype = str(self.gui.filterTypeComboBox.currentText().toAscii())
        fsel = str(self.gui.fselectComboBox.currentText().toAscii())

        if (fsel == "FIR"):
            self.b, self.a=[],[]
            if(ret):
                self.design_fir(ftype, fs, gain, winstr)
        elif (fsel == "IIR(scipy)"):
            with warnings.catch_warnings(record=True) as w:
                # Cause all warnings to always be triggered.
                warnings.simplefilter("always")
                self.design_iir()
                if len(w):
                    reply = QtGui.QMessageBox.information(self, "BadCoefficients",
                                                         str(w[-1].message),"&Ok")

    # Do FIR design
    def design_fir(self, ftype, fs, gain, winstr):
        self.iir = False
        self.cpicker.set_iir(False)
        self.cpicker2.set_iir(False)
        if(winstr == "Equiripple"):
            designer = {"Low Pass" : design_opt_lpf,
                        "Band Pass" : design_opt_bpf,
                        "Complex Band Pass" : design_opt_cbpf,
                        "Band Notch" : design_opt_bnf,
                        "Half Band" : design_opt_hb,
                        "High Pass" :  design_opt_hpf}
            taps,params,r = designer[ftype](fs, gain, self)

        else:
            designer = {"Low Pass" : design_win_lpf,
                        "Band Pass" : design_win_bpf,
                        "Complex Band Pass" : design_win_cbpf,
                        "Band Notch" : design_win_bnf,
                        "High Pass" :  design_win_hpf,
                        "Half Band" : design_win_hb,
                        "Root Raised Cosine" :  design_win_rrc,
                        "Gaussian" :  design_win_gaus}
            wintype = self.filterWindows[winstr]
            taps,params,r = designer[ftype](fs, gain, wintype, self)
        if(r):
            if self.gridview:
                self.params = params
                self.update_fft(taps, params)
                self.set_mfmagresponse()
                self.set_mttaps()
                self.gui.nTapsEdit.setText(Qt.QString("%1").arg(self.taps.size))
            else:
                self.draw_plots(taps,params)
        zeros=self.get_zeros()
        poles=self.get_poles()
        self.gui.pzPlot.insertZeros(zeros)
        self.gui.pzPlot.insertPoles(poles)
        self.gui.mpzPlot.insertZeros(zeros)
        self.gui.mpzPlot.insertPoles(poles)
        self.update_fcoeff()
        self.set_drawideal()
        #return taps if callback is enabled
        if self.callback:
            retobj = ApiObject()
            retobj.update_all("fir", self.params, self.taps, 1)
            self.callback(retobj)

    # Do IIR design
    def design_iir(self):
        iirftype = str(self.gui.iirfilterTypeComboBox.currentText().toAscii())
        iirbtype = str(self.gui.iirfilterBandComboBox.currentText().toAscii())
        atype = str(self.gui.adComboBox.currentText().toAscii())
        self.taps = []
        self.iir = True
        ret = True
        params = []
        besselparams = []
        self.cpicker.set_iir(True)
        self.cpicker2.set_iir(True)

        iirft = 	{"Elliptic" : 'ellip',
                     "Butterworth" : 'butter',
                     "Chebyshev-1" : 'cheby1',
                     "Chebyshev-2" : 'cheby2',
                     "Bessel" : 'bessel'  }

        sanalog = 	{"Analog (rad/second)" : 1,
                     "Digital (normalized 0-1)" : 0  }

        paramtype = { 1 : "analog",
                      0 : "digital" }
        iirabbr = {
                    "Low Pass" : "lpf",
                    "High Pass" : "hpf",
                    "Band Pass" : "bpf",
                    "Band Stop" : "bnf"  }

        iirboxes = {"Low Pass" : [self.gui.iirendofLpfPassBandEdit.text().toDouble(),
                                  self.gui.iirstartofLpfStopBandEdit.text().toDouble(),
                                  self.gui.iirLpfPassBandAttenEdit.text().toDouble(),
                                  self.gui.iirLpfStopBandRippleEdit.text().toDouble()],

                    "High Pass" : [self.gui.iirstartofHpfPassBandEdit.text().toDouble(),
                                   self.gui.iirendofHpfStopBandEdit.text().toDouble(),
                                   self.gui.iirHpfPassBandAttenEdit.text().toDouble(),
                                   self.gui.iirHpfStopBandRippleEdit.text().toDouble()],

                    "Band Pass" : [self.gui.iirstartofBpfPassBandEdit.text().toDouble(),
                                   self.gui.iirendofBpfPassBandEdit.text().toDouble(),
                                   self.gui.iirendofBpfStopBandEdit1.text().toDouble(),
                                   self.gui.iirstartofBpfStopBandEdit2.text().toDouble(),
                                   self.gui.iirBpfPassBandAttenEdit.text().toDouble(),
                                   self.gui.iirBpfStopBandRippleEdit.text().toDouble()],

                    "Band Stop" : [self.gui.iirendofBsfPassBandEdit1.text().toDouble(),
                                   self.gui.iirstartofBsfPassBandEdit2.text().toDouble(),
                                   self.gui.iirstartofBsfStopBandEdit.text().toDouble(),
                                   self.gui.iirendofBsfStopBandEdit.text().toDouble(),
                                   self.gui.iirBsfPassBandAttenEdit.text().toDouble(),
                                   self.gui.iirBsfStopBandRippleEdit.text().toDouble()]  }
        #Remove Ideal band-diagrams if IIR
        self.set_drawideal()
        for i in range(len(iirboxes[iirbtype])):
            params.append(iirboxes[iirbtype][i][0])
            ret = iirboxes[iirbtype][i][1] and ret

        if len(iirboxes[iirbtype]) == 6:
            params=[params[:2],params[2:4],params[4],params[5]]

        if(iirftype == "Bessel"):
            ret = True
            order,r = self.gui.besselordEdit.text().toDouble()
            if iirbtype == "Low Pass" or iirbtype == "High Pass":
                val,r = self.gui.iirbesselcritEdit1.text().toDouble()
                ret = ret and r
                besselparams.append(val)
            else:
                val,r = self.gui.iirbesselcritEdit1.text().toDouble()
                ret = ret and r
                besselparams.append(val)
                val,r = self.gui.iirbesselcritEdit2.text().toDouble()
                ret = ret and r
                besselparams.append(val)

        if(ret):
            if(iirftype == "Bessel"):
                try:
                    (self.b,self.a) = signal.iirfilter(order, besselparams, btype=iirbtype.replace(' ','').lower(),
                                                       analog=sanalog[atype], ftype=iirft[iirftype], output='ba')
                except StandardError, e:
                                reply = QtGui.QMessageBox.information(self, "IIR design error",
                                                                      e.args[0], "&Ok")
                (self.z,self.p,self.k) = signal.tf2zpk(self.b,self.a)
                iirparams = { "filttype": iirft[iirftype],"bandtype": iirabbr[iirbtype], "filtord": order, "paramtype":paramtype[sanalog[atype]],
                              "critfreq": besselparams}
            else:
                try:
                    (self.b,self.a) = signal.iirdesign(params[0], params[1], params[2],
                                             params[3], analog=sanalog[atype], ftype=iirft[iirftype], output='ba')
                except StandardError, e:
                                reply = QtGui.QMessageBox.information(self, "IIR design error",
                                                                      e.args[0], "&Ok")
                (self.z,self.p,self.k) = signal.tf2zpk(self.b,self.a)
                #Create params
                iirparams = { "filttype": iirft[iirftype], "bandtype": iirabbr[iirbtype],"paramtype":paramtype[sanalog[atype]],
                              "pbedge": params[0], "sbedge": params[1],"gpass": params[2],
                              "gstop": params[3]}
            self.gui.pzPlot.insertZeros(self.z)
            self.gui.pzPlot.insertPoles(self.p)
            self.gui.mpzPlot.insertZeros(self.z)
            self.gui.mpzPlot.insertPoles(self.p)
            self.iir_plot_all(self.z,self.p,self.k)
            self.update_fcoeff()
            self.gui.nTapsEdit.setText("-")
            self.params = iirparams
            #return api_object if callback is enabled
            if self.callback:
                retobj = ApiObject()
                retobj.update_all("iir", self.params, (self.b, self.a), 1)
                self.callback(retobj)

    # IIR Filter design plot updates
    def iir_plot_all(self,z,p,k):
        self.b,self.a = signal.zpk2tf(z,p,k)
        w,h = signal.freqz(self.b,self.a)
        self.fftdB = 20 * scipy.log10 (abs(h))
        self.freq = w/max(w)
        self.fftDeg = scipy.unwrap(scipy.arctan2(scipy.imag(h),scipy.real(h)))
        self.groupDelay = -scipy.diff(self.fftDeg)
        self.phaseDelay = -self.fftDeg[1:]/self.freq[1:]
        if self.gridview:
           self.set_mfmagresponse()
           self.set_mtimpulse()
        else:
           self.update_freq_curves()
           self.update_phase_curves()
           self.update_group_curves()
           self.update_pdelay_curves()
           self.update_step_curves()
           self.update_imp_curves()


    def nfft_edit_changed(self, nfft):
        infft,r = nfft.toInt()
        if(r and (infft != self.nfftpts)):
            self.nfftpts = infft
            self.update_freq_curves()

#    def tab_changed(self, tab):
#        if(tab == 0):
#            self.update_freq_curves()
#        if(tab == 1):
#            self.update_time_curves()
#        if(tab == 2):
#            self.update_phase_curves()
#        if(tab == 3):
#            self.update_group_curves()

    def get_fft(self, fs, taps, Npts):
        Ts = 1.0/fs
        fftpts = fftpack.fft(taps, Npts)
        self.freq = scipy.arange(0, fs, 1.0/(Npts*Ts))
        with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            self.fftdB = 20.0*scipy.log10(abs(fftpts))
            if any(self.fftdB == float('-inf')):
                sys.stderr.write('Filter design failed (taking log10 of 0).\n')
                self.fftdB = scipy.zeros([len(fftpts)])

        self.fftDeg = scipy.unwrap(scipy.angle(fftpts))
        self.groupDelay = -scipy.diff(self.fftDeg)
        self.phaseDelay = -self.fftDeg[1:]/self.freq[1:]

    def update_time_curves(self):
        ntaps = len(self.taps)
        if(ntaps > 0):
            if(type(self.taps[0]) == scipy.complex128):
                self.rcurve.setData(scipy.arange(ntaps), self.taps.real)
                self.icurve.setData(scipy.arange(ntaps), self.taps.imag)
                ymax = 1.5 * max(max(self.taps.real),max(self.taps.imag))
                ymin = 1.5 * min(min(self.taps.real),min(self.taps.imag))
            else:
                self.rcurve.setData(scipy.arange(ntaps), self.taps)
                self.icurve.setData([],[]);
                ymax = 1.5 * max(self.taps)
                ymin = 1.5 * min(self.taps)


            # Reset the x-axis to the new time scale
            self.gui.timePlot.setAxisScale(self.gui.timePlot.xBottom,
                                           0, ntaps)
            self.gui.timePlot.setAxisScale(self.gui.timePlot.yLeft,
                                           ymin, ymax)

            if self.mtoverlay:
                self.gui.mtimePlot.setAxisScale(self.rcurve.yAxis(),
                                                ymin, ymax)
                self.mtimeZoomer2.setEnabled(True)
                self.mtimeZoomer2.setZoomBase()
            else:
                self.mtimeZoomer2.setEnabled(False)
                self.gui.mtimePlot.enableAxis(Qwt.QwtPlot.yRight,False)
                self.gui.mtimePlot.setAxisScale(self.gui.mtimePlot.yLeft,
                                                ymin, ymax)
            # Set the zoomer base to unzoom to the new axis
            self.timeZoomer.setZoomBase()
            self.mtimeZoomer.setZoomBase()

            self.gui.timePlot.replot()
            self.gui.mtimePlot.replot()

    def update_step_curves(self):
        ntaps = len(self.taps)
        if(ntaps > 0 or self.iir):
            if self.iir:
                stepres=self.step_response(self.b,self.a)
                ntaps=50
            else:
                stepres=self.step_response(self.taps)
            if(type(stepres[0]) == scipy.complex128):
                self.steprescurve.setData(scipy.arange(ntaps), stepres.real)
                self.steprescurve_i.setData(scipy.arange(ntaps), stepres.imag)
                symax = 1.5 * max(max(stepres.real),max(stepres.imag))
                symin = 1.5 * min(min(stepres.real),min(stepres.imag))
            else:
                self.steprescurve.setData(scipy.arange(ntaps), stepres)
                self.steprescurve_i.setData([],[])
                symax = 1.5 * max(stepres)
                symin = 1.5 * min(stepres)

            # Reset the x-axis to the new time scale

            self.gui.stepresPlot.setAxisScale(self.gui.stepresPlot.xBottom,
                                           0, ntaps)
            self.gui.stepresPlot.setAxisScale(self.gui.stepresPlot.yLeft,
                                           symin, symax)
            if self.mtoverlay:
                self.gui.mtimePlot.setAxisScale(self.steprescurve.yAxis(),
                                                symin, symax)
                self.mtimeZoomer2.setEnabled(True)
                self.mtimeZoomer2.setZoomBase()
            else:
                self.mtimeZoomer2.setEnabled(False)
                self.gui.mtimePlot.enableAxis(Qwt.QwtPlot.yRight,False)
                self.gui.mtimePlot.setAxisScale(self.gui.mtimePlot.yLeft,
                                                symin, symax)

            # Set the zoomer base to unzoom to the new axis
            self.mtimeZoomer.setZoomBase()
            self.stepresZoomer.setZoomBase()

            self.gui.mtimePlot.replot()
            self.gui.stepresPlot.replot()

    def update_imp_curves(self):
        ntaps = len(self.taps)
        if(ntaps > 0 or self.iir):
            if self.iir:
                impres=self.impulse_response(self.b, self.a)
                ntaps=50
            else:
                impres=self.impulse_response(self.taps)
            if(type(impres[0]) == scipy.complex128):
                self.imprescurve.setData(scipy.arange(ntaps), impres.real)
                self.imprescurve_i.setData(scipy.arange(ntaps), impres.imag)
                iymax = 1.5 * max(max(impres.real),max(impres.imag))
                iymin = 1.5 * min(min(impres.real),min(impres.imag))
            else:
                self.imprescurve.setData(scipy.arange(ntaps), impres)
                self.imprescurve_i.setData([],[])
                iymax = 1.5 * max(impres)
                iymin = 1.5 * min(impres)

            # Reset the x-axis to the new time scale

            self.gui.impresPlot.setAxisScale(self.gui.impresPlot.xBottom,
                                           0, ntaps)
            self.gui.impresPlot.setAxisScale(self.gui.impresPlot.yLeft,
                                           iymin, iymax)

            if self.mtoverlay:
                self.gui.mtimePlot.setAxisScale(self.imprescurve.yAxis(),
                                                iymin, iymax)
                self.mtimeZoomer2.setEnabled(True)
                self.mtimeZoomer2.setZoomBase()
            else:
                self.mtimeZoomer2.setEnabled(False)
                self.gui.mtimePlot.enableAxis(Qwt.QwtPlot.yRight,False)
                self.gui.mtimePlot.setAxisScale(self.gui.mtimePlot.yLeft,
                                                iymin, iymax)

            # Set the zoomer base to unzoom to the new axis
            self.mtimeZoomer.setZoomBase()
            self.impresZoomer.setZoomBase()

            self.gui.mtimePlot.replot()
            self.gui.impresPlot.replot()

    def update_freq_curves(self):
        npts = len(self.fftdB)
        fxtitle=Qwt.QwtText("Frequency (Hz)")
        fxtitle.setFont(Qt.QFont("Helvetica", 9, Qt.QFont.Bold))
        fytitle=Qwt.QwtText("Magnitude (dB)")
        fytitle.setFont(Qt.QFont("Helvetica", 9, Qt.QFont.Bold))
        if(npts > 0):
            self.freqcurve.setData(self.freq, self.fftdB)

            # Reset the x-axis to the new time scale
            if self.iir:
                xmax = self.freq[npts-1]
                ymax = self.fftdB.max()
                if(ymax < 0):
                    ymax = 0.8 * self.fftdB.max()
                ymin = 1.1 * self.fftdB.min()
            else:
                xmax = self.freq[npts/2]
                ymax = 1.5 * max(self.fftdB[0:npts/2])
                ymin = 1.1 * min(self.fftdB[0:npts/2])
            xmin = self.freq[0]
            self.gui.freqPlot.setAxisScale(self.gui.freqPlot.xBottom,
                                           xmin, xmax)
            self.gui.freqPlot.setAxisScale(self.gui.freqPlot.yLeft,
                                           ymin, ymax)

            if self.mfoverlay:
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.xBottom,
                                                xmin, xmax)
                self.gui.mfreqPlot.setAxisScale(self.freqcurve.yAxis(),
                                                ymin, ymax)
                self.mfreqZoomer2.setEnabled(True)
                self.mfreqZoomer2.setZoomBase()
            else:
                self.mfreqZoomer2.setEnabled(False)
                self.gui.mfreqPlot.enableAxis(Qwt.QwtPlot.yRight,False)
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.xBottom,
                                                xmin, xmax)
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.yLeft,
                                                ymin, ymax)
            #Set Axis title
            self.gui.mfreqPlot.setAxisTitle(self.freqcurve.yAxis(),
						                        fytitle)
            self.gui.mfreqPlot.setAxisTitle(self.freqcurve.xAxis(),
						                        fxtitle)
            # Set the zoomer base to unzoom to the new axis
            self.freqZoomer.setZoomBase()
            self.mfreqZoomer.setZoomBase()

            self.gui.freqPlot.replot()
            self.gui.mfreqPlot.replot()


    def update_phase_curves(self):
        pytitle=Qwt.QwtText("Phase (Radians)")
        pytitle.setFont(Qt.QFont("Helvetica", 9, Qt.QFont.Bold))
        npts = len(self.fftDeg)
        if(npts > 0):
            self.phasecurve.setData(self.freq, self.fftDeg)

            # Reset the x-axis to the new time scale
            if self.iir:
                xmax = self.freq[npts-1]
                ymax = self.fftDeg.max()
                if(ymax < 0):
                    ymax = 0.8 * self.fftDeg.max()
                ymin = 1.1 * self.fftDeg.min()
            else:
                ymax = 1.5 * max(self.fftDeg[0:npts/2])
                ymin = 1.1 * min(self.fftDeg[0:npts/2])
                xmax = self.freq[npts/2]
            xmin = self.freq[0]
            self.gui.phasePlot.setAxisScale(self.gui.phasePlot.xBottom,
                                            xmin, xmax)
            self.gui.phasePlot.setAxisScale(self.gui.phasePlot.yLeft,
                                            ymin, ymax)

            if self.mfoverlay:
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.xBottom,
                                                xmin, xmax)
                self.gui.mfreqPlot.setAxisScale(self.phasecurve.yAxis(),
                                                ymin, ymax)
                self.mfreqZoomer2.setEnabled(True)
                self.mfreqZoomer2.setZoomBase()
            else:
                self.mfreqZoomer2.setEnabled(False)
                self.gui.mfreqPlot.enableAxis(Qwt.QwtPlot.yRight,False)
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.xBottom,
                                                xmin, xmax)
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.yLeft,
                                                ymin, ymax)

            #Set Axis title
            self.gui.mfreqPlot.setAxisTitle(self.phasecurve.yAxis(),
						                        pytitle)

            # Set the zoomer base to unzoom to the new axis
            self.phaseZoomer.setZoomBase()
            self.mfreqZoomer.setZoomBase()

            self.gui.phasePlot.replot()
            self.gui.mfreqPlot.replot()


    def update_group_curves(self):
        gytitle=Qwt.QwtText("Delay (sec)")
        gytitle.setFont(Qt.QFont("Helvetica", 9, Qt.QFont.Bold))
        npts = len(self.groupDelay)
        if(npts > 0):
            self.groupcurve.setData(self.freq, self.groupDelay)

            # Reset the x-axis to the new time scale
            if self.iir:
                xmax = self.freq[npts-1]
                ymax = self.groupDelay.max()
                if(ymax < 0):
                    ymax = 0.8 * self.groupDelay.max()
                ymin = 1.1 * self.groupDelay.min()
            else:
                ymax = 1.5 * max(self.groupDelay[0:npts/2])
                ymin = 1.1 * min(self.groupDelay[0:npts/2])
                xmax = self.freq[npts/2]
            xmin = self.freq[0]
            self.gui.groupPlot.setAxisScale(self.gui.groupPlot.xBottom,
                                            xmin, xmax)
            self.gui.groupPlot.setAxisScale(self.gui.groupPlot.yLeft,
                                            ymin, ymax)

            if self.mfoverlay:
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.xBottom,
                                                xmin, xmax)
                self.gui.mfreqPlot.setAxisScale(self.groupcurve.yAxis(),
                                                ymin, ymax)
                self.mfreqZoomer2.setEnabled(True)
                self.mfreqZoomer2.setZoomBase()
            else:
                self.mfreqZoomer2.setEnabled(False)
                self.gui.mfreqPlot.enableAxis(Qwt.QwtPlot.yRight,False)
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.xBottom,
                                                xmin, xmax)
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.yLeft,
                                                ymin, ymax)

            #Set Axis title
            self.gui.mfreqPlot.setAxisTitle(self.groupcurve.yAxis(),
						                        gytitle)
            # Set the zoomer base to unzoom to the new axis
            self.groupZoomer.setZoomBase()
            self.mfreqZoomer.setZoomBase()

            self.gui.groupPlot.replot()
            self.gui.mfreqPlot.replot()

    def update_pdelay_curves(self):
        phytitle=Qwt.QwtText("Phase Delay")
        phytitle.setFont(Qt.QFont("Helvetica", 9, Qt.QFont.Bold))
        npts = len(self.phaseDelay)
        if(npts > 0):
            self.pdelaycurve.setData(self.freq, self.phaseDelay)

            # Reset the x-axis to the new time scale
            if self.iir:
                xmax = self.freq[npts-1]
                ymax = self.phaseDelay.max()
                if(ymax < 0):
                    ymax = 0.8 * self.phaseDelay.max()
                ymin = 1.1 * self.phaseDelay.min()
            else:
                ymax = 1.3 * max(self.phaseDelay[0:npts])
                ymin = 0.8 * min(self.phaseDelay[0:npts])
                xmax = self.freq[npts/2]
            xmin = self.freq[0]
            self.gui.pdelayPlot.setAxisScale(self.gui.pdelayPlot.xBottom,
                                            xmin, xmax)
            self.gui.pdelayPlot.setAxisScale(self.gui.pdelayPlot.yLeft,
                                            ymin, ymax)

            if self.mfoverlay:
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.xBottom,
                                                xmin, xmax)
                self.gui.mfreqPlot.setAxisScale(self.pdelaycurve.yAxis(),
                                                ymin, ymax)
                self.mfreqZoomer2.setEnabled(True)
                self.mfreqZoomer2.setZoomBase()
            else:
                self.mfreqZoomer2.setEnabled(False)
                self.gui.mfreqPlot.enableAxis(Qwt.QwtPlot.yRight,False)
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.xBottom,
                                                xmin, xmax)
                self.gui.mfreqPlot.setAxisScale(self.gui.mfreqPlot.yLeft,
                                                ymin, ymax)
            #Set Axis title
            self.gui.mfreqPlot.setAxisTitle(self.pdelaycurve.yAxis(),
						                        phytitle)

            # Set the zoomer base to unzoom to the new axis
            self.pdelayZoomer.setZoomBase()
            self.mfreqZoomer.setZoomBase()

            self.gui.pdelayPlot.replot()
            self.gui.mfreqPlot.replot()

    def action_quick_access(self):
        #Hides quick access widget if unselected
        if (self.gui.quickFrame.isHidden()):
            self.gui.quickFrame.show()
        else:
            self.gui.quickFrame.hide()

    def action_spec_widget(self):
        #Hides spec widget if unselected
        if (self.gui.filterspecView.isHidden()):
            self.gui.filterspecView.show()
        else:
            self.gui.filterspecView.hide()

    def action_response_widget(self):
        if (self.gui.tabGroup.isHidden()):
            self.gui.tabGroup.show()
        else:
            self.gui.tabGroup.hide()

    def action_design_widget(self):
        #Hides design widget if unselected
        if (self.gui.filterFrame.isHidden()):
            self.gui.filterFrame.show()
        else:
            self.gui.filterFrame.hide()

    #Saves and attach the plots for comparison
    def set_bufferplots(self):
        if (self.gui.checkKeepcur.checkState() == 0 ):
            #Detach and delete all plots if unchecked
            for c in self.bufferplots:
                c.detach()
            self.replot_all()
            self.bufferplots = []
        else:
            self.bufferplots = []
            #Iterate through tabgroup children and copy curves
            for i in range(self.gui.tabGroup.count()):
                page = self.gui.tabGroup.widget(i)
                for item in page.children():
                    if isinstance(item, Qwt.QwtPlot):
                        #Change colours as both plots overlay
                        colours = [QtCore.Qt.darkYellow,QtCore.Qt.black]
                        for c in item.itemList():
                            if isinstance(c, Qwt.QwtPlotCurve):
                                dup = Qwt.QwtPlotCurve()
                                dpen = c.pen()
                                dsym = c.symbol()
                                dsym.setPen(Qt.QPen(colours[0]))
                                dsym.setSize(Qt.QSize(6, 6))
                                dpen.setColor(colours[0])
                                del colours[0]
                                dup.setPen(dpen)
                                dup.setSymbol(dsym)
                                dup.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
                                dup.setData([c.x(i) for i in range(c.dataSize())],
										    [c.y(i) for i in range(c.dataSize())])
                                self.bufferplots.append(dup)
                                self.bufferplots[-1].attach(item)

    def set_grid(self):
        if (self.gui.checkGrid.checkState() == 0 ):
            self.gridenable=False
            self.detach_allgrid()
            self.replot_all()
        else:
            self.gridenable=True
            if self.gridview:
                self.freqgrid.attach(self.gui.mfreqPlot)
                self.ftapsgrid.attach(self.gui.mtimePlot)
                self.gui.mfreqPlot.replot()
                self.gui.mtimePlot.replot()
            else:
                self.freqgrid.attach(self.gui.freqPlot)
                self.phasegrid.attach(self.gui.phasePlot)
                self.groupgrid.attach(self.gui.groupPlot)
                self.impresgrid.attach(self.gui.impresPlot)
                self.stepresgrid.attach(self.gui.stepresPlot)
                self.pdelaygrid.attach(self.gui.pdelayPlot)
                self.ftapsgrid.attach(self.gui.timePlot)
                self.replot_all()

    def set_actgrid(self):
        if (self.gui.actionGrid_2.isChecked() == 0 ):
            self.gridenable=False
            self.detach_allgrid()
            self.replot_all()
        else:
            self.gridenable=True
            if self.gridview:
                self.freqgrid.attach(self.gui.mfreqPlot)
                self.ftapsgrid.attach(self.gui.mtimePlot)
                self.gui.mfreqPlot.replot()
                self.gui.mtimePlot.replot()
            else:
                self.freqgrid.attach(self.gui.freqPlot)
                self.phasegrid.attach(self.gui.phasePlot)
                self.groupgrid.attach(self.gui.groupPlot)
                self.impresgrid.attach(self.gui.impresPlot)
                self.stepresgrid.attach(self.gui.stepresPlot)
                self.pdelaygrid.attach(self.gui.pdelayPlot)
                self.ftapsgrid.attach(self.gui.timePlot)
                self.replot_all()

    def set_magresponse(self):
        if (self.gui.checkMagres.checkState() == 0 ):
            self.magres=False
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.freqTab))
        else:
            self.magres=True
            self.gui.tabGroup.addTab(self.gui.freqTab, _fromUtf8("Magnitude Response"))
            self.update_freq_curves()

    def set_actmagresponse(self):
        if (self.gui.actionMagnitude_Response.isChecked() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.freqTab))
        else:
            self.gui.tabGroup.addTab(self.gui.freqTab, _fromUtf8("Magnitude Response"))
            self.update_freq_curves()

    def set_switchview(self):
        if (self.gui.actionGridview.isChecked() == 0 ):
            self.gridview=0
            self.set_defaultpen()
            self.set_actgrid()
            self.gui.stackedWindows.setCurrentIndex(0)
            self.freqcurve.attach(self.gui.freqPlot)
            self.phasecurve.attach(self.gui.phasePlot)
            self.pdelaycurve.attach(self.gui.pdelayPlot)
            self.groupcurve.attach(self.gui.groupPlot)
            self.rcurve.attach(self.gui.timePlot)
            self.icurve.attach(self.gui.timePlot)
            self.imprescurve.attach(self.gui.impresPlot)
            self.imprescurve_i.attach(self.gui.impresPlot)
            self.steprescurve.attach(self.gui.stepresPlot)
            self.steprescurve_i.attach(self.gui.stepresPlot)
            if self.iir:
                self.iir_plot_all(self.z,self.p,self.k)
            else:
                self.draw_plots(self.taps,self.params)
        else:
            self.gridview=1
            self.set_actgrid()
            self.gui.stackedWindows.setCurrentIndex(1)
            self.freqcurve.attach(self.gui.mfreqPlot)
            self.rcurve.attach(self.gui.mtimePlot)
            self.icurve.attach(self.gui.mtimePlot)
            self.update_freq_curves()
            self.update_time_curves()
        self.set_drawideal()

    def set_plotselect(self):
        if (self.gui.actionPlot_select.isChecked() == 0 ):
            self.gui.mfgroupBox.hide()
            self.gui.mtgroupBox.hide()
            self.gui.pzgroupBox.hide()
            self.gui.mpzgroupBox.hide()
        else:
            self.gui.mfgroupBox.show()
            self.gui.mtgroupBox.show()
            self.gui.pzgroupBox.show()
            self.gui.mpzgroupBox.show()

    def replot_all(self):
        self.gui.timePlot.replot()
        self.gui.mtimePlot.replot()
        self.gui.freqPlot.replot()
        self.gui.mfreqPlot.replot()
        self.gui.phasePlot.replot()
        self.gui.groupPlot.replot()
        self.gui.impresPlot.replot()
        self.gui.stepresPlot.replot()
        self.gui.pdelayPlot.replot()

    def detach_allgrid(self):
        self.freqgrid.detach()
        self.phasegrid.detach()
        self.groupgrid.detach()
        self.pdelaygrid.detach()
        self.impresgrid.detach()
        self.stepresgrid.detach()
        self.ftapsgrid.detach()

    def set_mfmagresponse(self):
        if self.mfoverlay:
            if not(self.ifinlist(self.freqcurve,self.gui.mfreqPlot.itemList())):
                self.detach_allgrid()
                self.freqcurve.attach(self.gui.mfreqPlot)
                self.detach_firstattached(self.gui.mfreqPlot)
            self.update_freq_curves()
            self.idbanditems.detach_allidealcurves(self.gui.mfreqPlot)
        else:
            self.gui.mfreqPlot.detachItems(Qwt.QwtPlotItem.Rtti_PlotItem, False)
            self.set_actgrid()
            self.freqcurve.setPen(QtGui.QPen(QtCore.Qt.blue, 1, QtCore.Qt.SolidLine))
            self.freqcurve.setYAxis(Qwt.QwtPlot.yLeft)
            self.freqcurve.attach(self.gui.mfreqPlot)
            self.update_freq_curves()
            self.set_drawideal()

    def set_mfphaseresponse(self):
        if self.mfoverlay:
            if not(self.ifinlist(self.phasecurve,self.gui.mfreqPlot.itemList())):
                self.detach_allgrid()
                self.phasecurve.attach(self.gui.mfreqPlot)
                self.detach_firstattached(self.gui.mfreqPlot)
                self.update_phase_curves()
            self.idbanditems.detach_allidealcurves(self.gui.mfreqPlot)
        else:
            self.gui.mfreqPlot.detachItems(Qwt.QwtPlotItem.Rtti_PlotItem, False)
            self.set_actgrid()
            self.phasecurve.setPen(QtGui.QPen(QtCore.Qt.blue, 1, QtCore.Qt.SolidLine))
            self.phasecurve.setYAxis(Qwt.QwtPlot.yLeft)
            self.phasecurve.attach(self.gui.mfreqPlot)
            self.update_phase_curves()

    def set_mfgroupdelay(self):
        if self.mfoverlay:
            if not(self.ifinlist(self.groupcurve,self.gui.mfreqPlot.itemList())):
                self.detach_allgrid()
                self.groupcurve.attach(self.gui.mfreqPlot)
                self.detach_firstattached(self.gui.mfreqPlot)
                self.update_group_curves()
            self.idbanditems.detach_allidealcurves(self.gui.mfreqPlot)
        else:
            self.gui.mfreqPlot.detachItems(Qwt.QwtPlotItem.Rtti_PlotItem, False)
            self.set_actgrid()
            self.groupcurve.setPen(QtGui.QPen(QtCore.Qt.blue, 1, QtCore.Qt.SolidLine))
            self.groupcurve.setYAxis(Qwt.QwtPlot.yLeft)
            self.groupcurve.attach(self.gui.mfreqPlot)
            self.update_group_curves()

    def set_mfphasedelay(self):
        if self.mfoverlay:
            if not(self.ifinlist(self.pdelaycurve,self.gui.mfreqPlot.itemList())):
                self.detach_allgrid()
                self.pdelaycurve.attach(self.gui.mfreqPlot)
                self.detach_firstattached(self.gui.mfreqPlot)
                self.update_pdelay_curves()
            self.idbanditems.detach_allidealcurves(self.gui.mfreqPlot)
        else:
            self.gui.mfreqPlot.detachItems(Qwt.QwtPlotItem.Rtti_PlotItem, False)
            self.set_actgrid()
            self.pdelaycurve.setPen(QtGui.QPen(QtCore.Qt.blue, 1, QtCore.Qt.SolidLine))
            self.pdelaycurve.setYAxis(Qwt.QwtPlot.yLeft)
            self.pdelaycurve.attach(self.gui.mfreqPlot)
            self.update_pdelay_curves()

    def ifinlist(self,a,dlist):
        for d in dlist:
            if self.compare_instances(a,d):
                return True
        return False

    def compare_instances(self,a,b):
        if a is b:
            return True
        else:
            return False

    def detach_firstattached(self, plot):
        items=plot.itemList()
        plot.enableAxis(Qwt.QwtPlot.yRight)
        if len(items) > 2:
            yaxis=items[0].yAxis()
            items[2].setPen(items[0].pen())
            items[2].setYAxis(yaxis)
            items[0].detach()
        else:
            items[1].setYAxis(Qwt.QwtPlot.yRight)
            if plot is self.gui.mfreqPlot:
                items[1].setPen(QtGui.QPen(QtCore.Qt.red, 1, QtCore.Qt.SolidLine))
        self.set_actgrid()


    def update_fft(self, taps, params):
        self.params = params
        self.taps = scipy.array(taps)
        self.get_fft(self.params["fs"], self.taps, self.nfftpts)

    def set_mfoverlay(self):
        self.mfoverlay = not(self.mfoverlay)

    def set_conj(self):
        self.cpicker.set_conjugate()

    def set_mconj(self):
        self.cpicker2.set_conjugate()

    def set_zeroadd(self):
        self.cpicker.add_zero()

    def set_mzeroadd(self):
        self.cpicker2.add_zero()

    def set_poleadd(self):
        self.cpicker.add_pole()

    def set_mpoleadd(self):
        self.cpicker2.add_pole()

    def set_delpz(self):
        self.cpicker.delete_pz()

    def set_mdelpz(self):
        self.cpicker2.delete_pz()

    def set_mttaps(self):
        if self.mtoverlay:
            if not(self.ifinlist(self.rcurve,self.gui.mtimePlot.itemList())):
                self.rcurve.attach(self.gui.mtimePlot)
                self.icurve.attach(self.gui.mtimePlot)
                self.detach_firstattached(self.gui.mtimePlot)
                self.update_time_curves()
        else:
            self.gui.mtimePlot.detachItems(Qwt.QwtPlotItem.Rtti_PlotItem, False)
            self.set_actgrid()
            self.rcurve.setYAxis(Qwt.QwtPlot.yLeft)
            self.icurve.setYAxis(Qwt.QwtPlot.yLeft)
            self.rcurve.attach(self.gui.mtimePlot)
            self.icurve.attach(self.gui.mtimePlot)
            self.update_time_curves()

    def set_mtstep(self):
        if self.mtoverlay:
            if not(self.ifinlist(self.steprescurve,self.gui.mtimePlot.itemList())):
                self.steprescurve.attach(self.gui.mtimePlot)
                self.steprescurve_i.attach(self.gui.mtimePlot)
                self.detach_firstattached(self.gui.mtimePlot)
                self.update_step_curves()
        else:
            self.gui.mtimePlot.detachItems(Qwt.QwtPlotItem.Rtti_PlotItem, False)
            self.set_actgrid()
            self.steprescurve.setYAxis(Qwt.QwtPlot.yLeft)
            self.steprescurve.attach(self.gui.mtimePlot)
            self.steprescurve_i.attach(self.gui.mtimePlot)
            self.update_step_curves()

    def set_mtimpulse(self):
        if self.mtoverlay:
            if not(self.ifinlist(self.imprescurve,self.gui.mtimePlot.itemList())):
                self.imprescurve.attach(self.gui.mtimePlot)
                self.imprescurve_i.attach(self.gui.mtimePlot)
                self.detach_firstattached(self.gui.mtimePlot)
                self.update_imp_curves()
        else:
            self.gui.mtimePlot.detachItems(Qwt.QwtPlotItem.Rtti_PlotItem, False)
            self.set_actgrid()
            self.imprescurve.setYAxis(Qwt.QwtPlot.yLeft)
            self.imprescurve.attach(self.gui.mtimePlot)
            self.imprescurve_i.attach(self.gui.mtimePlot)
            self.update_imp_curves()

    def set_gdelay(self):
        if (self.gui.checkGdelay.checkState() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.groupTab))
        else:
            self.gui.tabGroup.addTab(self.gui.groupTab, _fromUtf8("Group Delay"))
            self.update_freq_curves()

    def set_actgdelay(self):
        if (self.gui.actionGroup_Delay.isChecked() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.groupTab))
        else:
            self.gui.tabGroup.addTab(self.gui.groupTab, _fromUtf8("Group Delay"))
            self.update_freq_curves()

    def set_phase(self):
        if (self.gui.checkPhase.checkState() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.phaseTab))
        else:
            self.gui.tabGroup.addTab(self.gui.phaseTab, _fromUtf8("Phase Response"))
            self.update_freq_curves()

    def set_actphase(self):
        if (self.gui.actionPhase_Respone.isChecked() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.phaseTab))
        else:
            self.gui.tabGroup.addTab(self.gui.phaseTab, _fromUtf8("Phase Response"))
            self.update_freq_curves()

    def set_fcoeff(self):
        if (self.gui.checkFcoeff.checkState() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.fcTab))
        else:
            self.gui.tabGroup.addTab(self.gui.fcTab, _fromUtf8("Filter Coefficients"))
            self.update_fcoeff()

    def set_actfcoeff(self):
        if (self.gui.actionFilter_Coefficients.isChecked() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.fcTab))
        else:
            self.gui.tabGroup.addTab(self.gui.fcTab, _fromUtf8("Filter Coefficients"))
            self.update_fcoeff()

    def set_band(self):
        if (self.gui.checkBand.checkState() == 0 ):
            self.gui.filterspecView.removeTab(self.gui.filterspecView.indexOf(self.gui.bandDiagram))
        else:
            self.gui.filterspecView.addTab(self.gui.bandDiagram, _fromUtf8("Band Diagram"))

    def set_actband(self):
        if (self.gui.actionBand_Diagram.isChecked() == 0 ):
            self.gui.filterspecView.removeTab(self.gui.filterspecView.indexOf(self.gui.bandDiagram))
        else:
            self.gui.filterspecView.addTab(self.gui.bandDiagram, _fromUtf8("Band Diagram"))

    def set_drawideal(self):
        fsel = str(self.gui.fselectComboBox.currentText().toAscii())
        if self.gridview and not(self.mfoverlay):
            plot = self.gui.mfreqPlot
        else:
            plot = self.gui.freqPlot

        if (self.gui.actionIdeal_Band.isChecked() == 0 or fsel == "IIR(scipy)"):
            self.idbanditems.detach_allidealcurves(plot)
        elif(self.params):
            ftype = str(self.gui.filterTypeComboBox.currentText().toAscii())
            self.idbanditems.attach_allidealcurves(plot)
            self.idbanditems.plotIdealCurves(ftype, self.params, plot)
            plot.replot()

    def set_pzplot(self):
        if (self.gui.checkPzplot.checkState() == 0 ):
            self.gui.filterspecView.removeTab(self.gui.filterspecView.indexOf(self.gui.poleZero))
        else:
            self.gui.filterspecView.addTab(self.gui.poleZero, _fromUtf8("Pole-Zero Plot"))

    def set_actpzplot(self):
        if (self.gui.actionPole_Zero_Plot_2.isChecked() == 0 ):
            self.gui.filterspecView.removeTab(self.gui.filterspecView.indexOf(self.gui.poleZero))
        else:
            self.gui.filterspecView.addTab(self.gui.poleZero, _fromUtf8("Pole-Zero Plot"))

    def set_pdelay(self):
        if (self.gui.checkPzplot.checkState() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.pdelayTab))
        else:
            self.gui.tabGroup.addTab(self.gui.pdelayTab, _fromUtf8("Phase Delay"))

    def set_actpdelay(self):
        if (self.gui.actionPhase_Delay.isChecked() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.pdelayTab))
        else:
            self.gui.tabGroup.addTab(self.gui.pdelayTab, _fromUtf8("Phase Delay"))

    def set_impres(self):
        if (self.gui.checkImpulse.checkState() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.impresTab))
        else:
            self.gui.tabGroup.addTab(self.gui.impresTab, _fromUtf8("Impulse Response"))

    def set_actimpres(self):
        if (self.gui.actionImpulse_Response.isChecked() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.impresTab))
        else:
            self.gui.tabGroup.addTab(self.gui.impresTab, _fromUtf8("Impulse Response"))

    def set_stepres(self):
        if (self.gui.checkStep.checkState() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.stepresTab))
        else:
            self.gui.tabGroup.addTab(self.gui.stepresTab, _fromUtf8("Step Response"))

    def set_actstepres(self):
        if (self.gui.actionStep_Response.isChecked() == 0 ):
            self.gui.tabGroup.removeTab(self.gui.tabGroup.indexOf(self.gui.stepresTab))
        else:
            self.gui.tabGroup.addTab(self.gui.stepresTab, _fromUtf8("Step Response"))

    def populate_bandview(self,fitems):
        for item in fitems:
            if (item.isWidgetType()):
                self.scene.addWidget(item)
            else:
                self.scene.addItem(item)

    def remove_bandview(self):
        for item in self.scene.items():
            self.scene.removeItem(item)

    def set_fatten(self,atten):
        ftype = str(self.gui.filterTypeComboBox.currentText().toAscii())
        if (ftype == "Low Pass"):
            boxatten,r = self.gui.lpfStopBandAttenEdit.text().toDouble()
            self.gui.lpfStopBandAttenEdit.setText(Qt.QString(str(atten+boxatten)))

        if ftype == "High Pass":
            boxatten,r = self.gui.hpfStopBandAttenEdit.text().toDouble()
            self.gui.hpfStopBandAttenEdit.setText(Qt.QString(str(atten+boxatten)))

        if ftype == "Band Pass":
            boxatten,r = self.gui.bpfStopBandAttenEdit.text().toDouble()
            self.gui.bpfStopBandAttenEdit.setText(Qt.QString(str(atten+boxatten)))

        if ftype == "Band Notch":
            boxatten,r = self.gui.bnfStopBandAttenEdit.text().toDouble()
            self.gui.bnfStopBandAttenEdit.setText(Qt.QString(str(atten+boxatten)))

        if ftype == "Complex Band Pass":
            boxatten,r = self.gui.bpfStopBandAttenEdit.text().toDouble()
            self.gui.bpfStopBandAttenEdit.setText(Qt.QString(str(atten+boxatten)))
#self.design()

    def set_curvetaps(self,(zr,pl)):
        if self.iir:
            self.z=zr
            self.p=pl
            self.iir_plot_all(self.z,self.p,self.k)
            self.gui.mpzPlot.insertZeros(zr)
            self.gui.mpzPlot.insertPoles(pl)
            self.update_fcoeff()
            if self.callback:
                retobj = ApiObject()
                retobj.update_all("iir", self.params, (self.b, self.a), 1)
                self.callback(retobj)
        else:
            hz = poly1d(zr,r=1)
            #print hz.c
            self.taps=hz.c*self.taps[0]
            self.draw_plots(self.taps,self.params)
            self.update_fcoeff()
            #update the pzplot in other view
            zeros=self.get_zeros()
            poles=self.get_poles()
            self.gui.mpzPlot.insertZeros(zeros)
            self.gui.mpzPlot.insertPoles(poles)
            self.gui.nTapsEdit.setText(Qt.QString("%1").arg(self.taps.size))
            if self.callback:
                retobj = ApiObject()
                retobj.update_all("fir", self.params, self.taps, 1)
                self.callback(retobj)

    def set_mcurvetaps(self,(zr,pl)):
        if self.iir:
            self.z=zr
            self.p=pl
            self.iir_plot_all(self.z,self.p,self.k)
            self.gui.pzPlot.insertZeros(zr)
            self.gui.pzPlot.insertPoles(pl)
            self.update_fcoeff()
            if self.callback:
                retobj = ApiObject()
                retobj.update_all("iir", self.params, (self.b, self.a), 1)
                self.callback(retobj)
        else:
            hz = poly1d(zr,r=1)
            #print hz.c
            self.taps=hz.c*self.taps[0]
            if self.gridview:
                self.update_fft(self.taps, self.params)
                self.set_mfmagresponse()
                self.set_mttaps()
            else:
                self.draw_plots(self.taps,self.params)
            self.update_fcoeff()
            #update the pzplot in other view
            zeros=self.get_zeros()
            poles=self.get_poles()
            self.gui.pzPlot.insertZeros(zeros)
            self.gui.pzPlot.insertPoles(poles)
            self.gui.nTapsEdit.setText(Qt.QString("%1").arg(self.taps.size))
            if self.callback:
                retobj = ApiObject()
                retobj.update_all("fir", self.params, self.taps, 1)
                self.callback(retobj)

    def set_statusbar(self,(x,y)):
        if x == None:
        	self.gui.pzstatusBar.showMessage("")
        else:
        	self.gui.pzstatusBar.showMessage("X: "+str(x)+"  Y: "+str(y))

    def set_mstatusbar(self,(x,y)):
        if x == None:
        	self.gui.mpzstatusBar.showMessage("")
        else:
        	self.gui.mpzstatusBar.showMessage("X: "+str(x)+"  Y: "+str(y))

    def get_zeros(self):
        hz = poly1d(self.taps,r=0)
        return hz.r

    def get_poles(self):
        if len(self.taps):
            hp = zeros(len(self.taps)-1,complex)
            return hp
        else:
            return []

    def impulse_response(self,b,a=1):
        l = len(b)
        if self.iir:
            l = 50
        impulse = scipy.repeat(0.,l)
        impulse[0] =1.
        x = scipy.arange(0,l)
        response = signal.lfilter(b,a,impulse)
        return response

    def step_response(self,b,a=1):
        l = len(b)
        if self.iir:
            l = 50
        impulse = scipy.repeat(0.,l)
        impulse[0] =1.
        x = scipy.arange(0,l)
        response = signal.lfilter(b,a,impulse)
        step = scipy.cumsum(response)
        return step

    def update_fcoeff(self):
        fcoeff=""
        if self.iir:
            fcoeff="b = "+str(self.b.tolist())+"\na = "+str(self.a.tolist())
        else:
            fcoeff="taps = "+str(self.taps.tolist())
        self.gui.filterCoeff.setText(Qt.QString(fcoeff))
        self.gui.mfilterCoeff.setText(Qt.QString(fcoeff))

    def action_save_dialog(self):
        filename = QtGui.QFileDialog.getSaveFileName(self, "Save CSV Filter File", ".", "")
        try:
            handle = open(filename, "wb")
        except IOError:
            reply = QtGui.QMessageBox.information(self, 'File Name',
                                                  ("Could not save to file: %s" % filename),
                                                  "&Ok")
            return

        csvhandle = csv.writer(handle, delimiter=",")
        #indicate fir/iir for easy reading
        if self.iir:
            csvhandle.writerow(["restype","iir"])
        else:
            csvhandle.writerow(["restype","fir"])

        for k in self.params.keys():
            csvhandle.writerow([k, self.params[k]])
        if self.iir:
            csvhandle.writerow(["b",] + self.b.tolist())
            csvhandle.writerow(["a",] + self.a.tolist())
        else:
            csvhandle.writerow(["taps",] + self.taps.tolist())
        handle.close()

    def action_open_dialog(self):
        filename = QtGui.QFileDialog.getOpenFileName(self, "Open CSV Filter File", ".", "")
        if(len(filename) == 0):
            return

        try:
            handle = open(filename, "rb")
        except IOError:
            reply = QtGui.QMessageBox.information(self, 'File Name',
                                                  ("Could not open file: %s" % filename),
                                                  "&Ok")
            return

        csvhandle = csv.reader(handle, delimiter=",")
        b_a={}
        taps = []
        params = {}
        for row in csvhandle:
            if (row[0] == "restype"):
                restype = row[1]
            elif(row[0] == "taps"):
                testcpx = re.findall("[+-]?\d+\.*\d*[Ee]?[-+]?\d+j", row[1])
                if(len(testcpx) > 0): # it's a complex
                    taps = [complex(r) for r in row[1:]]
                else:
                    taps = [float(r) for r in row[1:]]
            elif(row[0] == "b" or row[0] == "a"):
                testcpx = re.findall("[+-]?\d+\.*\d*[Ee]?[-+]?\d+j", row[1])
                if(len(testcpx) > 0): # it's a complex
                    b_a[row[0]] = [complex(r) for r in row[1:]]
                else:
                    b_a[row[0]]= [float(r) for r in row[1:]]
            else:
                testcpx = re.findall("[+-]?\d+\.*\d*[Ee]?[-+]?\d+j", row[1])
                if(len(testcpx) > 0): # it's a complex
                    params[row[0]] = complex(row[1])
                else: # assume it's a float
                    try: # if it's not a float, its a string
                        params[row[0]] = float(row[1])
                    except ValueError:
                        params[row[0]] = row[1]
        handle.close()
        if restype == "fir":
            self.iir = False
            self.gui.fselectComboBox.setCurrentIndex(0)
            self.draw_plots(taps, params)
            zeros=self.get_zeros()
            poles=self.get_poles()
            self.gui.pzPlot.insertZeros(zeros)
            self.gui.pzPlot.insertPoles(poles)
            self.gui.mpzPlot.insertZeros(zeros)
            self.gui.mpzPlot.insertPoles(poles)

            self.gui.sampleRateEdit.setText(Qt.QString("%1").arg(params["fs"]))
            self.gui.filterGainEdit.setText(Qt.QString("%1").arg(params["gain"]))

            # Set up GUI parameters for each filter type
            if(params["filttype"] == "lpf"):
                self.gui.filterTypeComboBox.setCurrentIndex(0)
                self.gui.filterDesignTypeComboBox.setCurrentIndex(int(params["wintype"]))

                self.gui.endofLpfPassBandEdit.setText(Qt.QString("%1").arg(params["pbend"]))
                self.gui.startofLpfStopBandEdit.setText(Qt.QString("%1").arg(params["sbstart"]))
                self.gui.lpfStopBandAttenEdit.setText(Qt.QString("%1").arg(params["atten"]))
                if(params["wintype"] == self.EQUIRIPPLE_FILT):
                    self.gui.lpfPassBandRippleEdit.setText(Qt.QString("%1").arg(params["ripple"]))
            elif(params["filttype"] == "bpf"):
                self.gui.filterTypeComboBox.setCurrentIndex(1)
                self.gui.filterDesignTypeComboBox.setCurrentIndex(int(params["wintype"]))

                self.gui.startofBpfPassBandEdit.setText(Qt.QString("%1").arg(params["pbstart"]))
                self.gui.endofBpfPassBandEdit.setText(Qt.QString("%1").arg(params["pbend"]))
                self.gui.bpfTransitionEdit.setText(Qt.QString("%1").arg(params["tb"]))
                self.gui.bpfStopBandAttenEdit.setText(Qt.QString("%1").arg(params["atten"]))
                if(params["wintype"] == self.EQUIRIPPLE_FILT):
                    self.gui.bpfPassBandRippleEdit.setText(Qt.QString("%1").arg(params["ripple"]))
            elif(params["filttype"] == "cbpf"):
                self.gui.filterTypeComboBox.setCurrentIndex(2)
                self.gui.filterDesignTypeComboBox.setCurrentIndex(int(params["wintype"]))

                self.gui.startofBpfPassBandEdit.setText(Qt.QString("%1").arg(params["pbstart"]))
                self.gui.endofBpfPassBandEdit.setText(Qt.QString("%1").arg(params["pbend"]))
                self.gui.bpfTransitionEdit.setText(Qt.QString("%1").arg(params["tb"]))
                self.gui.bpfStopBandAttenEdit.setText(Qt.QString("%1").arg(params["atten"]))
                if(params["wintype"] == self.EQUIRIPPLE_FILT):
                    self.gui.bpfPassBandRippleEdit.setText(Qt.QString("%1").arg(params["ripple"]))
            elif(params["filttype"] == "bnf"):
                self.gui.filterTypeComboBox.setCurrentIndex(3)
                self.gui.filterDesignTypeComboBox.setCurrentIndex(int(params["wintype"]))

                self.gui.startofBnfStopBandEdit.setText(Qt.QString("%1").arg(params["sbstart"]))
                self.gui.endofBnfStopBandEdit.setText(Qt.QString("%1").arg(params["sbend"]))
                self.gui.bnfTransitionEdit.setText(Qt.QString("%1").arg(params["tb"]))
                self.gui.bnfStopBandAttenEdit.setText(Qt.QString("%1").arg(params["atten"]))
                if(params["wintype"] == self.EQUIRIPPLE_FILT):
                    self.gui.bnfPassBandRippleEdit.setText(Qt.QString("%1").arg(params["ripple"]))
            elif(params["filttype"] == "hpf"):
                self.gui.filterTypeComboBox.setCurrentIndex(4)
                self.gui.filterDesignTypeComboBox.setCurrentIndex(int(params["wintype"]))

                self.gui.endofHpfStopBandEdit.setText(Qt.QString("%1").arg(params["sbend"]))
                self.gui.startofHpfPassBandEdit.setText(Qt.QString("%1").arg(params["pbstart"]))
                self.gui.hpfStopBandAttenEdit.setText(Qt.QString("%1").arg(params["atten"]))
                if(params["wintype"] == self.EQUIRIPPLE_FILT):
                    self.gui.hpfPassBandRippleEdit.setText(Qt.QString("%1").arg(params["ripple"]))
            elif(params["filttype"] == "rrc"):
                self.gui.filterTypeComboBox.setCurrentIndex(5)
                self.gui.filterDesignTypeComboBox.setCurrentIndex(int(params["wintype"]))

                self.gui.rrcSymbolRateEdit.setText(Qt.QString("%1").arg(params["srate"]))
                self.gui.rrcAlphaEdit.setText(Qt.QString("%1").arg(params["alpha"]))
                self.gui.rrcNumTapsEdit.setText(Qt.QString("%1").arg(params["ntaps"]))
            elif(params["filttype"] == "gaus"):
                self.gui.filterTypeComboBox.setCurrentIndex(6)
                self.gui.filterDesignTypeComboBox.setCurrentIndex(int(params["wintype"]))

                self.gui.gausSymbolRateEdit.setText(Qt.QString("%1").arg(params["srate"]))
                self.gui.gausBTEdit.setText(Qt.QString("%1").arg(params["bt"]))
                self.gui.gausNumTapsEdit.setText(Qt.QString("%1").arg(params["ntaps"]))
        else:
            self.iir = True
            self.b, self.a = b_a["b"],b_a["a"]
            (self.z,self.p,self.k) = signal.tf2zpk(self.b, self.a)
            self.gui.pzPlot.insertZeros(self.z)
            self.gui.pzPlot.insertPoles(self.p)
            self.gui.mpzPlot.insertZeros(self.z)
            self.gui.mpzPlot.insertPoles(self.p)
            self.iir_plot_all(self.z,self.p,self.k)
            self.update_fcoeff()
            self.gui.nTapsEdit.setText("-")
            self.params = params

            #Set GUI for IIR type
            iirft = 	{  "ellip"  : 0,
                           "butter" : 1,
                           "cheby1" : 2,
                           "cheby2" : 3,
                           "bessel" : 4 }

            paramtype = { "analog"  : 1,
                          "digital" : 0 }
            bandpos = {
                         "lpf" : 0,
                         "bpf" : 1,
                         "bnf" : 2,
                         "hpf" : 3}

            iirboxes = {"lpf" : [self.gui.iirendofLpfPassBandEdit,
                                      self.gui.iirstartofLpfStopBandEdit,
                                      self.gui.iirLpfPassBandAttenEdit,
                                      self.gui.iirLpfStopBandRippleEdit],

                        "hpf" : [self.gui.iirstartofHpfPassBandEdit,
                                       self.gui.iirendofHpfStopBandEdit,
                                       self.gui.iirHpfPassBandAttenEdit,
                                       self.gui.iirHpfStopBandRippleEdit],

                        "bpf" : [self.gui.iirstartofBpfPassBandEdit,
                                       self.gui.iirendofBpfPassBandEdit,
                                       self.gui.iirendofBpfStopBandEdit1,
                                       self.gui.iirstartofBpfStopBandEdit2,
                                       self.gui.iirBpfPassBandAttenEdit,
                                       self.gui.iirBpfStopBandRippleEdit],

                        "bnf" : [self.gui.iirendofBsfPassBandEdit1,
                                       self.gui.iirstartofBsfPassBandEdit2,
                                       self.gui.iirstartofBsfStopBandEdit,
                                       self.gui.iirendofBsfStopBandEdit,
                                       self.gui.iirBsfPassBandAttenEdit,
                                       self.gui.iirBsfStopBandRippleEdit] }

            self.gui.fselectComboBox.setCurrentIndex(1)
            self.gui.iirfilterTypeComboBox.setCurrentIndex(iirft[params["filttype"]])
            self.gui.iirfilterBandComboBox.setCurrentIndex(bandpos[params["bandtype"]])
            if params["filttype"] == "bessel":
                critfreq = map(float, params["critfreq"][1:-1].split(','))
                self.gui.besselordEdit.setText(Qt.QString("%1").arg(params["filtord"]))
                self.gui.iirbesselcritEdit1.setText(Qt.QString("%1").arg(critfreq[0]))
                self.gui.iirbesselcritEdit2.setText(Qt.QString("%1").arg(critfreq[1]))
            else:
                self.gui.adComboBox.setCurrentIndex(paramtype[params["paramtype"]])
                if len(iirboxes[params["bandtype"]]) == 4:
                    sdata = [params["pbedge"], params["sbedge"], params["gpass"], params["gstop"]]
                else:
                    pbedge = map(float, params["pbedge"][1:-1].split(','))
                    sbedge = map(float, params["sbedge"][1:-1].split(','))
                    sdata = [pbedge[0], pbedge[1], sbedge[0],
                             sbedge[1], params["gpass"], params["gstop"]]

                cboxes = iirboxes[params["bandtype"]]
                for i in range(len(cboxes)):
                    cboxes[i].setText(Qt.QString("%1").arg(sdata[i]))

    def draw_plots(self, taps, params):
        self.params = params
        self.taps = scipy.array(taps)
        if self.params:
            self.get_fft(self.params["fs"], self.taps, self.nfftpts)
        self.update_time_curves()
        self.update_freq_curves()
        self.update_phase_curves()
        self.update_group_curves()
        self.update_pdelay_curves()
        self.update_step_curves()
        self.update_imp_curves()

        self.gui.nTapsEdit.setText(Qt.QString("%1").arg(self.taps.size))


def setup_options():
    usage="%prog: [options] (input_filename)"
    description = ""

    parser = OptionParser(conflict_handler="resolve",
                          usage=usage, description=description)
    return parser

def launch(args, callback=None, restype=""):
    parser = setup_options()
    (options, args) = parser.parse_args ()

    if callback == None:
        app = Qt.QApplication(args)
        gplt = gr_plot_filter(options, callback, restype)
        app.exec_()
        if gplt.iir:
            retobj = ApiObject()
            retobj.update_all("iir", gplt.params, (gplt.b, gplt.a), 1)
            return retobj
        else:
            retobj = ApiObject()
            retobj.update_all("fir", gplt.params, gplt.taps, 1)
            return retobj
    else:
        gplt = gr_plot_filter(options, callback, restype)
        return gplt

def main(args):
    parser = setup_options()
    (options, args) = parser.parse_args ()

    app = Qt.QApplication(args)
    gplt = gr_plot_filter(options)
    app.exec_()

if __name__ == '__main__':
    main(sys.argv)

