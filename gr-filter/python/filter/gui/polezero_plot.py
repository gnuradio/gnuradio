# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import sys
from PyQt5 import Qt, QtCore
from math import sin, cos, pi
import pyqtgraph as pg
from numpy import zeros
from numpy import float64 as Float
from numpy import vectorize
from numpy import delete


class PzPlot(pg.PlotWidget):

    def __init__(self, *args):
        pg.PlotWidget.__init__(self, *args)

        # Set Global pyqtgraph options
        # Default foreground color for text, lines, axes, etc.
        pg.setConfigOption('foreground', 'k')
        # Default background for GraphicsView.
        pg.setConfigOption('background', None)
        # Draw lines with smooth edges at the cost of reduced performance.
        pg.setConfigOptions(antialias=True)

        self.ymax = 0
        self.xmax = 0
        self.ymin = 0
        self.xmin = 0
        self.setBackground(Qt.QBrush(Qt.Qt.darkCyan))

        self.setRange(xRange=[-3, 3], yRange=[-2, 2])

        axis = self.getAxis('bottom')
        axis.setStyle(tickLength=-10)
        axis.setPen(Qt.QPen(Qt.Qt.white, 1.025, Qt.Qt.DotLine))

        axis = self.getAxis('left')
        axis.setStyle(tickLength=-10)
        axis.setPen(Qt.QPen(Qt.Qt.white, 1.025, Qt.Qt.DotLine))

        self.plotItem.showGrid(x=True, y=True, alpha=100)

        self.drawUnitcircle()

    def drawUnitcircle(self):
        radius = 1.0
        steps = 1024

        angleStep = 2 * pi / steps
        x = [sin(a * angleStep) * radius for a in range(0, steps)]
        y = [cos(a * angleStep) * radius for a in range(0, steps)]

        curve = self.plot(title="Unit Circle")
        curve.setPen(Qt.QPen(Qt.Qt.gray, 0.025, Qt.Qt.DotLine))
        curve.setData(x, y)

    def insertZeros(self, roots):
        self.removeallCurves()
        if len(roots):
            self.__insertZero(Qt.Qt.blue, roots.real, roots.imag)
            self.ymax = max(roots.imag)
            self.ymin = min(roots.imag)
            self.xmax = max(roots.real)
            self.xmin = min(roots.real)
            # To make the plot look good.
            if self.xmax <= 1.3:
                self.xmax = 2
            if self.xmin >= -1.3:
                self.xmin = -2
            if self.ymax <= 1:
                self.ymax = 1.5
            if self.ymin >= -1:
                self.ymin = -1.5

            self.setRange(xRange=[self.xmin, self.xmax],
                          yRange=[self.ymin, self.ymax])

    def insertPoles(self, roots):
        if len(roots):
            self.__insertPole(Qt.Qt.black, roots.real, roots.imag)
            ymax = max(roots.imag)
            ymax = max(ymax, self.ymax)
            ymin = min(roots.imag)
            ymin = min(ymin, self.ymin)
            xmax = max(roots.real)
            xmax = max(xmax, self.xmax)
            xmin = min(roots.real)
            xmin = min(xmin, self.xmin)

            # To make the plot look good.
            if xmax <= 1.3:
                xmax = 2
            else:
                xmax = 1.2 * xmax

            if xmin >= -1.3:
                xmin = -2
            else:
                xmin = 1.2 * xmin

            if ymax <= 1:
                ymax = 1.5
            else:
                ymax = 1.2 * ymax

            if ymin >= -1:
                ymin = -1.5
            else:
                ymin = 1.2 * ymin

            self.setRange(xRange=[xmin, xmax], yRange=[ymin, ymax])
            self.drawUnitcircle()
            self.replot()

    def __insertZero(self, color, px, py):
        curve = self.plot(name="Zero")
        curve.setPen(None)
        curve.setSymbol('o')
        curve.setSymbolPen('b')
        curve.setSymbolBrush(Qt.QBrush(Qt.Qt.gray))
        curve.setSymbolSize(10)
        curve.setData(px, py)

    def __insertPole(self, color, px, py):
        curve = self.plot(name="Pole")
        curve.setPen(None)
        curve.setSymbol('x')
        curve.setSymbolPen('b')
        curve.setSymbolBrush(Qt.QBrush(Qt.Qt.gray))
        curve.setSymbolSize(10)
        curve.setData(px, py)

    def removeallCurves(self):
        # TODO for curve in self.itemList():
        # if isinstance(curve, Qwt.QwtPlotCurve):
        # curve.detach()
        self.replot()


