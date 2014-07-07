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

import scipy
from gnuradio import filter
from PyQt4 import QtGui

# Filter design functions using a window
def design_win_lpf(fs, gain, wintype, mainwin):
    ret = True
    pb,r = mainwin.gui.endofLpfPassBandEdit.text().toDouble()
    ret = r and ret
    sb,r = mainwin.gui.startofLpfStopBandEdit.text().toDouble()
    ret = r and ret
    atten,r = mainwin.gui.lpfStopBandAttenEdit.text().toDouble()
    ret = r and ret

    if(ret):
        tb = sb - pb

        try:
            taps = filter.firdes.low_pass_2(gain, fs, pb, tb,
                                            atten, wintype)
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Runtime Error",
                                                  e.args[0], "&Ok")
            return ([], [], ret)
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "lpf", "pbend": pb, "sbstart": sb,
                      "atten": atten, "ntaps": len(taps)}
            return (taps, params, ret)
    else:
        return ([], [], ret)

def design_win_bpf(fs, gain, wintype, mainwin):
    ret = True
    pb1,r = mainwin.gui.startofBpfPassBandEdit.text().toDouble()
    ret = r and ret
    pb2,r = mainwin.gui.endofBpfPassBandEdit.text().toDouble()
    ret = r and ret
    tb,r  = mainwin.gui.bpfTransitionEdit.text().toDouble()
    ret = r and ret
    atten,r = mainwin.gui.bpfStopBandAttenEdit.text().toDouble()
    ret = r and ret

    if(ret):
        try:
            taps = filter.firdes.band_pass_2(gain, fs, pb1, pb2, tb,
                                             atten, wintype)
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Runtime Error",
                                                  e.args[0], "&Ok")
            return ([], [], ret)
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "bpf", "pbstart": pb1, "pbend": pb2,
                      "tb": tb, "atten": atten, "ntaps": len(taps)}
            return (taps,params,r)
    else:
        return ([],[],ret)

def design_win_cbpf(fs, gain, wintype, mainwin):
    ret = True
    pb1,r = mainwin.gui.startofBpfPassBandEdit.text().toDouble()
    ret = r and ret
    pb2,r = mainwin.gui.endofBpfPassBandEdit.text().toDouble()
    ret = r and ret
    tb,r  = mainwin.gui.bpfTransitionEdit.text().toDouble()
    ret = r and ret
    atten,r = mainwin.gui.bpfStopBandAttenEdit.text().toDouble()
    ret = r and ret

    if(ret):
        try:
            taps = filter.firdes.complex_band_pass_2(gain, fs, pb1, pb2, tb,
                                                     atten, wintype)
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Runtime Error",
                                                  e.args[0], "&Ok")
            return ([], [], ret)
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "cbpf", "pbstart": pb1, "pbend": pb2,
                      "tb": tb, "atten": atten, "ntaps": len(taps)}
            return (taps,params,r)
    else:
        return ([],[],ret)

def design_win_bnf(fs, gain, wintype, mainwin):
    ret = True
    pb1,r = mainwin.gui.startofBnfStopBandEdit.text().toDouble()
    ret = r and ret
    pb2,r = mainwin.gui.endofBnfStopBandEdit.text().toDouble()
    ret = r and ret
    tb,r  = mainwin.gui.bnfTransitionEdit.text().toDouble()
    ret = r and ret
    atten,r = mainwin.gui.bnfStopBandAttenEdit.text().toDouble()
    ret = r and ret

    if(ret):
        try:
            taps = filter.firdes.band_reject_2(gain, fs, pb1, pb2, tb,
                                               atten, wintype)
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Runtime Error",
                                                  e.args[0], "&Ok")
            return ([], [], ret)
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "bnf", "sbstart": pb1, "sbend": pb2,
                      "tb": tb, "atten": atten, "ntaps": len(taps)}
            return (taps,params,r)
    else:
        return ([],[],ret)

def design_win_hpf(fs, gain, wintype, mainwin):
    ret = True
    sb,r = mainwin.gui.endofHpfStopBandEdit.text().toDouble()
    ret = r and ret
    pb,r = mainwin.gui.startofHpfPassBandEdit.text().toDouble()
    ret = r and ret
    atten,r = mainwin.gui.hpfStopBandAttenEdit.text().toDouble()
    ret = r and ret

    if(ret):
        tb = pb - sb
        try:
            taps = filter.firdes.high_pass_2(gain, fs, pb, tb,
                                             atten, wintype)
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Runtime Error",
                                                  e.args[0], "&Ok")
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "hpf", "sbend": sb, "pbstart": pb,
                      "atten": atten, "ntaps": len(taps)}
            return (taps,params,ret)
    else:
        return ([],[],ret)

