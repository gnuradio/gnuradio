#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2016 Free Software Foundation, Inc.
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

from PyQt4 import Qt, QtCore, QtGui
from gnuradio import gr

def check_set_qss():
    app = QtGui.qApp
    qssfile = gr.prefs().get_string("qtgui","qss","")
    if(len(qssfile)>0):
        try:
            app.setStyleSheet(open(qssfile).read())
        except:
            print "WARNING: bad QSS file, %s"%(qssfile)

