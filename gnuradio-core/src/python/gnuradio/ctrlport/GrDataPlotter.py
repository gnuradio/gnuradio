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

from gnuradio import gr
import sys, time

try:
    from gnuradio import qtgui
    from PyQt4 import QtGui, QtCore
    import sip
except ImportError:
    print "Error: Program requires PyQt4 and gr-qtgui."
    sys.exit(1)

class GrDataPlotterC(gr.top_block):
    def __init__(self, name, rate, pmin=None, pmax=None):
        gr.top_block.__init__(self)

        self._name = name
        self._npts = 500
        samp_rate = 1.0

        self._last_data = self._npts*[0,]
        self._data_len = 0

        self.src = gr.vector_source_c([])
        self.thr = gr.throttle(gr.sizeof_gr_complex, rate)
        self.snk = qtgui.time_sink_c(self._npts, samp_rate,
                                     self._name, 1)
        self.snk.enable_autoscale(True)

        self.connect(self.src, self.thr, (self.snk, 0))

        self.snk.set_line_label(0, "Real")
        self.snk.set_line_label(1, "Imag")

        self.py_window = sip.wrapinstance(self.snk.pyqwidget(), QtGui.QWidget)

    def __del__(self):
        pass

    def qwidget(self):
        return self.py_window

    def name(self):
        return self._name

    def semilogy(self, en=True):
        self.snk.enable_semilogy(en)

    def update(self, data):
        # Ask GUI if there has been a change in nsamps
        npts = self.snk.nsamps()
        if(self._npts != npts):

            # Adjust buffers to accomodate new settings
            if(npts < self._npts):
                if(self._data_len < npts):
                    self._last_data = self._last_data[0:npts]
                else:
                    self._last_data = self._last_data[self._data_len-npts:self._data_len]
                    self._data_len = npts
            else:
                self._last_data += (npts - self._npts)*[0,]
            self._npts = npts
            self.snk.reset()
        
        # Update the plot data depending on type
        if(type(data) == list):
            data_r = data[0::2]
            data_i = data[1::2]
            data = [complex(r,i) for r,i in zip(data_r, data_i)]
            if(len(data) > self._npts):
                self.src.set_data(data)
                self._last_data = data[-self._npts:]
            else:
                newdata = self._last_data[-(self._npts-len(data)):]
                newdata += data
                self.src.set_data(newdata)
                self._last_data = newdata

        else: # single value update
            if(self._data_len < self._npts):
                self._last_data[self._data_len] = data
                self._data_len += 1
            else:
                self._last_data = self._last_data[1:]
                self._last_data.append(data)
            self.src.set_data(self._last_data)

class GrDataPlotterF(gr.top_block):
    def __init__(self, name, rate, pmin=None, pmax=None):
        gr.top_block.__init__(self)

        self._name = name
        self._npts = 500
        samp_rate = 1.0

        self._last_data = self._npts*[0,]
        self._data_len = 0

        self.src = gr.vector_source_f([])
        self.thr = gr.throttle(gr.sizeof_float, rate)
        self.snk = qtgui.time_sink_f(self._npts, samp_rate,
                                     self._name, 1)
        self.snk.enable_autoscale(True)

        self.connect(self.src, self.thr, (self.snk, 0))

        self.py_window = sip.wrapinstance(self.snk.pyqwidget(), QtGui.QWidget)

    def __del__(self):
        pass

    def qwidget(self):
        return self.py_window

    def name(self):
        return self._name

    def semilogy(self, en=True):
        self.snk.enable_semilogy(en)

    def update(self, data):
        # Ask GUI if there has been a change in nsamps
        npts = self.snk.nsamps()
        if(self._npts != npts):

            # Adjust buffers to accomodate new settings
            if(npts < self._npts):
                if(self._data_len < npts):
                    self._last_data = self._last_data[0:npts]
                else:
                    self._last_data = self._last_data[self._data_len-npts:self._data_len]
                    self._data_len = npts
            else:
                self._last_data += (npts - self._npts)*[0,]
            self._npts = npts
            self.snk.reset()
        
        # Update the plot data depending on type
        if(type(data) == list):
            if(len(data) > self._npts):
                self.src.set_data(data)
                self._last_data = data[-self._npts:]
            else:
                newdata = self._last_data[-(self._npts-len(data)):]
                newdata += data
                self.src.set_data(newdata)
                self._last_data = newdata

        else: # single value update
            if(self._data_len < self._npts):
                self._last_data[self._data_len] = data
                self._data_len += 1
            else:
                self._last_data = self._last_data[1:]
                self._last_data.append(data)
            self.src.set_data(self._last_data)