def design_win_hb(fs, gain, wintype, mainwin):
    ret = True
    filtord,r = mainwin.gui.firhbordEdit.text().toDouble()
    ret = r and ret
    trwidth,r = mainwin.gui.firhbtrEdit.text().toDouble()
    ret = r and ret
    filtwin = { filter.firdes.WIN_HAMMING : 'hamming',
                filter.firdes.WIN_HANN : 'hanning',
                filter.firdes.WIN_BLACKMAN : 'blackman',
                filter.firdes.WIN_RECTANGULAR: 'boxcar',
                filter.firdes.WIN_KAISER: ('kaiser', 4.0),
                filter.firdes.WIN_BLACKMAN_hARRIS: 'blackmanharris'}
    if int(filtord) & 1:
        reply = QtGui.QMessageBox.information(mainwin, "Filter order should be even",
                                              "Filter order should be even","&Ok")
        return ([],[],False)

    if(ret):
        taps = scipy.signal.firwin(int(filtord)+1, 0.5, window = filtwin[wintype])
        taps[abs(taps) <= 1e-6] = 0.
        params = {"fs": fs, "gain": gain, "wintype": wintype,
                  "filttype": "hb","ntaps": len(taps)}
        return (taps,params,ret)
    else:
        return ([],[],ret)

def design_win_rrc(fs, gain, wintype, mainwin):
    ret = True
    sr,r = mainwin.gui.rrcSymbolRateEdit.text().toDouble()
    ret = r and ret
    alpha,r = mainwin.gui.rrcAlphaEdit.text().toDouble()
    ret = r and ret
    ntaps,r = mainwin.gui.rrcNumTapsEdit.text().toInt()
    ret = r and ret

    if(ret):
        try:
            taps = filter.firdes.root_raised_cosine(gain, fs, sr,
                                                    alpha, ntaps)
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Runtime Error",
                                                  e.args[0], "&Ok")
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "rrc", "srate": sr, "alpha": alpha,
                      "ntaps": ntaps}
            return (taps,params,ret)
    else:
        return ([],[],ret)

def design_win_gaus(fs, gain, wintype, mainwin):
    ret = True
    sr,r = mainwin.gui.gausSymbolRateEdit.text().toDouble()
    ret = r and ret
    bt,r = mainwin.gui.gausBTEdit.text().toDouble()
    ret = r and ret
    ntaps,r = mainwin.gui.gausNumTapsEdit.text().toInt()
    ret = r and ret

    if(ret):
        spb = fs / sr
        try:
            taps = filter.firdes.gaussian(gain, spb, bt, ntaps)

        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Runtime Error",
                                                  e.args[0], "&Ok")
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "gaus", "srate": sr, "bt": bt,
                      "ntaps": ntaps}
            return (taps,params,ret)
    else:
        return ([],[],ret)

# Design Functions for Equiripple Filters
def design_opt_lpf(fs, gain, mainwin):
    ret = True
    pb,r = mainwin.gui.endofLpfPassBandEdit.text().toDouble()
    ret = r and ret
    sb,r = mainwin.gui.startofLpfStopBandEdit.text().toDouble()
    ret = r and ret
    atten,r = mainwin.gui.lpfStopBandAttenEdit.text().toDouble()
    ret = r and ret
    ripple,r = mainwin.gui.lpfPassBandRippleEdit.text().toDouble()
    ret = r and ret

    if(ret):
        try:
            taps = filter.optfir.low_pass(gain, fs, pb, sb,
                                          ripple, atten)
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Filter did not converge",
                                                  e.args[0], "&Ok")
            return ([],[],False)
        else:
            params = {"fs": fs, "gain": gain, "wintype": mainwin.EQUIRIPPLE_FILT,
                      "filttype": "lpf", "pbend": pb, "sbstart": sb,
                      "atten": atten, "ripple": ripple, "ntaps": len(taps)}
            return (taps, params, ret)
    else:
        return ([], [], ret)

def design_opt_bpf(fs, gain, mainwin):
    ret = True
    pb1,r = mainwin.gui.startofBpfPassBandEdit.text().toDouble()
    ret = r and ret
    pb2,r = mainwin.gui.endofBpfPassBandEdit.text().toDouble()
    ret = r and ret
    tb,r  = mainwin.gui.bpfTransitionEdit.text().toDouble()
    ret = r and ret
    atten,r = mainwin.gui.bpfStopBandAttenEdit.text().toDouble()
    ret = r and ret
    ripple,r = mainwin.gui.bpfPassBandRippleEdit.text().toDouble()
    ret = r and ret

    if(r):
        sb1 = pb1 - tb
        sb2 = pb2 + tb
        try:
            taps = filter.optfir.band_pass(gain, fs, sb1, pb1, pb2, sb2,
                                           ripple, atten)
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Filter did not converge",
                                                  e.args[0], "&Ok")
            return ([],[],False)

        else:
            params = {"fs": fs, "gain": gain, "wintype": mainwin.EQUIRIPPLE_FILT,
                      "filttype": "bpf", "pbstart": pb1, "pbend": pb2,
                      "tb": tb, "atten": atten, "ripple": ripple,
                      "ntaps": len(taps)}
            return (taps,params,r)
    else:
        return ([],[],r)

