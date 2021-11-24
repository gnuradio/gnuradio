# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from PyQt5 import QtGui, QtCore, Qt
import pyqtgraph as pg
import numpy


class IdealBandItems(object):
    def __init__(self):
        # Set Global pyqtgraph options
        # Default foreground color for text, lines, axes, etc.
        pg.setConfigOption('foreground', 'k')
        # Default background for GraphicsView.
        pg.setConfigOption('background', None)
        # Draw lines with smooth edges at the cost of reduced performance.
        pg.setConfigOptions(antialias=True)

        self.win = pg.GraphicsWindow()
        self.plot = self.win.addPlot()
        self.idealbandhcurves = [self.plot.plot() for i in range(4)]
        self.idealbandvcurves = [self.plot.plot() for i in range(4)]
        self.params = ""

    def setLinetype(self):
        for c in self.idealbandhcurves:
            c.setPen(Qt.QPen(Qt.Qt.red, 1, Qt.Qt.DotLine))
        for c in self.idealbandvcurves:
            c.setPen(Qt.QPen(Qt.Qt.red, 1, Qt.Qt.DotLine))

    def plotIdealCurves(self, ftype, params, plot):
        self.params = params
        try:
            if (ftype == "Low Pass"):
                self.detach_unwantedcurves(plot)
                x = [0, self.params["pbend"]]
                y = [20.0 * numpy.log10(self.params["gain"])] * 2
                self.idealbandhcurves[0].setData(x, y)

                x = [self.params["pbend"]] * 2
                y = [20.0 * numpy.log10(self.params["gain"]),
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[0].setData(x, y)

                x = [self.params["sbstart"], self.params["fs"] / 2.0]
                y = [-self.params["atten"]] * 2
                self.idealbandhcurves[1].setData(x, y)

                x = [self.params["sbstart"]] * 2
                y = [-self.params["atten"],
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[1].setData(x, y)

            elif ftype == "High Pass":
                self.detach_unwantedcurves(plot)
                x = [self.params["pbstart"], self.params["fs"] / 2.0]
                y = [20.0 * numpy.log10(self.params["gain"])] * 2
                self.idealbandhcurves[0].setData(x, y)

                x = [self.params["pbstart"]] * 2
                y = [20.0 * numpy.log10(self.params["gain"]),
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[0].setData(x, y)

                x = [0, self.params["sbend"]]
                y = [-self.params["atten"]] * 2
                self.idealbandhcurves[1].setData(x, y)

                x = [self.params["sbend"]] * 2
                y = [-self.params["atten"],
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[1].setData(x, y)

            elif ftype == "Band Notch":
                x = [self.params["sbstart"], self.params["sbend"]]
                y = [-self.params["atten"]] * 2
                self.idealbandhcurves[0].setData(x, y)

                x = [self.params["sbstart"]] * 2
                y = [-self.params["atten"],
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[0].setData(x, y)

                x = [self.params["sbend"]] * 2
                y = [-self.params["atten"],
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[1].setData(x, y)

                x = [0, self.params["sbstart"] - self.params["tb"]]
                y = [20.0 * numpy.log10(self.params["gain"])] * 2
                self.idealbandhcurves[1].setData(x, y)

                x = [self.params["sbstart"] - self.params["tb"]] * 2
                y = [20.0 * numpy.log10(self.params["gain"]),
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[2].setData(x, y)

                x = [self.params["sbend"] + self.params["tb"],
                     self.params["fs"] / 2.0]
                y = [20.0 * numpy.log10(self.params["gain"])] * 2
                self.idealbandhcurves[2].setData(x, y)

                x = [self.params["sbend"] + self.params["tb"]] * 2
                y = [20.0 * numpy.log10(self.params["gain"]),
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[3].setData(x, y)

            elif ftype == "Band Pass":
                x = [self.params["pbstart"], self.params["pbend"]]
                y = [20.0 * numpy.log10(self.params["gain"])] * 2
                self.idealbandhcurves[0].setData(x, y)

                x = [self.params["pbstart"]] * 2
                y = [20.0 * numpy.log10(self.params["gain"]),
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[0].setData(x, y)

                x = [self.params["pbend"]] * 2
                y = [20.0 * numpy.log10(self.params["gain"]),
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[1].setData(x, y)

                x = [0, self.params["pbstart"] - self.params["tb"]]
                y = [-self.params["atten"]] * 2
                self.idealbandhcurves[1].setData(x, y)

                x = [self.params["pbstart"] - self.params["tb"]] * 2
                y = [-self.params["atten"],
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[2].setData(x, y)

                x = [self.params["pbend"] + self.params["tb"],
                     self.params["fs"] / 2.0]
                y = [-self.params["atten"]] * 2
                self.idealbandhcurves[2].setData(x, y)

                x = [self.params["pbend"] + self.params["tb"]] * 2
                y = [-self.params["atten"],
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[3].setData(x, y)

            elif ftype == "Complex Band Pass":
                x = [self.params["pbstart"], self.params["pbend"]]
                y = [20.0 * numpy.log10(self.params["gain"])] * 2
                self.idealbandhcurves[0].setData(x, y)

                x = [self.params["pbstart"]] * 2
                y = [20.0 * numpy.log10(self.params["gain"]),
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[0].setData(x, y)

                x = [self.params["pbend"]] * 2
                y = [20.0 * numpy.log10(self.params["gain"]),
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[1].setData(x, y)

                x = [0, self.params["pbstart"] - self.params["tb"]]
                y = [-self.params["atten"]] * 2
                self.idealbandhcurves[1].setData(x, y)

                x = [self.params["pbstart"] - self.params["tb"]] * 2
                y = [-self.params["atten"],
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[2].setData(x, y)

                x = [self.params["pbend"] + self.params["tb"],
                     self.params["fs"] / 2.0]
                y = [-self.params["atten"]] * 2
                self.idealbandhcurves[2].setData(x, y)

                x = [self.params["pbend"] + self.params["tb"]] * 2
                y = [-self.params["atten"],
                     plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[3].setData(x, y)
            else:
                self.detach_allidealcurves(plot)

        except KeyError:
            print('All parameters not set for ideal band diagram')
            self.detach_allidealcurves(plot)

    def detach_allidealcurves(self, plot):
        ''' TODO
        for c in self.idealbandhcurves:
            c.detach()

        for c in self.idealbandvcurves:
            c.detach()
        '''
        plot.replot()

    def detach_unwantedcurves(self, plot):
        for i in range(2, 4):
            self.idealbandvcurves[i].detach()
            self.idealbandhcurves[i].detach()

        plot.replot()

    def attach_allidealcurves(self, plot):
        ''' TODO
        for c in self.idealbandhcurves:
            c.attach(plot)
        for c in self.idealbandvcurves:
            c.attach(plot)
        '''
        plot.replot()