class GrDataPlotterConst(gr.top_block):
    def __init__(self, name, rate, pmin=None, pmax=None):
        gr.top_block.__init__(self)

        self._name = name
        self._npts = 500
        samp_rate = 1.0

        self._last_data = self._npts*[0,]
        self._data_len = 0

        self.src = gr.vector_source_c([])
        self.thr = gr.throttle(gr.sizeof_gr_complex, rate)
        self.snk = qtgui.const_sink_c(self._npts,
                                      self._name, 1)
        self.snk.enable_autoscale(True)

        self.connect(self.src, self.thr, (self.snk, 0))

        self.py_window = sip.wrapinstance(self.snk.pyqwidget(), QtGui.QWidget)

    def __del__(self):
        pass

    def qwidget(self):
        return self.py_window

    def name(self):
        return self._name

    def update(self, data):
        # Ask GUI if there has been a change in nsamps
        npts = self.snk.nsamps()
        if(self._npts != npts):

            # Adjust buffers to accomodate new settings
            if(npts < self._npts):
                if(self._data_len < npts):
                    self._last_data = self._last_data[0:npts]
                else:
                    self._last_data = self._last_data[self._data_len-npts:self._data_len]
                    self._data_len = npts
            else:
                self._last_data += (npts - self._npts)*[0,]
            self._npts = npts
            self.snk.reset()
        
        # Update the plot data depending on type
        if(type(data) == list):
            data_r = data[0::2]
            data_i = data[1::2]
            data = [complex(r,i) for r,i in zip(data_r, data_i)]
            if(len(data) > self._npts):
                self.src.set_data(data)
                self._last_data = data[-self._npts:]
            else:
                newdata = self._last_data[-(self._npts-len(data)):]
                newdata += data
                self.src.set_data(newdata)
                self._last_data = newdata

        else: # single value update
            if(self._data_len < self._npts):
                self._last_data[self._data_len] = data
                self._data_len += 1
            else:
                self._last_data = self._last_data[1:]
                self._last_data.append(data)
            self.src.set_data(self._last_data)


class GrDataPlotterPsdC(gr.top_block):
    def __init__(self, name, rate, pmin=None, pmax=None):
        gr.top_block.__init__(self)

        self._name = name
        self._samp_rate = 1.0
        self._fftsize = 2048
        self._wintype = gr.firdes.WIN_BLACKMAN_hARRIS
        self._fc = 0
        
        self._last_data = self._fftsize*[0,]
        self._data_len = 0

        self.src = gr.vector_source_c([])
        self.thr = gr.throttle(gr.sizeof_gr_complex, rate)
        self.snk = qtgui.freq_sink_c(self._fftsize, self._wintype,
                                     self._fc, self._samp_rate,
                                     self._name, 1)
        self.snk.enable_autoscale(True)

        self.connect(self.src, self.thr, (self.snk, 0))

        self.py_window = sip.wrapinstance(self.snk.pyqwidget(), QtGui.QWidget)

    def __del__(self):
        pass

    def qwidget(self):
        return self.py_window

    def name(self):
        return self._name

    def update(self, data):
        # Ask GUI if there has been a change in nsamps
        fftsize = self.snk.fft_size()
        if(self._fftsize != fftsize):

            # Adjust buffers to accomodate new settings
            if(fftsize < self._fftsize):
                if(self._data_len < fftsize):
                    self._last_data = self._last_data[0:fftsize]
                else:
                    self._last_data = self._last_data[self._data_len-fftsize:self._data_len]
                    self._data_len = fftsize
            else:
                self._last_data += (fftsize - self._fftsize)*[0,]
            self._fftsize = fftsize
            self.snk.reset()
        
        # Update the plot data depending on type
        if(type(data) == list):
            data_r = data[0::2]
            data_i = data[1::2]
            data = [complex(r,i) for r,i in zip(data_r, data_i)]
            if(len(data) > self._fftsize):
                self.src.set_data(data)
                self._last_data = data[-self._fftsize:]
            else:
                newdata = self._last_data[-(self._fftsize-len(data)):]
                newdata += data
                self.src.set_data(newdata)
                self._last_data = newdata

        else: # single value update
            if(self._data_len < self._fftsize):
                self._last_data[self._data_len] = data
                self._data_len += 1
            else:
                self._last_data = self._last_data[1:]
                self._last_data.append(data)
            self.src.set_data(self._last_data)

