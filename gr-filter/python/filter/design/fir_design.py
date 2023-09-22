# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import scipy
from gnuradio import filter, fft
from PyQt5 import QtGui, QtWidgets


# Filter design functions using a window
def design_win_lpf(fs, gain, wintype, mainwin):
    ret = True
    pb, r = getfloat(mainwin.gui.endofLpfPassBandEdit.text())
    ret = r and ret
    sb, r = getfloat(mainwin.gui.startofLpfStopBandEdit.text())
    ret = r and ret
    atten, r = getfloat(mainwin.gui.lpfStopBandAttenEdit.text())
    ret = r and ret

    if(ret):
        tb = sb - pb

        try:
            taps = filter.firdes.low_pass_2(gain, fs, pb, tb,
                                            atten, wintype)
        except (RuntimeError, IndexError) as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Runtime Error",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
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
    pb1, r = getfloat(mainwin.gui.startofBpfPassBandEdit.text())
    ret = r and ret
    pb2, r = getfloat(mainwin.gui.endofBpfPassBandEdit.text())
    ret = r and ret
    tb, r = getfloat(mainwin.gui.bpfTransitionEdit.text())
    ret = r and ret
    atten, r = getfloat(mainwin.gui.bpfStopBandAttenEdit.text())
    ret = r and ret

    if(ret):
        try:
            taps = filter.firdes.band_pass_2(gain, fs, pb1, pb2, tb,
                                             atten, wintype)
        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Runtime Error",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], ret)
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "bpf", "pbstart": pb1, "pbend": pb2,
                      "tb": tb, "atten": atten, "ntaps": len(taps)}
            return (taps, params, r)
    else:
        return ([], [], ret)


def design_win_cbpf(fs, gain, wintype, mainwin):
    ret = True
    pb1, r = getfloat(mainwin.gui.startofBpfPassBandEdit.text())
    ret = r and ret
    pb2, r = getfloat(mainwin.gui.endofBpfPassBandEdit.text())
    ret = r and ret
    tb, r = getfloat(mainwin.gui.bpfTransitionEdit.text())
    ret = r and ret
    atten, r = getfloat(mainwin.gui.bpfStopBandAttenEdit.text())
    ret = r and ret

    if(ret):
        try:
            taps = filter.firdes.complex_band_pass_2(gain, fs, pb1, pb2, tb,
                                                     atten, wintype)
        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Runtime Error",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], ret)
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "cbpf", "pbstart": pb1, "pbend": pb2,
                      "tb": tb, "atten": atten, "ntaps": len(taps)}
            return (taps, params, r)
    else:
        return ([], [], ret)


def design_win_bnf(fs, gain, wintype, mainwin):
    ret = True
    pb1, r = getfloat(mainwin.gui.startofBnfStopBandEdit.text())
    ret = r and ret
    pb2, r = getfloat(mainwin.gui.endofBnfStopBandEdit.text())
    ret = r and ret
    tb, r = getfloat(mainwin.gui.bnfTransitionEdit.text())
    ret = r and ret
    atten, r = getfloat(mainwin.gui.bnfStopBandAttenEdit.text())
    ret = r and ret

    if(ret):
        try:
            taps = filter.firdes.band_reject_2(gain, fs, pb1, pb2, tb,
                                               atten, wintype)
        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Runtime Error",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], ret)
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "bnf", "sbstart": pb1, "sbend": pb2,
                      "tb": tb, "atten": atten, "ntaps": len(taps)}
            return (taps, params, r)
    else:
        return ([], [], ret)


def design_win_hpf(fs, gain, wintype, mainwin):
    ret = True
    sb, r = getfloat(mainwin.gui.endofHpfStopBandEdit.text())
    ret = r and ret
    pb, r = getfloat(mainwin.gui.startofHpfPassBandEdit.text())
    ret = r and ret
    atten, r = getfloat(mainwin.gui.hpfStopBandAttenEdit.text())
    ret = r and ret

    if(ret):
        tb = pb - sb
        try:
            taps = filter.firdes.high_pass_2(gain, fs, pb, tb,
                                             atten, wintype)
        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Runtime Error",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "hpf", "sbend": sb, "pbstart": pb,
                      "atten": atten, "ntaps": len(taps)}
            return (taps, params, ret)
    else:
        return ([], [], ret)


