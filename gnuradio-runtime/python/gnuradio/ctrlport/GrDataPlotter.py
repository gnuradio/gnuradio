#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
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
from gnuradio import blocks
from gnuradio import filter
from gnuradio.ctrlport.GNURadio import ControlPort
import sys, time, struct

try:
    from gnuradio import qtgui
    from PyQt4 import QtGui, QtCore
    import sip
except ImportError:
    print "Error: Program requires PyQt4 and gr-qtgui."
    sys.exit(1)

class GrDataPlotParent(gr.top_block, QtGui.QWidget):
    # Setup signals
    plotupdated = QtCore.pyqtSignal(QtGui.QWidget)

    def __init__(self, name, rate, pmin=None, pmax=None):
        gr.top_block.__init__(self)
        QtGui.QWidget.__init__(self, None)

        self._name = name
        self._npts = 500
        self._rate = rate
        self.knobnames = [name,]

        self.layout = QtGui.QVBoxLayout()
        self.setLayout(self.layout)

        self.setAcceptDrops(True)

    def _setup(self, nconnections):
        self.stop()
        self.wait()

        if(self.layout.count() > 0):
            # Remove and disconnect. Making sure no references to snk
            # remain so that the plot gets deleted.
            self.layout.removeWidget(self.py_window)
            self.disconnect(self.thr, (self.snk, 0))
            self.disconnect(self.src[0], self.thr)
            for n in xrange(1, self._ncons):
                self.disconnect(self.src[n], (self.snk,n))

        self._ncons = nconnections
        self._data_len = self._ncons*[0,]

        self.thr = blocks.throttle(self._datasize, self._rate)
        self.snk = self.get_qtsink()

        self.connect(self.thr, (self.snk, 0))

        self._last_data = []
        self.src = []
        for n in xrange(self._ncons):
            self.set_line_label(n, self.knobnames[n])

            self._last_data.append(int(self._npts)*[0,])
            self.src.append(self.get_vecsource())

            if(n == 0):
                self.connect(self.src[n], self.thr)
            else:
                self.connect(self.src[n], (self.snk,n))

        self.py_window = sip.wrapinstance(self.snk.pyqwidget(), QtGui.QWidget)

        self.layout.addWidget(self.py_window)

    def __del__(self):
        pass

    def close(self):
        self.snk.close()

    def qwidget(self):
        return self.py_window

    def name(self):
        return self._name

    def semilogy(self, en=True):
        self.snk.enable_semilogy(en)

    def dragEnterEvent(self, e):
        e.acceptProposedAction()

    def dropEvent(self, e):
        if(e.mimeData().hasFormat("text/plain")):
            data = str(e.mimeData().text())

            #"PlotData:{0}:{1}".format(tag, iscomplex)
            datalst = data.split(":::")
            tag = datalst[0]
            name = datalst[1]
            cpx = datalst[2] != "0"

            if(tag == "PlotData" and cpx == self._iscomplex):
                self.knobnames.append(name)

                # create a new qwidget plot with the new data stream.
                self._setup(len(self.knobnames))

                # emit that this plot has been updated with a new qwidget.
                self.plotupdated.emit(self)

                e.acceptProposedAction()

    def data_to_complex(self, data):
        if(self._iscomplex):
            data_r = data[0::2]
            data_i = data[1::2]
            data = [complex(r,i) for r,i in zip(data_r, data_i)]
        return data

    def update(self, data):
        # Ask GUI if there has been a change in nsamps
        npts = self.get_npts()
        if(self._npts != npts):

            # Adjust buffers to accommodate new settings
            for n in xrange(self._ncons):
                if(npts < self._npts):
                    if(self._data_len[n] < npts):
                        self._last_data[n] = self._last_data[n][0:npts]
                    else:
                        self._last_data[n] = self._last_data[n][self._data_len[n]-npts:self._data_len[n]]
                        self._data_len[n] = npts
                else:
                    self._last_data[n] += (npts - self._npts)*[0,]
            self._npts = npts
            self.snk.reset()

        if(self._stripchart):
            # Update the plot data depending on type
            for n in xrange(self._ncons):
                if(type(data[n]) == list):
                    data[n] = self.data_to_complex(data[n])
                    if(len(data[n]) > self._npts):
                        self.src[n].set_data(data[n])
                        self._last_data[n] = data[n][-self._npts:]
                    else:
                        newdata = self._last_data[n][-(self._npts-len(data)):]
                        newdata += data[n]
                        self.src[n].set_data(newdata)
                        self._last_data[n] = newdata

                else: # single value update
                    if(self._iscomplex):
                        data[n] = complex(data[n][0], data[n][1])
                    if(self._data_len[n] < self._npts):
                        self._last_data[n][self._data_len[n]] = data[n]
                        self._data_len[n] += 1
                    else:
                        self._last_data[n] = self._last_data[n][1:]
                        self._last_data[n].append(data[n])
                    self.src[n].set_data(self._last_data[n])
        else:
            for n in xrange(self._ncons):
                if(type(data[n]) != list):
                    data[n] = [data[n],]
                data[n] = self.data_to_complex(data[n])
                self.src[n].set_data(data[n])



