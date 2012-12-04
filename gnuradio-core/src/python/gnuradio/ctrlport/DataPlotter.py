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

import sys

try:
    from PyQt4.QtCore import Qt;
    from PyQt4 import QtGui, QtCore
except ImportError:
    print "Error: Program requires PyQt4."
    sys.exit(1)

try:
    import PyQt4.Qwt5 as Qwt
    from PyQt4.Qwt5.anynumpy import *
except ImportError:
    print "Error: Program requires PyQwt5."
    sys.exit(1)


class Zoomer(Qwt.QwtPlotZoomer):
    def __init__(self, a,b,c,d,e):
        Qwt.QwtPlotZoomer.__init__(self,a,b,c,d,e);
        self.zoomers = [];

    def zoom(self, r):
        Qwt.QwtPlotZoomer.zoom(self,r);
        if(r == 0):
            #self.plot().setAxisAutoScale(True);
            self.plot().setAxisAutoScale(Qwt.QwtPlot.xBottom)
            self.plot().setAxisAutoScale(Qwt.QwtPlot.yLeft)
            self.plot().replot();


class DataPlotterBase(Qwt.QwtPlot):
    DefaultColors = ( Qt.green, Qt.red, Qt.blue,
              Qt.cyan, Qt.magenta, Qt.black, Qt.darkRed, 
	      Qt.darkGray, Qt.darkGreen, Qt.darkBlue, Qt.yellow)

    dropSignal = QtCore.pyqtSignal(QtCore.QEvent)
 
    def contextMenuEvent(self,e):
        menu = QtGui.QMenu(self);
        menu.addAction(self.gridAct);
        menu.addAction(self.axesAct);
        menu.addAction(self.curvAct);
        menu.exec_(e.globalPos());

    def dragEnterEvent(self,e):
        e.accept();

    def dropEvent(self, e):
        Qwt.QwtPlot.dropEvent(self,e)
        self.dropSignal.emit( e );



    def __init__(self, parent, title, xlabel, ylabel, size, x, y):
        Qwt.QwtPlot.__init__(self, parent)
        self.callback = None;

        self.gridAct = QtGui.QAction("Toggle &Grid", self, triggered=self.toggleGrid);
        self.axesAct = QtGui.QAction("Toggle &Axes", self, triggered=self.toggleAxes);
        self.curvAct = QtGui.QAction("Toggle &Lines", self, triggered=self.toggleCurve);

        # Set up the zoomer   
        self.zoomer = Zoomer(Qwt.QwtPlot.xBottom,
                                        Qwt.QwtPlot.yLeft,
                                        Qwt.QwtPicker.DragSelection,
                                        Qwt.QwtPicker.AlwaysOff,
                                        self.canvas())

        # Crosshairs + Data labels
        self.picker = Qwt.QwtPlotPicker(
            Qwt.QwtPlot.xBottom,
            Qwt.QwtPlot.yLeft,
            Qwt.QwtPicker.PointSelection | Qwt.QwtPicker.DragSelection,
            Qwt.QwtPlotPicker.CrossRubberBand,
            Qwt.QwtPicker.AlwaysOn,
            self.canvas())
        self.picker.setRubberBandPen(QtGui.QPen(QtCore.Qt.white, 1, QtCore.Qt.DashLine))
        self.picker.setTrackerPen(QtGui.QPen(QtCore.Qt.white))

        self.axisEnable = False;
        # Turn off bloated data labels
        self.enableAxis(Qwt.QwtPlot.yLeft, False);
        self.enableAxis(Qwt.QwtPlot.xBottom, False);

        # Allow panning with middle mouse
        panner = Qwt.QwtPlotPanner(self.canvas())
        panner.setAxisEnabled(Qwt.QwtPlot.yRight, False)
        panner.setMouseButton(Qt.MidButton)

        # Accept dropping of stats
        self.setAcceptDrops(True);
        self.grid = None
        self.curve_en = False
        self.setCanvasBackground(Qt.black)

        self.insertLegend(Qwt.QwtLegend(), Qwt.QwtPlot.TopLegend);

        self.axisEnabled(True);

        self.resize(size, size)
        self.setAutoReplot(False)
        self.show()
        self.updateTimerInt = 500
        self.startTimer(self.updateTimerInt)
    
        # Set Axis on and Grid off by default
        #self.toggleGrid();
        self.toggleAxes();


    def toggleAxes(self):
        self.axisEnable = not self.axisEnable;
        self.enableAxis(Qwt.QwtPlot.yLeft, self.axisEnable);
        self.enableAxis(Qwt.QwtPlot.xBottom, self.axisEnable);


    def toggleGrid(self):
        # grid
        if self.grid == None:
            self.grid = Qwt.QwtPlotGrid()
            self.grid.enableXMin(True)
            self.grid.setMajPen(QtGui.QPen(Qt.gray, 0, Qt.DotLine))
            self.grid.setMinPen(QtGui.QPen(Qt.gray, 0 , Qt.DotLine))
            self.grid.attach(self)
        else:
            self.grid.detach()
            self.grid = None

    	return self

    def toggleCurve(self):
        self.curve_en = not self.curve_en;
        