def design_win_hb(fs, gain, wintype, mainwin):
    ret = True
    filtord, r = getfloat(mainwin.gui.firhbordEdit.text())
    ret = r and ret
    trwidth, r = getfloat(mainwin.gui.firhbtrEdit.text())
    ret = r and ret
    filtwin = {fft.window.WIN_HAMMING: 'hamming',
               fft.window.WIN_HANN: 'hanning',
               fft.window.WIN_BLACKMAN: 'blackman',
               fft.window.WIN_RECTANGULAR: 'boxcar',
               fft.window.WIN_KAISER: ('kaiser', 4.0),
               fft.window.WIN_BLACKMAN_hARRIS: 'blackmanharris'}

    if int(filtord) & 1:
        reply = QtWidgets.QMessageBox.information(mainwin, "Filter order should be even",
                                                  "Filter order should be even", QtWidgets.QMessageBox.Ok)
        return ([], [], False)

    if(ret):
        taps = scipy.signal.firwin(
            int(filtord) + 1, 0.5, window=filtwin[wintype])
        taps[abs(taps) <= 1e-6] = 0.
        params = {"fs": fs, "gain": gain, "wintype": wintype,
                  "filttype": "hb", "ntaps": len(taps)}
        return (taps, params, ret)
    else:
        return ([], [], ret)


def design_win_rrc(fs, gain, wintype, mainwin):
    ret = True
    sr, r = getfloat(mainwin.gui.rrcSymbolRateEdit.text())
    ret = r and ret
    alpha, r = getfloat(mainwin.gui.rrcAlphaEdit.text())
    ret = r and ret
    ntaps, r = getint(mainwin.gui.rrcNumTapsEdit.text())
    ret = r and ret

    if(ret):
        try:
            taps = filter.firdes.root_raised_cosine(gain, fs, sr,
                                                    alpha, ntaps)
        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Runtime Error",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "rrc", "srate": sr, "alpha": alpha,
                      "ntaps": ntaps}
            return (taps, params, ret)
    else:
        return ([], [], ret)


def design_win_gaus(fs, gain, wintype, mainwin):
    ret = True
    sr, r = getfloat(mainwin.gui.gausSymbolRateEdit.text())
    ret = r and ret
    bt, r = getfloat(mainwin.gui.gausBTEdit.text())
    ret = r and ret
    ntaps, r = getint(mainwin.gui.gausNumTapsEdit.text())
    ret = r and ret

    if(ret):
        spb = fs / sr
        try:
            taps = filter.firdes.gaussian(gain, spb, bt, ntaps)

        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Runtime Error",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
        else:
            params = {"fs": fs, "gain": gain, "wintype": wintype,
                      "filttype": "gaus", "srate": sr, "bt": bt,
                      "ntaps": ntaps}
            return (taps, params, ret)
    else:
        return ([], [], ret)


# Design Functions for Equiripple Filters
def design_opt_lpf(fs, gain, mainwin):
    ret = True
    pb, r = getfloat(mainwin.gui.endofLpfPassBandEdit.text())
    ret = r and ret
    sb, r = getfloat(mainwin.gui.startofLpfStopBandEdit.text())
    ret = r and ret
    atten, r = getfloat(mainwin.gui.lpfStopBandAttenEdit.text())
    ret = r and ret
    ripple, r = getfloat(mainwin.gui.lpfPassBandRippleEdit.text())
    ret = r and ret

    if(ret):
        try:
            taps = filter.optfir.low_pass(gain, fs, pb, sb,
                                          ripple, atten)
        except ValueError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Invalid filter parameters",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], False)
        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Filter did not converge",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], False)
        else:
            params = {"fs": fs, "gain": gain, "wintype": mainwin.EQUIRIPPLE_FILT,
                      "filttype": "lpf", "pbend": pb, "sbstart": sb,
                      "atten": atten, "ripple": ripple, "ntaps": len(taps)}
            return (taps, params, ret)
    else:
        return ([], [], ret)


def design_opt_bpf(fs, gain, mainwin):
    ret = True
    pb1, r = getfloat(mainwin.gui.startofBpfPassBandEdit.text())
    ret = r and ret
    pb2, r = getfloat(mainwin.gui.endofBpfPassBandEdit.text())
    ret = r and ret
    tb, r = getfloat(mainwin.gui.bpfTransitionEdit.text())
    ret = r and ret
    atten, r = getfloat(mainwin.gui.bpfStopBandAttenEdit.text())
    ret = r and ret
    ripple, r = getfloat(mainwin.gui.bpfPassBandRippleEdit.text())
    ret = r and ret

    if(r):
        sb1 = pb1 - tb
        sb2 = pb2 + tb
        try:
            taps = filter.optfir.band_pass(gain, fs, sb1, pb1, pb2, sb2,
                                           ripple, atten)
        except ValueError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Invalid filter parameters",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], False)
        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Filter did not converge",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], False)

        else:
            params = {"fs": fs, "gain": gain, "wintype": mainwin.EQUIRIPPLE_FILT,
                      "filttype": "bpf", "pbstart": pb1, "pbend": pb2,
                      "tb": tb, "atten": atten, "ripple": ripple,
                      "ntaps": len(taps)}
            return (taps, params, r)
    else:
        return ([], [], r)