class GrDataPlotterPsdF(gr.top_block):
    def __init__(self, name, rate, pmin=None, pmax=None):
        gr.top_block.__init__(self)

        self._name = name
        self._samp_rate = 1.0
        self._fftsize = 2048
        self._wintype = gr.firdes.WIN_BLACKMAN_hARRIS
        self._fc = 0
        
        self._last_data = self._fftsize*[0,]
        self._data_len = 0

        self.src = gr.vector_source_f([])
        self.thr = gr.throttle(gr.sizeof_float, rate)
        self.snk = qtgui.freq_sink_f(self._fftsize, self._wintype,
                                     self._fc, self._samp_rate,
                                     self._name, 1)
        self.snk.enable_autoscale(True)

        self.connect(self.src, self.thr, (self.snk, 0))

        self.py_window = sip.wrapinstance(self.snk.pyqwidget(), QtGui.QWidget)

    def __del__(self):
        pass

    def qwidget(self):
        return self.py_window

    def name(self):
        return self._name

    def update(self, data):
        # Ask GUI if there has been a change in nsamps
        fftsize = self.snk.fft_size()
        if(self._fftsize != fftsize):

            # Adjust buffers to accomodate new settings
            if(fftsize < self._fftsize):
                if(self._data_len < fftsize):
                    self._last_data = self._last_data[0:fftsize]
                else:
                    self._last_data = self._last_data[self._data_len-fftsize:self._data_len]
                    self._data_len = fftsize
            else:
                self._last_data += (fftsize - self._fftsize)*[0,]
            self._fftsize = fftsize
            self.snk.reset()
        
        # Update the plot data depending on type
        if(type(data) == list):
            data_r = data[0::2]
            data_i = data[1::2]
            data = [complex(r,i) for r,i in zip(data_r, data_i)]
            if(len(data) > self._fftsize):
                self.src.set_data(data)
                self._last_data = data[-self._fftsize:]
            else:
                newdata = self._last_data[-(self._fftsize-len(data)):]
                newdata += data
                self.src.set_data(newdata)
                self._last_data = newdata

        else: # single value update
            if(self._data_len < self._fftsize):
                self._last_data[self._data_len] = data
                self._data_len += 1
            else:
                self._last_data = self._last_data[1:]
                self._last_data.append(data)
            self.src.set_data(self._last_data)