class DataPlotterVector(DataPlotterBase):
    def __init__(self, parent, legend, title, xlabel, ylabel, size, x, y):
        DataPlotterBase.__init__(self, parent, title, xlabel, ylabel, size, x, y)
        self.curve = Qwt.QwtPlotCurve(legend)
        self.curve.attach(self)
        self.tag = None;
        self.x = self.y = [0.0];

    def offerData(self, data, tag):
        if(tag == self.tag):
            self.x = data[::2]; self.y = data[1::2]

    def timerEvent(self, e):
        if(self.curve_en):
            self.curve.setStyle(Qwt.QwtPlotCurve.Lines)
            self.curve.setPen(QtGui.QPen(Qt.green))
        else:
            self.curve.setStyle(Qwt.QwtPlotCurve.NoCurve)

        self.curve.setData(self.x, self.y)
        self.replot()

    def enableLegend(self):
        self.insertLegend(Qwt.QwtLegend(), Qwt.QwtPlot.BottomLegend);
        return self

    def setSeries(self,tag,name):
        self.tag = tag;
        self.curve.setTitle(name)
    

class DataPlotterVectorOne(DataPlotterVector):
    def __init__(self, parent, legend, title, xlabel, ylabel, size, x, y):
        DataPlotterVector.__init__(self, parent, legend, title, xlabel, ylabel, size, x, y)
        self.curve.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.XCross,
        	QtGui.QBrush(), QtGui.QPen(Qt.green), QtCore.QSize(2, 2)))
        self.setAxisAutoScale(True)
        self.axisSet = False;

        # Lines on by default
        self.toggleCurve();

    def offerData(self, data, tag):
        if(tag == self.tag):
            if not self.axisSet:
                self.setAxisScale(1, 0, len(data));
                self.axisSet = True;
            self.x = range(0,len(data));
            self.y = data;
    

class DataPlotterConst(DataPlotterVector):
    def __init__(self, parent, legend, title, xlabel, ylabel, size, x, y):
        DataPlotterVector.__init__(self, parent, legend, title, xlabel, ylabel, size, x, y)
        self.x = arange(-2, 100.1, 2)
        self.y = zeros(len(self.x), Float)
        self.curve.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.XCross,
        	QtGui.QBrush(), QtGui.QPen(Qt.green), QtCore.QSize(2, 2)))
        self.curve.setStyle(Qwt.QwtPlotCurve.NoCurve)
        self.setAxisAutoScale(False)

class DataPlotterEqTaps(DataPlotterVector):
    def __init__(self, parent, legend, title, xlabel, ylabel, size, x, y, qtcolor):
	DataPlotterVector.__init__(self, parent, legend, title, xlabel, ylabel, size, x, y)
        self.x = arange(-.5, .5, 1)
        self.y = zeros(len(self.x), Float)
        self.curve.setPen(QtGui.QPen(qtcolor))

class DataPlotterTicker(DataPlotterBase):
    def __init__(self, parent, title, xlabel, ylabel, size, x, y, seconds = 60):
    	DataPlotterBase.__init__(self, parent, title, xlabel, ylabel, size, x, y)
    	self.series = {}
    	self.setTimeScale(seconds)

# AAAAAAAAAA - enable for legend at bottom
#        self.insertLegend(Qwt.QwtLegend(), Qwt.QwtPlot.BottomLegend);
    	self.skipEvents=1

    def setTimeScale(self, seconds):
	intv =  float(self.updateTimerInt) / 1000
        self.x = arange(0, seconds, intv)
        #self.x = arange(0, seconds, 1)
	return self

    def addSeries(self, tag, label, qtcolor = None, alpha = 1):
    	class Series:
    		def __init__(self, tag, label, qtcolor, x, plot):
    			self.vec =  zeros(len(x), Float)
    			self.value = None
    			self.alpha = alpha
    			self.curve = Qwt.QwtPlotCurve(label)
    			self.curve.setPen(QtGui.QPen(qtcolor))
    			self.plot = plot
    
    	if qtcolor == None: qtcolor = self.DefaultColors[len(self.series)]
    	self.series[tag] = s = Series(tag, label, qtcolor, self.x, self)
    	self.enableSeries(tag)
    	return self

    def enableSeries(self, tag):
    	if self.hasSeries(tag):
    		s = self.series[tag]
    		s.enabled = True
            	s.curve.attach(s.plot)
    	return self

    def disableSeries(self, tag):
    	if self.hasSeries(tag):
    		s = self.series[tag]
    		s.enabled = False 
            	s.curve.detach()
    	return self

    def toggleSeries(self,tag):
    	if self.seriesIsEnabled(tag):
    		self.disableSeries(tag)
    	else:
    		self.enableSeries(tag)
    	return self

    def timerEvent(self, e):
        for k, v in self.series.iteritems():
            if v.value == None: continue
            elif v.vec[0] == 0: v.vec =  ones(len(v.vec), Float)*v.value
	
            prev = v.vec[0]
            v.vec = concatenate((v.vec[:1], v.vec[:-1]), 1)
            v.vec[0] = v.value*v.alpha + prev*(1-v.alpha)
            self.series[k] = v 
            v.curve.setData(self.x, v.vec)
            self.replot()

    def offerData(self, value, tag):
        if(self.series.has_key(tag)):
            if not value == NaN:
            	self.series[tag].value = value
                #print "Data Offer %s items:"%(tag)
        return self

    def hasSeries(self, tag):
	       return self.series.has_key(tag);

    def seriesIsEnabled(self, tag):
    	if self.hasSeries(tag): return self.series[tag].enabled 
    	else: return False