def design_opt_cbpf(fs, gain, mainwin):
    ret = True
    pb1,r = mainwin.gui.startofBpfPassBandEdit.text().toDouble()
    ret = r and ret
    pb2,r = mainwin.gui.endofBpfPassBandEdit.text().toDouble()
    ret = r and ret
    tb,r  = mainwin.gui.bpfTransitionEdit.text().toDouble()
    ret = r and ret
    atten,r = mainwin.gui.bpfStopBandAttenEdit.text().toDouble()
    ret = r and ret
    ripple,r = mainwin.gui.bpfPassBandRippleEdit.text().toDouble()
    ret = r and ret

    if(r):
        sb1 = pb1 - tb
        sb2 = pb2 + tb
        try:
            taps = filter.optfir.complex_band_pass(gain, fs, sb1, pb1, pb2, sb2,
                                                   ripple, atten)
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Filter did not converge",
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

def design_opt_bnf(fs, gain, mainwin):
    ret = True
    sb1,r = mainwin.gui.startofBnfStopBandEdit.text().toDouble()
    ret = r and ret
    sb2,r = mainwin.gui.endofBnfStopBandEdit.text().toDouble()
    ret = r and ret
    tb,r  = mainwin.gui.bnfTransitionEdit.text().toDouble()
    ret = r and ret
    atten,r = mainwin.gui.bnfStopBandAttenEdit.text().toDouble()
    ret = r and ret
    ripple,r = mainwin.gui.bnfPassBandRippleEdit.text().toDouble()
    ret = r and ret

    if(ret):
        pb1 = sb1 - tb
        pb2 = sb2 + tb
        try:
            taps = filter.optfir.band_reject(gain, fs, pb1, sb1, sb2, pb2,
                                             ripple, atten)
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Filter did not converge",
                                                  e.args[0], "&Ok")
            return ([],[],False)
        else:
            params = {"fs": fs, "gain": gain, "wintype": mainwin.EQUIRIPPLE_FILT,
                      "filttype": "bnf", "sbstart": pb1, "sbend": pb2,
                      "tb": tb, "atten": atten, "ripple": ripple,
                      "ntaps": len(taps)}
            return (taps,params,ret)
    else:
        return ([],[],ret)

def design_opt_hb(fs, gain, mainwin):
    ret = True
    filtord,r = mainwin.gui.firhbordEdit.text().toDouble()
    ret = r and ret
    trwidth,r = mainwin.gui.firhbtrEdit.text().toDouble()
    ret = r and ret
    if int(filtord) & 1:
        reply = QtGui.QMessageBox.information(mainwin, "Filter order should be even",
                                              "Filter order should be even","&Ok")
        return ([],[],False)

    if(ret):
        try:
            bands = [0,.25 - (trwidth/fs), .25 + (trwidth/fs), 0.5]
            taps = scipy.signal.remez(int(filtord)+1, bands, [1,0], [1,1])
            taps[abs(taps) <= 1e-6] = 0.
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Filter Design Error",
                                                  e.args[0], "&Ok")
            return ([],[],False)
        else:
            params = {"fs": fs, "gain": gain, "wintype": self.EQUIRIPPLE_FILT,
                      "filttype": "hb", "ntaps": len(taps)}
            return (taps,params,ret)
    else:
        return ([],[],ret)

def design_opt_hpf(fs, gain, mainwin):
    ret = True
    sb,r = mainwin.gui.endofHpfStopBandEdit.text().toDouble()
    ret = r and ret
    pb,r = mainwin.gui.startofHpfPassBandEdit.text().toDouble()
    ret = r and ret
    atten,r = mainwin.gui.hpfStopBandAttenEdit.text().toDouble()
    ret = r and ret
    ripple,r = mainwin.gui.hpfPassBandRippleEdit.text().toDouble()
    ret = r and ret

    if(ret):
        try:
            taps = filter.optfir.high_pass(gain, fs, sb, pb,
                                           atten, ripple)
        except RuntimeError, e:
            reply = QtGui.QMessageBox.information(mainwin, "Filter did not converge",
                                                  e.args[0], "&Ok")
            return ([],[],False)
        else:
            params = {"fs": fs, "gain": gain, "wintype": self.EQUIRIPPLE_FILT,
                      "filttype": "hpf", "sbend": sb, "pbstart": pb,
                      "atten": atten, "ripple": ripple,
                      "ntaps": len(taps)}
            return (taps,params,ret)
    else:
        return ([],[],ret)
