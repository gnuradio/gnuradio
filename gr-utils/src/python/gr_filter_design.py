#!/usr/bin/env python

import sys, os, re, csv
from optparse import OptionParser
from gnuradio import gr, blks2, eng_notation

try:
    import scipy
    from scipy import fftpack
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
    from gnuradio.pyqt_filter import Ui_MainWindow
except ImportError:
    print "Could not import from pyqt_filter. Please build with \"pyuic4 pyqt_filter.ui -o pyqt_filter.py\""
    raise SystemExit, 1


class gr_plot_filter(QtGui.QMainWindow):
    def __init__(self, qapp, options):
        QtGui.QWidget.__init__(self, None)
        self.gui = Ui_MainWindow()
        self.gui.setupUi(self)

        self.connect(self.gui.action_save,
                     Qt.SIGNAL("activated()"),
                     self.action_save_dialog)
        self.connect(self.gui.action_open,
                     Qt.SIGNAL("activated()"),
                     self.action_open_dialog)


        self.connect(self.gui.filterTypeComboBox,
                     Qt.SIGNAL("currentIndexChanged(const QString&)"),
                     self.changed_filter_type)
        self.connect(self.gui.filterDesignTypeComboBox,
                     Qt.SIGNAL("currentIndexChanged(const QString&)"),
                     self.changed_filter_design_type)

        self.connect(self.gui.designButton,
                     Qt.SIGNAL("released()"),
                     self.design)

        self.connect(self.gui.tabGroup,
                     Qt.SIGNAL("currentChanged(int)"),
                     self.tab_changed)

        self.connect(self.gui.nfftEdit,
                     Qt.SIGNAL("textEdited(QString)"),
                     self.nfft_edit_changed)

        self.gui.designButton.setShortcut(QtCore.Qt.Key_Return)

        self.taps = []
        self.fftdB = []
        self.fftDeg = []
        self.groupDelay = []
        self.nfftpts = int(10000)
        self.gui.nfftEdit.setText(Qt.QString("%1").arg(self.nfftpts))

        self.firFilters = ("Low Pass", "Band Pass", "Complex Band Pass", "Band Notch",
                           "High Pass", "Root Raised Cosine", "Gaussian")
        self.optFilters = ("Low Pass", "Band Pass", "Complex Band Pass",
                           "Band Notch", "High Pass")
        
        self.set_windowed()
                
        # Initialize to LPF
        self.gui.filterTypeWidget.setCurrentWidget(self.gui.firlpfPage)

        # Set Axis labels
        self.gui.freqPlot.setAxisTitle(self.gui.freqPlot.xBottom,
                                       "Frequency (Hz)")
        self.gui.freqPlot.setAxisTitle(self.gui.freqPlot.yLeft,
                                       "Magnitude (dB)")
        self.gui.timePlot.setAxisTitle(self.gui.timePlot.xBottom,
                                       "Tap number")
        self.gui.timePlot.setAxisTitle(self.gui.timePlot.yLeft,
                                       "Amplitude")
        self.gui.phasePlot.setAxisTitle(self.gui.phasePlot.xBottom,
                                        "Frequency (Hz)")
        self.gui.phasePlot.setAxisTitle(self.gui.phasePlot.yLeft,
                                        "Phase (Radians)")
        self.gui.groupPlot.setAxisTitle(self.gui.groupPlot.xBottom,
                                        "Frequency (Hz)")
        self.gui.groupPlot.setAxisTitle(self.gui.groupPlot.yLeft,
                                        "Delay (sec)")

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

        # Set up pen for colors and line width
        blue = QtGui.qRgb(0x00, 0x00, 0xFF)
        blueBrush = Qt.QBrush(Qt.QColor(blue))
        red = QtGui.qRgb(0xFF, 0x00, 0x00)
        redBrush = Qt.QBrush(Qt.QColor(red))
        self.freqcurve.setPen(Qt.QPen(blueBrush, 2))
        self.rcurve.setPen(Qt.QPen(blueBrush, 2))
        self.icurve.setPen(Qt.QPen(redBrush, 2))
        self.phasecurve.setPen(Qt.QPen(blueBrush, 2))
        self.groupcurve.setPen(Qt.QPen(blueBrush, 2))

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

        self.filterWindows = {"Hamming Window" : gr.firdes.WIN_HAMMING,
                              "Hann Window" : gr.firdes.WIN_HANN,
                              "Blackman Window" : gr.firdes.WIN_BLACKMAN,
                              "Rectangular Window" : gr.firdes.WIN_RECTANGULAR,
                              "Kaiser Window" : gr.firdes.WIN_KAISER,
                              "Blackman-harris Window" : gr.firdes.WIN_BLACKMAN_hARRIS}
        self.EQUIRIPPLE_FILT = 6 # const for equiripple filter window types
        self.show()

    def changed_filter_type(self, ftype):
        strftype = str(ftype.toAscii())
        if(ftype == "Low Pass"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firlpfPage)
        elif(ftype == "Band Pass"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firbpfPage)
        elif(ftype == "Complex Band Pass"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firbpfPage)
        elif(ftype == "Band Notch"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firbnfPage)
        elif(ftype == "High Pass"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.firhpfPage)
        elif(ftype == "Root Raised Cosine"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.rrcPage)
        elif(ftype == "Gaussian"):
            self.gui.filterTypeWidget.setCurrentWidget(self.gui.gausPage)

        self.design()
        
    def changed_filter_design_type(self, design):
        if(design == "Equiripple"):
            self.set_equiripple()
        else:
            self.set_windowed()
            
        self.design()

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

        if(ret):
            winstr = str(self.gui.filterDesignTypeComboBox.currentText().toAscii())
            ftype = str(self.gui.filterTypeComboBox.currentText().toAscii())

            if(winstr == "Equiripple"):
                designer = {"Low Pass" : self.design_opt_lpf,
                            "Band Pass" : self.design_opt_bpf,
                            "Complex Band Pass" : self.design_opt_cbpf,
                            "Band Notch" : self.design_opt_bnf,
                            "High Pass" :  self.design_opt_hpf}
                taps,params,r = designer[ftype](fs, gain)

            else:
                designer = {"Low Pass" : self.design_win_lpf,
                            "Band Pass" : self.design_win_bpf,
                            "Complex Band Pass" : self.design_win_cbpf,
                            "Band Notch" : self.design_win_bnf,
                            "High Pass" :  self.design_win_hpf,
                            "Root Raised Cosine" :  self.design_win_rrc,
                            "Gaussian" :  self.design_win_gaus}
                wintype = self.filterWindows[winstr]
                taps,params,r = designer[ftype](fs, gain, wintype)

            if(r):
                self.draw_plots(taps, params)


    # Filter design functions using a window
    def design_win_lpf(self, fs, gain, wintype):
        ret = True
        pb,r = self.gui.endofLpfPassBandEdit.text().toDouble()
        ret = r and ret
        sb,r = self.gui.startofLpfStopBandEdit.text().toDouble()
        ret = r and ret
        atten,r = self.gui.lpfStopBandAttenEdit.text().toDouble()
        ret = r and ret

        if(ret):
            tb = sb - pb
            
            taps = gr.firdes.low_pass_2(gain, fs, pb, tb,
                                        atten, wintype)
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "lpf", "pbend": pb, "sbstart": sb,
                      "atten": atten, "ntaps": len(taps)}
            return (taps, params, ret)
        else:
            return ([], [], ret)
    
    def design_win_bpf(self, fs, gain, wintype):
        ret = True
        pb1,r = self.gui.startofBpfPassBandEdit.text().toDouble()
        ret = r and ret
        pb2,r = self.gui.endofBpfPassBandEdit.text().toDouble()
        ret = r and ret
        tb,r  = self.gui.bpfTransitionEdit.text().toDouble()
        ret = r and ret
        atten,r = self.gui.bpfStopBandAttenEdit.text().toDouble()
        ret = r and ret

        if(r):
            taps = gr.firdes.band_pass_2(gain, fs, pb1, pb2, tb,
                                         atten, wintype)
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "bpf", "pbstart": pb1, "pbend": pb2,
                      "tb": tb, "atten": atten, "ntaps": len(taps)}
            return (taps,params,r)
        else:
            return ([],[],r)

    def design_win_cbpf(self, fs, gain, wintype):
        ret = True
        pb1,r = self.gui.startofBpfPassBandEdit.text().toDouble()
        ret = r and ret
        pb2,r = self.gui.endofBpfPassBandEdit.text().toDouble()
        ret = r and ret
        tb,r  = self.gui.bpfTransitionEdit.text().toDouble()
        ret = r and ret
        atten,r = self.gui.bpfStopBandAttenEdit.text().toDouble()
        ret = r and ret

        if(r):
            taps = gr.firdes.complex_band_pass_2(gain, fs, pb1, pb2, tb,
                                                 atten, wintype)
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "cbpf", "pbstart": pb1, "pbend": pb2,
                      "tb": tb, "atten": atten, "ntaps": len(taps)}
            return (taps,params,r)
        else:
            return ([],[],r)

    def design_win_bnf(self, fs, gain, wintype):
        ret = True
        pb1,r = self.gui.startofBnfStopBandEdit.text().toDouble()
        ret = r and ret
        pb2,r = self.gui.endofBnfStopBandEdit.text().toDouble()
        ret = r and ret
        tb,r  = self.gui.bnfTransitionEdit.text().toDouble()
        ret = r and ret
        atten,r = self.gui.bnfStopBandAttenEdit.text().toDouble()
        ret = r and ret

        if(r):
            taps = gr.firdes.band_reject_2(gain, fs, pb1, pb2, tb,
                                           atten, wintype)
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "bnf", "sbstart": pb1, "sbend": pb2,
                      "tb": tb, "atten": atten, "ntaps": len(taps)}
            return (taps,params,r)
        else:
            return ([],[],r)

    def design_win_hpf(self, fs, gain, wintype):
        ret = True
        sb,r = self.gui.endofHpfStopBandEdit.text().toDouble()
        ret = r and ret
        pb,r = self.gui.startofHpfPassBandEdit.text().toDouble()
        ret = r and ret
        atten,r = self.gui.hpfStopBandAttenEdit.text().toDouble()
        ret = r and ret

        if(r):
            tb = pb - sb
            taps = gr.firdes.high_pass_2(gain, fs, pb, tb,
                                         atten, wintype)            
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "hpf", "sbend": sb, "pbstart": pb,
                      "atten": atten, "ntaps": len(taps)}
            return (taps,params,r)
        else:
            return ([],[],r)

    def design_win_rrc(self, fs, gain, wintype):
        ret = True
        sr,r = self.gui.rrcSymbolRateEdit.text().toDouble()
        ret = r and ret
        alpha,r = self.gui.rrcAlphaEdit.text().toDouble()
        ret = r and ret
        ntaps,r = self.gui.rrcNumTapsEdit.text().toInt()
        ret = r and ret

        if(r):
            taps = gr.firdes.root_raised_cosine(gain, fs, sr,
                                                alpha, ntaps)
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "rrc", "srate": sr, "alpha": alpha,
                      "ntaps": ntaps}
            return (taps,params,r)
        else:
            return ([],[],r)

    def design_win_gaus(self, fs, gain, wintype):
        ret = True
        sr,r = self.gui.gausSymbolRateEdit.text().toDouble()
        ret = r and ret
        bt,r = self.gui.gausBTEdit.text().toDouble()
        ret = r and ret
        ntaps,r = self.gui.gausNumTapsEdit.text().toInt()
        ret = r and ret

        if(r):
            spb = fs / sr
            taps = gr.firdes.gaussian(gain, spb, bt, ntaps)
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "gaus", "srate": sr, "bt": bt,
                      "ntaps": ntaps}
            return (taps,params,r)
        else:
            return ([],[],r)

    # Design Functions for Equiripple Filters
    def design_opt_lpf(self, fs, gain):
        ret = True
        pb,r = self.gui.endofLpfPassBandEdit.text().toDouble()
        ret = r and ret
        sb,r = self.gui.startofLpfStopBandEdit.text().toDouble()
        ret = r and ret
        atten,r = self.gui.lpfStopBandAttenEdit.text().toDouble()
        ret = r and ret
        ripple,r = self.gui.lpfPassBandRippleEdit.text().toDouble()
        ret = r and ret

        if(ret):
            try:
                taps = blks2.optfir.low_pass(gain, fs, pb, sb,
                                             ripple, atten)
            except RuntimeError, e:
                reply = QtGui.QMessageBox.information(self, "Filter did not converge",
                                                      e.args[0], "&Ok")
                return ([],[],False)
            else:
                params = {"fs": fs, "gain": gain, "wintype": self.EQUIRIPPLE_FILT,
                          "filttype": "lpf", "pbend": pb, "sbstart": sb,
                          "atten": atten, "ripple": ripple, "ntaps": len(taps)}
                return (taps, params, ret)
        else:
            return ([], [], ret)
    
    def design_opt_bpf(self, fs, gain):
        ret = True
        pb1,r = self.gui.startofBpfPassBandEdit.text().toDouble()
        ret = r and ret
        pb2,r = self.gui.endofBpfPassBandEdit.text().toDouble()
        ret = r and ret
        tb,r  = self.gui.bpfTransitionEdit.text().toDouble()
        ret = r and ret
        atten,r = self.gui.bpfStopBandAttenEdit.text().toDouble()
        ret = r and ret
        ripple,r = self.gui.bpfPassBandRippleEdit.text().toDouble()
        ret = r and ret

        if(r):
            sb1 = pb1 - tb
            sb2 = pb2 + tb
            try:
                taps = blks2.optfir.band_pass(gain, fs, sb1, pb1, pb2, sb2,
                                              ripple, atten)
            except RuntimeError, e:
                reply = QtGui.QMessageBox.information(self, "Filter did not converge",
                                                      e.args[0], "&Ok")
                return ([],[],False)

            else:
                params = {"fs": fs, "gain": gain, "wintype": self.EQUIRIPPLE_FILT,
                          "filttype": "bpf", "pbstart": pb1, "pbend": pb2,
                          "tb": tb, "atten": atten, "ripple": ripple,
                          "ntaps": len(taps)}
                return (taps,params,r)
        else:
            return ([],[],r)

    def design_opt_cbpf(self, fs, gain):
        ret = True
        pb1,r = self.gui.startofBpfPassBandEdit.text().toDouble()
        ret = r and ret
        pb2,r = self.gui.endofBpfPassBandEdit.text().toDouble()
        ret = r and ret
        tb,r  = self.gui.bpfTransitionEdit.text().toDouble()
        ret = r and ret
        atten,r = self.gui.bpfStopBandAttenEdit.text().toDouble()
        ret = r and ret
        ripple,r = self.gui.bpfPassBandRippleEdit.text().toDouble()
        ret = r and ret

        if(r):
            sb1 = pb1 - tb
            sb2 = pb2 + tb
            try:
                taps = blks2.optfir.complex_band_pass(gain, fs, sb1, pb1, pb2, sb2,
                                                      ripple, atten)
            except RuntimeError, e:
                reply = QtGui.QMessageBox.information(self, "Filter did not converge",
                                                      e.args[0], "&Ok")
                return ([],[],False)
            else:
                params = {"fs": fs, "gain": gain, "wintype": self.EQUIRIPPLE_FILT,
                          "filttype": "cbpf", "pbstart": pb1, "pbend": pb2,
                          "tb": tb, "atten": atten, "ripple": ripple,
                          "ntaps": len(taps)}
                return (taps,params,r)
        else:
            return ([],[],r)

    def design_opt_bnf(self, fs, gain):
        ret = True
        sb1,r = self.gui.startofBnfStopBandEdit.text().toDouble()
        ret = r and ret
        sb2,r = self.gui.endofBnfStopBandEdit.text().toDouble()
        ret = r and ret
        tb,r  = self.gui.bnfTransitionEdit.text().toDouble()
        ret = r and ret
        atten,r = self.gui.bnfStopBandAttenEdit.text().toDouble()
        ret = r and ret
        ripple,r = self.gui.bnfPassBandRippleEdit.text().toDouble()
        ret = r and ret

        if(r):
            pb1 = sb1 - tb
            pb2 = sb2 + tb
            try:
                taps = blks2.optfir.band_reject(gain, fs, pb1, sb1, sb2, pb2,
                                                ripple, atten)
            except RuntimeError, e:
                reply = QtGui.QMessageBox.information(self, "Filter did not converge",
                                                      e.args[0], "&Ok")
                return ([],[],False)
            else:
                params = {"fs": fs, "gain": gain, "wintype": self.EQUIRIPPLE_FILT,
                          "filttype": "bnf", "sbstart": pb1, "sbend": pb2,
                          "tb": tb, "atten": atten, "ripple": ripple,
                          "ntaps": len(taps)}
                return (taps,params,r)
        else:
            return ([],[],r)

    def design_opt_hpf(self, fs, gain):
        ret = True
        sb,r = self.gui.endofHpfStopBandEdit.text().toDouble()
        ret = r and ret
        pb,r = self.gui.startofHpfPassBandEdit.text().toDouble()
        ret = r and ret
        atten,r = self.gui.hpfStopBandAttenEdit.text().toDouble()
        ret = r and ret
        ripple,r = self.gui.hpfPassBandRippleEdit.text().toDouble()
        ret = r and ret

        if(r):
            try:
                taps = blks2.optfir.high_pass(gain, fs, sb, pb,
                                              atten, ripple)
            except RuntimeError, e:
                reply = QtGui.QMessageBox.information(self, "Filter did not converge",
                                                      e.args[0], "&Ok")
                return ([],[],False)
            else:
                params = {"fs": fs, "gain": gain, "wintype": self.EQUIRIPPLE_FILT,
                          "filttype": "hpf", "sbend": sb, "pbstart": pb,
                          "atten": atten, "ripple": ripple,
                          "ntaps": len(taps)}
                return (taps,params,r)
        else:
            return ([],[],r)

    def nfft_edit_changed(self, nfft):
        infft,r = nfft.toInt()
        if(r and (infft != self.nfftpts)):
            self.nfftpts = infft
            self.update_freq_curves()

    def tab_changed(self, tab):
        if(tab == 0):
            self.update_freq_curves()
        if(tab == 1):
            self.update_time_curves()
        if(tab == 2):
            self.update_phase_curves()
        if(tab == 3):
            self.update_group_curves()
        
    def get_fft(self, fs, taps, Npts):
        Ts = 1.0/fs
        fftpts = fftpack.fft(taps, Npts)
        self.freq = scipy.arange(0, fs, 1.0/(Npts*Ts))        
        self.fftdB = 20.0*scipy.log10(abs(fftpts))
        self.fftDeg = scipy.unwrap(scipy.angle(fftpts))
        self.groupDelay = -scipy.diff(self.fftDeg)
        
    def update_time_curves(self):
        ntaps = len(self.taps)
        if(ntaps > 0):
            if(type(self.taps[0]) == scipy.complex128):
                self.rcurve.setData(scipy.arange(ntaps), self.taps.real)
                self.icurve.setData(scipy.arange(ntaps), self.taps.imag)
            else:
                self.rcurve.setData(scipy.arange(ntaps), self.taps)

            # Reset the x-axis to the new time scale
            ymax = 1.5 * max(self.taps)
            ymin = 1.5 * min(self.taps)
            self.gui.timePlot.setAxisScale(self.gui.timePlot.xBottom,
                                           0, ntaps)
            self.gui.timePlot.setAxisScale(self.gui.timePlot.yLeft,
                                           ymin, ymax)
            
            # Set the zoomer base to unzoom to the new axis
            self.timeZoomer.setZoomBase()
            
            self.gui.timePlot.replot()
        
    def update_freq_curves(self):
        npts = len(self.fftdB)
        if(npts > 0):
            self.freqcurve.setData(self.freq, self.fftdB)
            
            # Reset the x-axis to the new time scale
            ymax = 1.5 * max(self.fftdB[0:npts/2])
            ymin = 1.1 * min(self.fftdB[0:npts/2])
            xmax = self.freq[npts/2]
            xmin = self.freq[0]
            self.gui.freqPlot.setAxisScale(self.gui.freqPlot.xBottom,
                                           xmin, xmax)
            self.gui.freqPlot.setAxisScale(self.gui.freqPlot.yLeft,
                                           ymin, ymax)
            
            # Set the zoomer base to unzoom to the new axis
            self.freqZoomer.setZoomBase()
            
            self.gui.freqPlot.replot()


    def update_phase_curves(self):
        npts = len(self.fftDeg)
        if(npts > 0):
            self.phasecurve.setData(self.freq, self.fftDeg)
            
            # Reset the x-axis to the new time scale
            ymax = 1.5 * max(self.fftDeg[0:npts/2])
            ymin = 1.1 * min(self.fftDeg[0:npts/2])
            xmax = self.freq[npts/2]
            xmin = self.freq[0]
            self.gui.phasePlot.setAxisScale(self.gui.phasePlot.xBottom,
                                            xmin, xmax)
            self.gui.phasePlot.setAxisScale(self.gui.phasePlot.yLeft,
                                            ymin, ymax)
            
            # Set the zoomer base to unzoom to the new axis
            self.phaseZoomer.setZoomBase()
            
            self.gui.phasePlot.replot()

    def update_group_curves(self):
        npts = len(self.groupDelay)
        if(npts > 0):
            self.groupcurve.setData(self.freq, self.groupDelay)
            
            # Reset the x-axis to the new time scale
            ymax = 1.5 * max(self.groupDelay[0:npts/2])
            ymin = 1.1 * min(self.groupDelay[0:npts/2])
            xmax = self.freq[npts/2]
            xmin = self.freq[0]
            self.gui.groupPlot.setAxisScale(self.gui.groupPlot.xBottom,
                                            xmin, xmax)
            self.gui.groupPlot.setAxisScale(self.gui.groupPlot.yLeft,
                                            ymin, ymax)
            
            # Set the zoomer base to unzoom to the new axis
            self.groupZoomer.setZoomBase()
            
            self.gui.groupPlot.replot()

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
        for k in self.params.keys():
            csvhandle.writerow([k, self.params[k]])
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
        taps = []
        params = {}
        for row in csvhandle:
            if(row[0] != "taps"):
                testcpx = re.findall("[+-]?\d+\.*\d*[Ee]?[-+]?\d+j", row[1])
                if(len(testcpx) > 0): # it's a complex
                    params[row[0]] = complex(row[1])
                else: # assume it's a float
                    try: # if it's not a float, its a string
                        params[row[0]] = float(row[1])
                    except ValueError:
                        params[row[0]] = row[1]
            else:
                testcpx = re.findall("[+-]?\d+\.*\d*[Ee]?[-+]?\d+j", row[1])
                if(len(testcpx) > 0): # it's a complex
                    taps = [complex(r) for r in row[1:]]
                else:
                    taps = [float(r) for r in row[1:]]
        handle.close()
        self.draw_plots(taps, params)

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

    def draw_plots(self, taps, params):
        self.params = params
        self.taps = scipy.array(taps)
        self.get_fft(self.params["fs"], self.taps, self.nfftpts)
        self.update_time_curves()
        self.update_freq_curves()
        self.update_phase_curves()
        self.update_group_curves()
        
        self.gui.nTapsEdit.setText(Qt.QString("%1").arg(self.taps.size))
        

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