class DataPlotterTickerWithSeriesButtons(DataPlotterTicker):
    def __init__(self, parent, title, xlabel, ylabel, size, x, y, seconds = 60):
        DataPlotterTicker.__init__(self, parent, title, xlabel, ylabel, size, x, y, seconds)
        DataPlotterTicker.setAcceptDrops(self,True);
    	self.buttonx = 50; self.buttony = 20; self.buttonSize = 16 
    	self.btns = [] 




    def addSeriesWithButton(self, tag, legend, qtcolor=None, alpha = 1):
    	self.addSeries(tag, legend, qtcolor, alpha)
    	lenbtns = len(self.btns)
    
        btn = QtGui.QToolButton(self)
    	btn.rank = lenbtns
        btn.setText(str(btn.rank))
    	btn.tag = tag
        #btn.setIcon(Qt.QIcon(Qt.QPixmap(print_xpm)))
        #btn.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)
    	#btn.setForegroundColor(Qt.red)
    	self.btns.append(btn)
    	btn.setGeometry(self.buttonx, self.buttony, self.buttonSize, self.buttonSize)
    	self.buttonx += self.buttonSize
    
    	if lenbtns == 0: callback = self.print_0
    	if lenbtns == 1: callback = self.print_1
    	if lenbtns == 2: callback = self.print_2
    	if lenbtns == 3: callback = self.print_3
    	if lenbtns == 4: callback = self.print_4
    	if lenbtns == 5: callback = self.print_5
    	if lenbtns == 6: callback = self.print_6
    	if lenbtns == 7: callback = self.print_7
        self.connect(btn, QtCore.SIGNAL('clicked()'), callback)
    	return self

    def toggleSeriesWithButton(self,btn):
    	DataPlotterTicker.toggleSeries(self, btn.tag)
    
    	if self.seriesIsEnabled(btn.tag):
    #		btn.setForegroundRole(Qt.QPalette.NoRole)
    		btn.setText(str(btn.rank))
    	else:
    		btn.setText('x')
    #		btn.setForegroundRole(Qt.QPalette.Light)
    
    def print_0(self): self.toggleSeriesWithButton(self.btns[0])
    def print_1(self): self.toggleSeriesWithButton(self.btns[1])
    def print_2(self): self.toggleSeriesWithButton(self.btns[2])
    def print_3(self): self.toggleSeriesWithButton(self.btns[3])
    def print_4(self): self.toggleSeriesWithButton(self.btns[4])
    def print_5(self): self.toggleSeriesWithButton(self.btns[5])
    def print_6(self): self.toggleSeriesWithButton(self.btns[6])
    def print_7(self): self.toggleSeriesWithButton(self.btns[7])

class DataPlotterValueTable:
    def __init__(self, parent, x, y, xsize, ysize, headers=['Statistic Key ( Source Block :: Stat Name )  ', 'Curent Value', 'Units', 'Description']):
	# must encapsulate, cuz Qt's bases are not classes
        self.treeWidget = QtGui.QTreeWidget(parent)
        self.treeWidget.setColumnCount(len(headers))
        self.treeWidget.setGeometry(x,y,xsize,ysize)
        self.treeWidget.setHeaderLabels(headers)
        self.treeWidget.resizeColumnToContents(0)

    def updateItems(self, knobs, knobprops):
        # save previous selection if exists
        sel = self.treeWidget.currentItem();
        row = self.treeWidget.indexOfTopLevelItem(sel);
        items = [];
        self.treeWidget.clear()
        for k, v in knobs.iteritems():
            items.append(QtGui.QTreeWidgetItem([str(k), str(v.value), knobprops[k].units, knobprops[k].description]))
        self.treeWidget.insertTopLevelItems(0, items)
        # re-set previous selection if exists
        if(row != -1):
            try:
                self.treeWidget.setCurrentItem(self.treeWidget.topLevelItem(row));
            except:
                pass