class GrDataPlotterC(GrDataPlotParent):
    def __init__(self, name, rate, pmin=None, pmax=None, stripchart=False):
        GrDataPlotParent.__init__(self, name, rate, pmin, pmax)

        self._stripchart = stripchart
        self._datasize = gr.sizeof_gr_complex
        self._iscomplex = True

        self._setup(1)

    def stem(self, en=True):
        self.snk.enable_stem_plot(en)

    def get_qtsink(self):
        snk = qtgui.time_sink_c(self._npts, 1.0,
                                self._name, self._ncons)
        snk.enable_autoscale(True)
        return snk

    def get_vecsource(self):
        return blocks.vector_source_c([])

    def get_npts(self):
        self._npts = self.snk.nsamps()
        return self._npts

    def set_line_label(self, n, name):
        self.snk.set_line_label(2*n+0, "Re{" + self.knobnames[n] + "}")
        self.snk.set_line_label(2*n+1, "Im{" + self.knobnames[n] + "}")


class GrDataPlotterF(GrDataPlotParent):
    def __init__(self, name, rate, pmin=None, pmax=None, stripchart=False):
        GrDataPlotParent.__init__(self, name, rate, pmin, pmax)

        self._stripchart = stripchart
        self._datasize = gr.sizeof_float
        self._iscomplex = False

        self._setup(1)

    def stem(self, en=True):
        self.snk.enable_stem_plot(en)

    def get_qtsink(self):
        snk = qtgui.time_sink_f(self._npts, 1.0,
                                self._name, self._ncons)
        snk.enable_autoscale(True)
        return snk

    def get_vecsource(self):
        return blocks.vector_source_f([])

    def get_npts(self):
        self._npts = self.snk.nsamps()
        return self._npts

    def set_line_label(self, n, name):
        self.snk.set_line_label(n, self.knobnames[n])


class GrDataPlotterConst(GrDataPlotParent):
    def __init__(self, name, rate, pmin=None, pmax=None, stripchart=False):
        GrDataPlotParent.__init__(self, name, rate, pmin, pmax)

        self._datasize = gr.sizeof_gr_complex
        self._stripchart = stripchart
        self._iscomplex = True

        self._setup(1)

    def get_qtsink(self):
        snk = qtgui.const_sink_c(self._npts,
                                 self._name,
                                 self._ncons)
        snk.enable_autoscale(True)
        return snk

    def get_vecsource(self):
        return blocks.vector_source_c([])

    def get_npts(self):
        self._npts = self.snk.nsamps()
        return self._npts

    def scatter(self, en=True):
        if(en):
            self.snk.set_line_style(0, 0)
        else:
            self.snk.set_line_style(0, 1)

    def set_line_label(self, n, name):
        self.snk.set_line_label(n, self.knobnames[n])


class GrDataPlotterPsdC(GrDataPlotParent):
    def __init__(self, name, rate, pmin=None, pmax=None):
        GrDataPlotParent.__init__(self, name, rate, pmin, pmax)

        self._datasize = gr.sizeof_gr_complex
        self._stripchart = True
        self._iscomplex = True

        self._npts = 2048
        self._wintype = filter.firdes.WIN_BLACKMAN_hARRIS
        self._fc = 0

        self._setup(1)

    def get_qtsink(self):
        snk = qtgui.freq_sink_c(self._npts, self._wintype,
                                self._fc, 1.0,
                                self._name,
                                self._ncons)
        snk.enable_autoscale(True)
        return snk

    def get_vecsource(self):
        return blocks.vector_source_c([])

    def get_npts(self):
        self._npts = self.snk.fft_size()
        return self._npts

    def set_line_label(self, n, name):
        self.snk.set_line_label(n, self.knobnames[n])