class CanvasPicker(Qt.QObject):
    curveChanged = QtCore.pyqtSignal(tuple)
    mouseposChanged = QtCore.pyqtSignal(tuple)

    def __init__(self, plot):
        Qt.QObject.__init__(self, plot)
        self.__selectedCurve = None
        self.__selectedPoint = -1
        self.__selectedcPoint = -1
        self.__addedZero = -1
        self.__addedcZero = -1
        self.changeConjugate = False
        self.enableZeroadd = False
        self.enablePoleadd = False
        self.enablepzDelete = False
        self.iir = False
        self.__plot = plot

        ''' TODO
        canvas = plot.canvas()
        canvas.installEventFilter(self)

        # We want the focus, but no focus rect.
        # The selected point will be highlighted instead.
        canvas.setFocusPolicy(Qt.Qt.StrongFocus)
        canvas.setCursor(Qt.Qt.PointingHandCursor)
        canvas.setFocusIndicator(Qwt.QwtPlotCanvas.ItemFocusIndicator)
        canvas.setFocus()

        canvas.setWhatsThis(
            'All points can be moved using the left mouse button '
            'or with these keys:\n\n'
            '- Up: Select next curve\n'
            '- Down: Select previous curve\n'
            '- Left, "-": Select next point\n'
            '- Right, "+": Select previous point\n'
            '- 7, 8, 9, 4, 6, 1, 2, 3: Move selected point'
            )

        self.__shiftCurveCursor(True)
        '''

    def event(self, event):
        if event.type() == Qt.QEvent.User:
            self.__showCursor(True)
            return True
        try:
            return Qt.QObject.event(event)
        except TypeError:
            return False

    def set_conjugate(self):
        self.changeConjugate = not(self.changeConjugate)

    def set_iir(self, val=True):
        self.iir = val

    def add_zero(self):
        self.enableZeroadd = not(self.enableZeroadd)

    def add_pole(self):
        # Adding pole support only for IIR.
        if self.iir:
            self.enablePoleadd = not(self.enablePoleadd)

    def delete_pz(self):
        self.enablepzDelete = not(self.enablepzDelete)

    def eventFilter(self, object, event):

        if event.type() == Qt.QEvent.FocusIn:
            self.__showCursor(True)
        if event.type() == Qt.QEvent.FocusOut:
            self.__showCursor(False)

        if event.type() == Qt.QEvent.Paint:
            Qt.QApplication.postEvent(
                self, Qt.QEvent(Qt.QEvent.User))
        elif event.type() == Qt.QEvent.MouseButtonPress:
            if self.enableZeroadd or self.enablePoleadd:
                self.__drawAddedzero_pole(True, event.pos())
            else:
                self.__select(event.pos())
            return True
        elif event.type() == Qt.QEvent.MouseMove:
            curve = self.__selectedCurve
            if curve:
                tp = (self.__plot.invTransform(curve.xAxis(), event.pos().x()),
                      self.__plot.invTransform(curve.xAxis(), event.pos().y()))
                self.mouseposChanged.emit(tp)
            self.__move(event.pos())
            return True

        if event.type() == Qt.QEvent.KeyPress:
            delta = 5
            key = event.key()
            if key == Qt.Qt.Key_Up:
                self.__shiftCurveCursor(True)
                return True
            elif key == Qt.Qt.Key_Down:
                self.__shiftCurveCursor(False)
                return True
            elif key == Qt.Qt.Key_Right or key == Qt.Qt.Key_Plus:
                if self.__selectedCurve:
                    self.__shiftPointCursor(True)
                else:
                    self.__shiftCurveCursor(True)
                return True
            elif key == Qt.Qt.Key_Left or key == Qt.Qt.Key_Minus:
                if self.__selectedCurve:
                    self.__shiftPointCursor(False)
                else:
                    self.__shiftCurveCursor(True)
                return True

            if key == Qt.Qt.Key_1:
                self.__moveBy(-delta, delta)
            elif key == Qt.Qt.Key_2:
                self.__moveBy(0, delta)
            elif key == Qt.Qt.Key_3:
                self.__moveBy(delta, delta)
            elif key == Qt.Qt.Key_4:
                self.__moveBy(-delta, 0)
            elif key == Qt.Qt.Key_6:
                self.__moveBy(delta, 0)
            elif key == Qt.Qt.Key_7:
                self.__moveBy(-delta, -delta)
            elif key == Qt.Qt.Key_8:
                self.__moveBy(0, -delta)
            elif key == Qt.Qt.Key_9:
                self.__moveBy(delta, -delta)

        return Qwt.QwtPlot.eventFilter(self, object, event)

    def __select(self, pos):
        found, distance, point = None, 1e100, -1

        for curve in self.__plot.itemList():
            if isinstance(curve, Qwt.QwtPlotCurve):
                if curve.symbol().style() != Qwt.QwtSymbol.NoSymbol:
                    i, d = curve.closestPoint(pos)
                    if d < distance:
                        found = curve
                        point = i
                        distance = d

        self.__showCursor(False)
        self.__selectedCurve = None
        self.__selectedPoint = -1
        self.__selectedcPoint = -1

        if found and distance < 10:
            self.__selectedCurve = found
            self.__selectedPoint = point
            # Search for conjugate point if enabled.
            if self.changeConjugate:
                j = self.__searchConjugate(found.x(point), found.y(point))
                self.__selectedcPoint = j
            # Delete zero/pole if enabled.
            if self.enablepzDelete:
                self.__deleteZero()
            self.__showCursor(True)
        else:
            self.mouseposChanged.emit((None, None))

    def __deleteZero(self):
        curve = self.__selectedCurve
        xData = zeros(curve.dataSize(), Float)
        yData = zeros(curve.dataSize(), Float)

        for i in range(curve.dataSize()):
            xData[i] = curve.x(i)
            yData[i] = curve.y(i)

        if(self.__selectedPoint != -1):
            xData = delete(xData, self.__selectedPoint)
            yData = delete(yData, self.__selectedPoint)
        # One less to accommodate previous delete.
        if(self.__selectedcPoint != -1):
            xData = delete(xData, self.__selectedcPoint - 1)
            yData = delete(yData, self.__selectedcPoint - 1)

        curve.setData(xData, yData)
        self.__plot.replot()
        px = []
        py = []
        for c in self.__plot.itemList():
            if isinstance(c, Qwt.QwtPlotCurve):
                px.append([c.x(i) for i in range(c.dataSize())])
                py.append([c.y(i) for i in range(c.dataSize())])
        tp = (vectorize(complex)(px[0], py[0]),
              vectorize(complex)(px[1], py[1]))
        self.curveChanged.emit(tp)

    def __moveBy(self, dx, dy):
        if dx == 0 and dy == 0:
            return

        curve = self.__selectedCurve
        if not curve:
            return

        x = self.__plot.transform(
            curve.xAxis(), curve.x(self.__selectedPoint)) + dx
        y = self.__plot.transform(
            curve.yAxis(), curve.y(self.__selectedPoint)) + dy
        self.__move(Qt.QPoint(x, y))

    def __move(self, pos):
        curve = self.__selectedCurve
        if not curve:
            return

        xData = zeros(curve.dataSize(), Float)
        yData = zeros(curve.dataSize(), Float)

        # Poles made unmovable when FIR design is active.
        move_enable = True
        if not self.iir:
            if self.__selectedCurve.symbol().style() == Qwt.QwtSymbol.XCross:
                move_enable = False
        if move_enable:
            for i in range(curve.dataSize()):
                if i == self.__selectedPoint:
                    xData[i] = self.__plot.invTransform(curve.xAxis(), pos.x())
                    yData[i] = self.__plot.invTransform(curve.yAxis(), pos.y())
                    if(self.__selectedcPoint != -1):
                        xData[self.__selectedcPoint] = xData[i]
                        yData[self.__selectedcPoint] = -yData[i]
                elif i != self.__selectedcPoint:
                    xData[i] = curve.x(i)
                    yData[i] = curve.y(i)
            curve.setData(xData, yData)
            self.__plot.replot()
            px = []
            py = []
            for c in self.__plot.itemList():
                if isinstance(c, Qwt.QwtPlotCurve):
                    px.append([c.x(i) for i in range(c.dataSize())])
                    py.append([c.y(i) for i in range(c.dataSize())])
            tp = (vectorize(complex)(px[0], py[0]),
                  vectorize(complex)(px[1], py[1]))
            self.curveChanged.emit(tp)
            self.__showCursor(True)

    def __searchConjugate(self, x, y):
        curve = self.__selectedCurve
        for i in range(curve.dataSize()):
            if (round(curve.x(i), 8) == round(x, 8) and round(curve.y(i), 8) == -round(y, 8)):
                if (y != 0):
                    return i
        return -1

    def __drawAddedzero_pole(self, showIt, pos):
        editcurve = ''
        for curve in self.__plot.itemList():
            if isinstance(curve, Qwt.QwtPlotCurve):
                if self.enableZeroadd:
                    if curve.symbol().style() == Qwt.QwtSymbol.Ellipse:
                        editcurve = curve
                if self.enablePoleadd:
                    if curve.symbol().style() == Qwt.QwtSymbol.XCross:
                        editcurve = curve
        if not editcurve:
            return

        if self.changeConjugate:
            extrapoints = 2
        else:
            extrapoints = 1

        xData = zeros(editcurve.dataSize() + extrapoints, Float)
        yData = zeros(editcurve.dataSize() + extrapoints, Float)

        for i in range(editcurve.dataSize()):
            xData[i] = editcurve.x(i)
            yData[i] = editcurve.y(i)
        xData[i + 1] = self.__plot.invTransform(editcurve.xAxis(), pos.x())
        yData[i + 1] = self.__plot.invTransform(editcurve.yAxis(), pos.y())

        if self.changeConjugate:
            xData[i + 2] = xData[i + 1]
            yData[i + 2] = -yData[i + 1]
            self.__addedcZero = i + 2

        editcurve.setData(xData, yData)

        self.__addedZero = i + 1
        symbol = Qwt.QwtSymbol(editcurve.symbol())
        newSymbol = Qwt.QwtSymbol(symbol)
        newSymbol.setPen(Qt.QPen(Qt.Qt.red))
        doReplot = self.__plot.autoReplot()

        self.__plot.setAutoReplot(False)
        editcurve.setSymbol(newSymbol)

        editcurve.draw(self.__addedZero, self.__addedZero)
        if self.changeConjugate:
            editcurve.draw(self.__addedcZero, self.__addedcZero)

        editcurve.setSymbol(symbol)
        self.__plot.setAutoReplot(doReplot)
        px = []
        py = []
        for c in self.__plot.itemList():
            if isinstance(c, Qwt.QwtPlotCurve):
                px.append([c.x(i) for i in range(c.dataSize())])
                py.append([c.y(i) for i in range(c.dataSize())])
        tp = (vectorize(complex)(px[0], py[0]),
              vectorize(complex)(px[1], py[1]))
        self.curveChanged.emit(tp)
        self.__showCursor(True)

    def __showCursor(self, showIt):
        curve = self.__selectedCurve
        if not curve:
            return

        # Use copy constructors to defeat the reference semantics.
        symbol = Qwt.QwtSymbol(curve.symbol())
        newSymbol = Qwt.QwtSymbol(symbol)
        if showIt:
            newSymbol.setBrush(symbol.brush().color().dark(150))

        doReplot = self.__plot.autoReplot()

        self.__plot.setAutoReplot(False)
        curve.setSymbol(newSymbol)

        curve.draw(self.__selectedPoint, self.__selectedPoint)
        if (self.__selectedcPoint != -1):
            curve.draw(self.__selectedcPoint, self.__selectedcPoint)

        curve.setSymbol(symbol)
        self.__plot.setAutoReplot(doReplot)

    def __shiftCurveCursor(self, up):
        curves = [curve for curve in self.__plot.itemList()
                  if isinstance(curve, Qwt.QwtPlotCurve)]

        if not curves:
            return

        if self.__selectedCurve in curves:
            index = curves.index(self.__selectedCurve)
            if up:
                index += 1
            else:
                index -= 1
            # Keep index within [0, len(curves)).
            index += len(curves)
            index %= len(curves)
        else:
            index = 0

        self.__showCursor(False)
        self.__selectedPoint = 0
        self.__selectedCurve = curves[index]
        self.__showCursor(True)

    def __shiftPointCursor(self, up):
        curve = self.__selectedCurve
        if not curve:
            return

        if up:
            index = self.__selectedPoint + 1
        else:
            index = self.__selectedPoint - 1
        # Keep index within [0, curve.dataSize()).
        index += curve.dataSize()
        index %= curve.dataSize()
        if index != self.__selectedPoint:
            self.__showCursor(False)
            self.__selectedPoint = index
            self.__showCursor(True)