class GrTimeRasterF(gr.top_block):
    def __init__(self, name, rate, pmin=None, pmax=None):
        gr.top_block.__init__(self)

        self._name = name
        self._npts = 100
        self._rows = 100
        samp_rate = 1.0

        self._last_data = self._npts*[0,]
        self._data_len = 0

        self.src = gr.vector_source_f([])
        self.thr = gr.throttle(gr.sizeof_float, rate)
        self.snk = qtgui.time_raster_sink_f(samp_rate, self._npts, self._rows,
                                            [], [], self._name, 1)

        self.connect(self.src, self.thr, (self.snk, 0))

        self.py_window = sip.wrapinstance(self.snk.pyqwidget(), QtGui.QWidget)

    def __del__(self):
        pass

    def qwidget(self):
        return self.py_window

    def name(self):
        return self._name

    def update(self, data):
        # Ask GUI if there has been a change in nsamps
        npts = int(self.snk.num_cols())
        if(self._npts != npts):

            # Adjust buffers to accomodate new settings
            if(npts < self._npts):
                if(self._data_len < npts):
                    self._last_data = self._last_data[0:npts]
                else:
                    self._last_data = self._last_data[self._data_len-npts:self._data_len]
                    self._data_len = npts
            else:
                self._last_data += (npts - self._npts)*[0,]
            self._npts = npts
            self.snk.reset()
        
        # Update the plot data depending on type
        if(type(data) == list):
            if(len(data) > self._npts):
                self.src.set_data(data)
                self._last_data = data[-self._npts:]
            else:
                newdata = self._last_data[-(self._npts-len(data)):]
                newdata += data
                self.src.set_data(newdata)
                self._last_data = newdata

        else: # single value update
            if(self._data_len < self._npts):
                self._last_data[self._data_len] = data
                self._data_len += 1
            else:
                self._last_data = self._last_data[1:]
                self._last_data.append(data)
            self.src.set_data(self._last_data)

class GrTimeRasterB(gr.top_block):
    def __init__(self, name, rate, pmin=None, pmax=None):
        gr.top_block.__init__(self)

        self._name = name
        self._npts = 100
        self._rows = 100
        samp_rate = 1.0

        self._last_data = self._npts*[0,]
        self._data_len = 0

        self.src = gr.vector_source_b([])
        self.thr = gr.throttle(gr.sizeof_char, rate)
        self.snk = qtgui.time_raster_sink_b(samp_rate, self._npts, self._rows,
                                            [], [], self._name, 1)

        self.connect(self.src, self.thr, (self.snk, 0))

        self.py_window = sip.wrapinstance(self.snk.pyqwidget(), QtGui.QWidget)

    def __del__(self):
        pass

    def qwidget(self):
        return self.py_window

    def name(self):
        return self._name

    def update(self, data):
        # Ask GUI if there has been a change in nsamps
        npts = self.snk.num_cols()
        if(self._npts != npts):

            # Adjust buffers to accomodate new settings
            if(npts < self._npts):
                if(self._data_len < npts):
                    self._last_data = self._last_data[0:npts]
                else:
                    self._last_data = self._last_data[self._data_len-npts:self._data_len]
                    self._data_len = npts
            else:
                self._last_data += (npts - self._npts)*[0,]
            self._npts = npts
            self.snk.reset()
        
        # Update the plot data depending on type
        if(type(data) == list):
            if(len(data) > self._npts):
                self.src.set_data(data)
                self._last_data = data[-self._npts:]
            else:
                newdata = self._last_data[-(self._npts-len(data)):]
                newdata += data
                self.src.set_data(newdata)
                self._last_data = newdata

        else: # single value update
            if(self._data_len < self._npts):
                self._last_data[self._data_len] = data
                self._data_len += 1
            else:
                self._last_data = self._last_data[1:]
                self._last_data.append(data)
            self.src.set_data(self._last_data)


class GrDataPlotterValueTable:
    def __init__(self, uid, parent, x, y, xsize, ysize,
                 headers=['Statistic Key ( Source Block :: Stat Name )  ',
                          'Curent Value', 'Units', 'Description']):
	# must encapsulate, cuz Qt's bases are not classes
        self.uid = uid
        self.treeWidget = QtGui.QTreeWidget(parent)
        self.treeWidget.setColumnCount(len(headers))
        self.treeWidget.setGeometry(x,y,xsize,ysize)
        self.treeWidget.setHeaderLabels(headers)
        self.treeWidget.resizeColumnToContents(0)

    def updateItems(self, knobs, knobprops):
        items = [];
        self.treeWidget.clear()
        for k, v in knobs.iteritems():
            items.append(QtGui.QTreeWidgetItem([str(k), str(v.value),
                                                knobprops[k].units,
                                                knobprops[k].description]))
        self.treeWidget.insertTopLevelItems(0, items)