class GrDataPlotterPsdF(GrDataPlotParent):
    def __init__(self, name, rate, pmin=None, pmax=None):
        GrDataPlotParent.__init__(self, name, rate, pmin, pmax)

        self._datasize = gr.sizeof_float
        self._stripchart = True
        self._iscomplex = False

        self._npts = 2048
        self._wintype = filter.firdes.WIN_BLACKMAN_hARRIS
        self._fc = 0

        self._setup(1)

    def get_qtsink(self):
        snk = qtgui.freq_sink_f(self._npts, self._wintype,
                                self._fc, 1.0,
                                self._name,
                                self._ncons)
        snk.enable_autoscale(True)
        return snk

    def get_vecsource(self):
        return blocks.vector_source_f([])

    def get_npts(self):
        self._npts = self.snk.fft_size()
        return self._npts

    def set_line_label(self, n, name):
        self.snk.set_line_label(n, self.knobnames[n])


class GrTimeRasterF(GrDataPlotParent):
    def __init__(self, name, rate, pmin=None, pmax=None):
        GrDataPlotParent.__init__(self, name, rate, pmin, pmax)

        self._npts = 10
        self._rows = 40

        self._datasize = gr.sizeof_float
        self._stripchart = False
        self._iscomplex = False

        self._setup(1)

    def get_qtsink(self):
        snk = qtgui.time_raster_sink_f(1.0, self._npts, self._rows,
                                       [], [], self._name,
                                       self._ncons)
        return snk

    def get_vecsource(self):
        return blocks.vector_source_f([])

    def get_npts(self):
        self._npts = self.snk.num_cols()
        return self._npts

    def set_line_label(self, n, name):
        self.snk.set_line_label(n, self.knobnames[n])

class GrTimeRasterB(GrDataPlotParent):
    def __init__(self, name, rate, pmin=None, pmax=None):
        GrDataPlotParent.__init__(self, name, rate, pmin, pmax)

        self._npts = 10
        self._rows = 40

        self._datasize = gr.sizeof_char
        self._stripchart = False
        self._iscomplex = False

        self._setup(1)

    def get_qtsink(self):
        snk = qtgui.time_raster_sink_b(1.0, self._npts, self._rows,
                                       [], [], self._name,
                                       self._ncons)
        return snk

    def get_vecsource(self):
        return blocks.vector_source_b([])

    def get_npts(self):
        self._npts = self.snk.num_cols()
        return self._npts

    def set_line_label(self, n, name):
        self.snk.set_line_label(n, self.knobnames[n])


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
        items = []
        foundKeys = []
        deleteKeys = []
        numItems = self.treeWidget.topLevelItemCount()

        # The input knobs variable is a dict of stats to display in the tree.

        # Update tree stat values with new values found in knobs.
        # Track found keys and track keys in tree that are not in input knobs.
        for i in range(0, numItems):
            item = self.treeWidget.topLevelItem(i)

            # itemKey is the text in the first column of a QTreeWidgetItem
            itemKey = str(item.text(0))
            if itemKey in knobs.keys():

                # This key was found in the tree, update its values.
                foundKeys.append(itemKey)
                v = knobs[itemKey].value
                units = str(knobprops[itemKey].units)
                descr = str(knobprops[itemKey].description)

                if(type(v) == ControlPort.complex):
                    v = v.re + v.im*1j
                # If it's a byte stream, Python thinks it's a string.
                # Unpack and convert to floats for plotting.
                # Ignore the edge list knob if it's being exported
                elif(type(v) == str and itemKey.find('probe2_b') == 0):
                    v = struct.unpack(len(v)*'b', v)

                # Convert the final value to a string for displaying
                v = str(v)

                if (item.text(1) != v or
                    item.text(2) != units or
                    item.text(3) != descr):

                    item.setText(1, v)
                    item.setText(2, units)
                    item.setText(3, descr)
            else:
                # This item is not in the knobs list...track it for removal.
                deleteKeys.append(itemKey)

        # Add items to tree that are not currently in the tree.
        for k in knobs.keys():
            if k not in foundKeys:
                v = knobs[k].value
                if(type(v) == ControlPort.complex):
                    v = v.re + v.im*1j
                # If it's a byte stream, Python thinks it's a string.
                # Unpack and convert to floats for plotting.
                # Ignore the edge list knob if it's being exported
                elif(type(v) == str and k.find('probe2_b') == 0):
                    v = struct.unpack(len(v)*'b', v)

                item = QtGui.QTreeWidgetItem([k, str(v),
                            knobprops[k].units, knobprops[k].description])
                self.treeWidget.addTopLevelItem(item)

        # Remove items currently in tree that are not in the knob list.
        for itemKey in deleteKeys:
            qtwiList = self.treeWidget.findItems(itemKey, Qt.Qt.MatchFixedString)
            if (len(qtwiList) > 1):
                raise Exception('More than one item with key %s in tree' %
                                itemKey)
            elif (len(qtwiList) == 1):
                i = self.treeWidget.indexOfTopLevelItem(qtwiList[0])
                self.treeWidget.takeTopLevelItem(i)