def design_opt_cbpf(fs, gain, mainwin):
    ret = True
    pb1, r = getfloat(mainwin.gui.startofBpfPassBandEdit.text())
    ret = r and ret
    pb2, r = getfloat(mainwin.gui.endofBpfPassBandEdit.text())
    ret = r and ret
    tb, r = getfloat(mainwin.gui.bpfTransitionEdit.text())
    ret = r and ret
    atten, r = getfloat(mainwin.gui.bpfStopBandAttenEdit.text())
    ret = r and ret
    ripple, r = getfloat(mainwin.gui.bpfPassBandRippleEdit.text())
    ret = r and ret

    if(r):
        sb1 = pb1 - tb
        sb2 = pb2 + tb
        try:
            taps = filter.optfir.complex_band_pass(gain, fs, sb1, pb1, pb2, sb2,
                                                   ripple, atten)
        except ValueError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Invalid filter parameters",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], False)
        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Filter did not converge",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], False)
        else:
            params = {"fs": fs, "gain": gain, "wintype": mainwin.EQUIRIPPLE_FILT,
                      "filttype": "cbpf", "pbstart": pb1, "pbend": pb2,
                      "tb": tb, "atten": atten, "ripple": ripple,
                      "ntaps": len(taps)}
            return (taps, params, r)
    else:
        return ([], [], r)


def design_opt_bnf(fs, gain, mainwin):
    ret = True
    sb1, r = getfloat(mainwin.gui.startofBnfStopBandEdit.text())
    ret = r and ret
    sb2, r = getfloat(mainwin.gui.endofBnfStopBandEdit.text())
    ret = r and ret
    tb, r = getfloat(mainwin.gui.bnfTransitionEdit.text())
    ret = r and ret
    atten, r = getfloat(mainwin.gui.bnfStopBandAttenEdit.text())
    ret = r and ret
    ripple, r = getfloat(mainwin.gui.bnfPassBandRippleEdit.text())
    ret = r and ret

    if(ret):
        pb1 = sb1 - tb
        pb2 = sb2 + tb
        try:
            taps = filter.optfir.band_reject(gain, fs, pb1, sb1, sb2, pb2,
                                             ripple, atten)
        except ValueError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Invalid filter parameters",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], False)
        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Filter did not converge",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], False)
        else:
            params = {"fs": fs, "gain": gain, "wintype": mainwin.EQUIRIPPLE_FILT,
                      "filttype": "bnf", "sbstart": pb1, "sbend": pb2,
                      "tb": tb, "atten": atten, "ripple": ripple,
                      "ntaps": len(taps)}
            return (taps, params, ret)
    else:
        return ([], [], ret)


def design_opt_hb(fs, gain, mainwin):
    ret = True
    filtord, r = getfloat(mainwin.gui.firhbordEdit.text())
    ret = r and ret
    trwidth, r = getfloat(mainwin.gui.firhbtrEdit.text())
    ret = r and ret
    if int(filtord) & 1:
        reply = QtWidgets.QMessageBox.information(mainwin, "Filter order should be even",
                                                  "Filter order should be even", QtWidgets.QMessageBox.Ok)
        return ([], [], False)

    if(ret):
        try:
            bands = [0, .25 - (trwidth / fs), .25 + (trwidth / fs), 0.5]
            taps = scipy.signal.remez(int(filtord) + 1, bands, [1, 0], [1, 1])
            taps[abs(taps) <= 1e-6] = 0.
        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Filter Design Error",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], False)
        else:
            params = {"fs": fs, "gain": gain, "wintype": mainwin.EQUIRIPPLE_FILT,
                      "filttype": "hb", "ntaps": len(taps)}
            return (taps, params, ret)
    else:
        return ([], [], ret)


def design_opt_hpf(fs, gain, mainwin):
    ret = True
    sb, r = getfloat(mainwin.gui.endofHpfStopBandEdit.text())
    ret = r and ret
    pb, r = getfloat(mainwin.gui.startofHpfPassBandEdit.text())
    ret = r and ret
    atten, r = getfloat(mainwin.gui.hpfStopBandAttenEdit.text())
    ret = r and ret
    ripple, r = getfloat(mainwin.gui.hpfPassBandRippleEdit.text())
    ret = r and ret

    if(ret):
        try:
            taps = filter.optfir.high_pass(gain, fs, sb, pb,
                                           atten, ripple)
        except ValueError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Invalid filter parameters",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], False)
        except RuntimeError as e:
            reply = QtWidgets.QMessageBox.information(mainwin, "Filter did not converge",
                                                      e.args[0], QtWidgets.QMessageBox.Ok)
            return ([], [], False)
        else:
            params = {"fs": fs, "gain": gain, "wintype": mainwin.EQUIRIPPLE_FILT,
                      "filttype": "hpf", "sbend": sb, "pbstart": pb,
                      "atten": atten, "ripple": ripple,
                      "ntaps": len(taps)}
            return (taps, params, ret)
    else:
        return ([], [], ret)


def getint(value):
    try:
        return (int(value), True)
    except ValueError:
        return (0, False)


def getfloat(value):
    try:
        return (float(value), True)
    except ValueError:
        return ("NaN", False)
