#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from PyQt5 import QtWidgets
import numpy as np
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import math

from gnuradio import gr
import pmt


class AzElPlot(gr.sync_block, FigureCanvas):
    """
    This block creates a polar plot with azimuth represented as the angle
    clockwise around the circle, and elevation represented as the radius.
    90 degrees elevation is center (directly overhead),
    while the horizon (0 degrees) is the outside circe.  Note that if an
    elevation < 0 is provided, the marker will turn to an open circle
    on the perimeter at the specified azimuth angle.
    """

    def __init__(self, lbl, backgroundColor, dotColor, Parent=None,
                 width=4, height=4, dpi=90):
        gr.sync_block.__init__(self, name="MsgPushButton", in_sig=None,
                               out_sig=None)

        self.lbl = lbl

        self.message_port_register_in(pmt.intern("azel"))
        self.set_msg_handler(pmt.intern("azel"), self.msgHandler)

        self.dotColor = dotColor
        self.backgroundColor = backgroundColor
        self.scaleColor = 'black'
        if (self.backgroundColor == 'black'):
            self.scaleColor = 'white'

        self.fig = Figure(figsize=(width, height), dpi=dpi)
        self.fig.patch.set_facecolor(self.backgroundColor)

        self.axes = self.fig.add_subplot(
            111, polar=True, facecolor=self.backgroundColor)

        # Create an "invisible" line at 90 to set the max for the plot
        self.axes.plot(np.linspace(0, 2 * np.pi, 90), np.ones(90) * 90, color=self.scaleColor,
                       linestyle='')

        # Plot line: Initialize out to 90 and blank
        radius = 90
        self.blackline = self.axes.plot(np.linspace(0, 2 * np.pi, 90), np.ones(90) * radius,
                                        color=self.scaleColor, linestyle='-')
        self.reddot = None

        # Rotate zero up
        self.axes.set_theta_zero_location("N")

        # Set limits:
        self.axes.set_rlim(0, 90)

        self.axes.set_yticklabels([], color=self.scaleColor)
        self.axes.set_xticklabels(['0', '315', '270', '225', '180', '135', '90', '45'],
                                  color=self.scaleColor)

        FigureCanvas.__init__(self, self.fig)
        self.setParent(Parent)

        self.title = self.fig.suptitle(
            self.lbl, fontsize=8, fontweight='bold', color='black')

        FigureCanvas.setSizePolicy(self,
                                   QtWidgets.QSizePolicy.Expanding,
                                   QtWidgets.QSizePolicy.Expanding)

        self.setMinimumSize(240, 230)
        FigureCanvas.updateGeometry(self)

    def msgHandler(self, msg):
        new_val = None

        try:
            new_val = pmt.to_python(pmt.car(msg))
            if new_val is not None:
                if type(new_val) == dict:
                    if 'az' in new_val and 'el' in new_val:
                        self.updateData(
                            float(new_val['az']), float(new_val['el']))
                    else:
                        gr.log.error(
                            "az and el keys were not found in the dictionary.")
                else:
                    gr.log.error("Value received was not a dictionary.  Expecting a dictionary "
                                 "in the car message component with az and el keys.")
            else:
                gr.log.error("The CAR section of the inbound message was None.  "
                             "This part should contain the dictionary with 'az' and 'el' float keys.")
        except Exception as e:
            gr.log.error(
                "[AzElPlot] Error with message conversion: %s" % str(e))
            if new_val is not None:
                gr.log.error(str(new_val))

    def updateData(self, azimuth, elevation):
        if self.reddot is not None:
            self.reddot.pop(0).remove()

        # Plot is angle, radius where angle is in radians

        if (elevation > 0):
            # Need to reverse elevation.  90 degrees is center (directly overhead),
            # and 90 degrees is horizon.
            if (elevation > 90.0):
                elevation = 90.0

            convertedElevation = 90.0 - elevation
            # Note: +azimuth for the plot is measured counter-clockwise, so need to reverse it.
            self.reddot = self.axes.plot(-azimuth * math.pi / 180.0, convertedElevation, self.dotColor,
                                         markersize=8)
        else:
            # It's below the horizon.  Show an open circle at the perimeter
            elevation = 0.0
            self.reddot = self.axes.plot(-azimuth * math.pi / 180.0, 89.0, self.dotColor,
                                         markerfacecolor="None", markersize=16, fillstyle=None)

        self.draw()
