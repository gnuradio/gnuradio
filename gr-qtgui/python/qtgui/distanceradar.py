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

import sys
from qtpy import QtWidgets
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from gnuradio import gr
import pmt


class DistanceRadar(gr.sync_block, FigureCanvas):
    """
    This block creates a radar-like screen used to represent distance or size.
    This can be used in many ways such as circles closer to the center are
    closer, or just the opposite where closer to the center is smaller.

    Note: Incoming values should range between 0 (center bullseye) and
    100 (all the way out)
    """

    def __init__(self, lbl, ticklabels, backgroundColor, fontColor, ringColor, Parent=None,
                 width=4, height=4, dpi=100):
        gr.sync_block.__init__(self, name="distanceradar",
                               in_sig=None, out_sig=None)

        self.lbl = lbl

        self.message_port_register_in(pmt.intern("radius"))
        self.set_msg_handler(pmt.intern("radius"), self.msgHandler)

        self.fontColor = fontColor
        self.backgroundColor = backgroundColor
        self.ringColor = ringColor

        self.fig = Figure(figsize=(width, height), dpi=dpi)
        self.fig.patch.set_facecolor(self.backgroundColor)
        self.axes = self.fig.add_subplot(
            111, polar=True, facecolor=self.backgroundColor)

        # Create an "invisible" line at 100 to set the max for the plot
        self.axes.plot(np.linspace(0, 2 * np.pi, 100), np.ones(100) * 100, color=self.fontColor,
                       linestyle='')

        # Plot line: Initialize out to 100 and blank
        radius = 100
        self.blackline = self.axes.plot(np.linspace(0, 2 * np.pi, 100), np.ones(100) * radius,
                                        color=self.fontColor, linestyle='-')
        self.redline = None

        self.filledcircle = None
        # Create bullseye
        circle = plt.Circle((0.0, 0.0), 20, transform=self.axes.transData._b, color=self.fontColor,
                            alpha=0.4)
        self.bullseye = self.axes.add_artist(circle)

        # Rotate zero up
        self.axes.set_theta_zero_location("N")

        self.axes.set_yticklabels(ticklabels, color=self.fontColor)
        self.axes.set_xticklabels([], color=self.fontColor)

        FigureCanvas.__init__(self, self.fig)
        self.setParent(Parent)

        self.title = self.fig.suptitle(self.lbl, fontsize=8, fontweight='bold',
                                       color=self.fontColor)

        FigureCanvas.setSizePolicy(self,
                                   QtWidgets.QSizePolicy.Expanding,
                                   QtWidgets.QSizePolicy.Expanding)
        self.setMinimumSize(240, 230)
        FigureCanvas.updateGeometry(self)

    def msgHandler(self, msg):
        try:
            new_val = pmt.to_python(pmt.cdr(msg))

            if type(new_val) == float or type(new_val) == int:
                self.updateData(new_val)
            else:
                gr.log.error("Value received was not an int or a "
                             "float: %s" % str(type(new_val)))

        except Exception as e:
            gr.log.error("Error with message conversion: %s" % str(e))

    def updateData(self, radius):
        if self.redline is not None:
            self.redline.pop(0).remove()
        self.redline = self.axes.plot(np.linspace(0, 2 * np.pi, 100), np.ones(100) * radius,
                                      color='r', linestyle='-')

        if self.filledcircle:
            self.filledcircle.remove()

        self.bullseye.remove()
        circle = plt.Circle((0.0, 0.0), radius, transform=self.axes.transData._b,
                            color=self.ringColor, alpha=0.4)
        self.filledcircle = self.axes.add_artist(circle)
        # Create bullseye
        circle = plt.Circle((0.0, 0.0), 20, transform=self.axes.transData._b,
                            color=self.fontColor, alpha=0.4)
        self.bullseye = self.axes.add_artist(circle)

        self.draw()
