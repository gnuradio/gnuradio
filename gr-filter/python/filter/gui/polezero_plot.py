# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import sys
from PyQt6.QtCore import QEvent, QObject, QPoint, Qt, Signal
from PyQt6.QtGui import QBrush, QPen
from PyQt6.QtWidgets import QApplication
from math import sin, cos, pi
import pyqtgraph as pg
from numpy import zeros
from numpy import float64 as Float
from numpy import vectorize
from numpy import delete


class PzPlot(pg.PlotWidget):

    def __init__(self, *args, **kwargs):
        pg.PlotWidget.__init__(self, *args, **kwargs)

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
        self.setBackground(QBrush(Qt.darkCyan))

        self.setRange(xRange=[-3, 3], yRange=[-2, 2])

        axis = self.getAxis('bottom')
        axis.setStyle(tickLength=-10)
        axis.setPen(QPen(Qt.white, 1.025, Qt.DotLine))

        axis = self.getAxis('left')
        axis.setStyle(tickLength=-10)
        axis.setPen(QPen(Qt.white, 1.025, Qt.DotLine))

        self.plotItem.showGrid(x=True, y=True, alpha=100)

        self.drawUnitcircle()

    def drawUnitcircle(self):
        radius = 1.0
        steps = 1024

        angleStep = 2 * pi / steps
        x = [sin(a * angleStep) * radius for a in range(0, steps)]
        y = [cos(a * angleStep) * radius for a in range(0, steps)]

        curve = self.plot(title="Unit Circle")
        curve.setPen(QPen(Qt.gray, 0.025, Qt.DotLine))
        curve.setData(x, y)

    def insertZeros(self, roots):
        self.removeallCurves()
        if len(roots):
            self.__insertZero(Qt.blue, roots.real, roots.imag)
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
            self.__insertPole(Qt.black, roots.real, roots.imag)
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
        curve.setSymbolBrush(QBrush(Qt.gray))
        curve.setSymbolSize(10)
        curve.setData(px, py)

    def __insertPole(self, color, px, py):
        curve = self.plot(name="Pole")
        curve.setPen(None)
        curve.setSymbol('x')
        curve.setSymbolPen('b')
        curve.setSymbolBrush(QBrush(Qt.gray))
        curve.setSymbolSize(10)
        curve.setData(px, py)

    def removeallCurves(self):
        # TODO for curve in self.itemList():
        # if isinstance(curve, Qwt.QwtPlotCurve):
        # curve.detach()
        self.replot()


class CanvasPicker(QObject):
    curveChanged = Signal(tuple)
    mouseposChanged = Signal(tuple)

    def __init__(self, plot):
        QObject.__init__(self, plot)
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
        canvas.setFocusPolicy(Qt.StrongFocus)
        canvas.setCursor(Qt.PointingHandCursor)
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
        if event.type() == QEvent.User:
            self.__showCursor(True)
            return True
        try:
            return QObject.event(event)
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
        self.__move(QPoint(x, y))

    def __searchConjugate(self, x, y):
        curve = self.__selectedCurve
        for i in range(curve.dataSize()):
            if (round(curve.x(i), 8) == round(x, 8) and round(curve.y(i), 8) == -round(y, 8)):
                if (y != 0):
                    return i
        return -1

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
