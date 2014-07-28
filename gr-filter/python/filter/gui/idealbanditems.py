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

from PyQt4 import QtGui, QtCore, Qt
import PyQt4.Qwt5 as Qwt
import scipy

class IdealBandItems():
    def __init__(self):
        self.idealbandhcurves= [ Qwt.QwtPlotCurve() for i in range(4) ]
        self.idealbandvcurves= [ Qwt.QwtPlotCurve() for i in range(4) ]
        self.params=""

    def setLinetype(self):
        for c in self.idealbandhcurves:
            c.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
            c.setPen(Qt.QPen(Qt.Qt.red, 2, Qt.Qt.DotLine))
        for c in self.idealbandvcurves:
            c.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
            c.setPen(Qt.QPen(Qt.Qt.red, 2, Qt.Qt.DotLine))

    def plotIdealCurves(self, ftype, params, plot):
        self.params=params
        try:
            if (ftype == "Low Pass"):
                self.detach_unwantedcurves(plot)
                x=[0, self.params["pbend"]]
                y=[20.0*scipy.log10(self.params["gain"])]*2
                self.idealbandhcurves[0].setData(x, y)

                x=[self.params["pbend"]]*2
                y=[20.0*scipy.log10(self.params["gain"]),
	               plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[0].setData(x, y)

                x=[self.params["sbstart"], self.params["fs"]/2.0]
                y=[-self.params["atten"]]*2
                self.idealbandhcurves[1].setData(x, y)

                x=[self.params["sbstart"]]*2
                y=[-self.params["atten"],
	               plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[1].setData(x, y)

            elif ftype == "High Pass":
                self.detach_unwantedcurves(plot)
                x=[self.params["pbstart"],self.params["fs"]/2.0]
                y=[20.0*scipy.log10(self.params["gain"])]*2
                self.idealbandhcurves[0].setData(x, y)

                x=[self.params["pbstart"]]*2
                y=[20.0*scipy.log10(self.params["gain"]),
	               plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[0].setData(x, y)

                x=[0,self.params["sbend"]]
                y=[-self.params["atten"]]*2
                self.idealbandhcurves[1].setData(x, y)

                x=[self.params["sbend"]]*2
                y=[-self.params["atten"],
	               plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[1].setData(x, y)

            elif ftype == "Band Notch":
                x=[self.params["sbstart"],self.params["sbend"]]
                y=[-self.params["atten"]]*2
                self.idealbandhcurves[0].setData(x, y)

                x=[self.params["sbstart"]]*2
                y=[-self.params["atten"],
	               plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[0].setData(x, y)

                x=[self.params["sbend"]]*2
                y=[-self.params["atten"],
	               plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[1].setData(x, y)

                x=[0,self.params["sbstart"]-self.params["tb"]]
                y=[20.0*scipy.log10(self.params["gain"])]*2
                self.idealbandhcurves[1].setData(x, y)

                x=[self.params["sbstart"]-self.params["tb"]]*2
                y=[20.0*scipy.log10(self.params["gain"]),
	               plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[2].setData(x, y)

                x=[self.params["sbend"]+self.params["tb"],self.params["fs"]/2.0]
                y=[20.0*scipy.log10(self.params["gain"])]*2
                self.idealbandhcurves[2].setData(x, y)

                x=[self.params["sbend"]+self.params["tb"]]*2
                y=[20.0*scipy.log10(self.params["gain"]),
	               plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[3].setData(x, y)

            elif ftype == "Band Pass":
                x=[self.params["pbstart"],self.params["pbend"]]
                y=[20.0*scipy.log10(self.params["gain"])]*2
                self.idealbandhcurves[0].setData(x, y)

                x=[self.params["pbstart"]]*2
                y=[20.0*scipy.log10(self.params["gain"]),
	        	   plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[0].setData(x, y)

                x=[self.params["pbend"]]*2
                y=[20.0*scipy.log10(self.params["gain"]),
	        	   plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[1].setData(x, y)

                x=[0,self.params["pbstart"]-self.params["tb"]]
                y=[-self.params["atten"]]*2
                self.idealbandhcurves[1].setData(x, y)

                x=[self.params["pbstart"]-self.params["tb"]]*2
                y=[-self.params["atten"],
	        	   plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[2].setData(x, y)

                x=[self.params["pbend"]+self.params["tb"],self.params["fs"]/2.0]
                y=[-self.params["atten"]]*2
                self.idealbandhcurves[2].setData(x, y)

                x=[self.params["pbend"]+self.params["tb"]]*2
                y=[-self.params["atten"],
	        	   plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[3].setData(x, y)

            elif ftype == "Complex Band Pass":
                x=[self.params["pbstart"],self.params["pbend"]]
                y=[20.0*scipy.log10(self.params["gain"])]*2
                self.idealbandhcurves[0].setData(x, y)

                x=[self.params["pbstart"]]*2
                y=[20.0*scipy.log10(self.params["gain"]),
	        	   plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[0].setData(x, y)

                x=[self.params["pbend"]]*2
                y=[20.0*scipy.log10(self.params["gain"]),
	        	   plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[1].setData(x, y)

                x=[0,self.params["pbstart"]-self.params["tb"]]
                y=[-self.params["atten"]]*2
                self.idealbandhcurves[1].setData(x, y)

                x=[self.params["pbstart"]-self.params["tb"]]*2
                y=[-self.params["atten"],
	        	   plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[2].setData(x, y)

                x=[self.params["pbend"]+self.params["tb"],self.params["fs"]/2.0]
                y=[-self.params["atten"]]*2
                self.idealbandhcurves[2].setData(x, y)

                x=[self.params["pbend"]+self.params["tb"]]*2
                y=[-self.params["atten"],
	        	   plot.axisScaleDiv(Qwt.QwtPlot.yLeft).lowerBound()]
                self.idealbandvcurves[3].setData(x, y)
            else:
                self.detach_allidealcurves(plot)

        except KeyError:
            print "All parameters not set for ideal band diagram"
            self.detach_allidealcurves(plot)

    def detach_allidealcurves(self, plot):
        for c in self.idealbandhcurves:
            c.detach()
        for c in self.idealbandvcurves:
            c.detach()
        plot.replot()

    def detach_unwantedcurves(self, plot):
        for i in range(2,4):
            self.idealbandvcurves[i].detach()
            self.idealbandhcurves[i].detach()
        plot.replot()

    def attach_allidealcurves(self, plot):
        for c in self.idealbandhcurves:
            c.attach(plot)
        for c in self.idealbandvcurves:
            c.attach(plot)
        plot.replot()
