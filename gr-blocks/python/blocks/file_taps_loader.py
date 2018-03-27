#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018 Free Software Foundation, Inc.
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

import numpy
from gnuradio import gr
from pathlib import Path
import numpy as np
import csv
import sys

class file_taps_loader(gr.basic_block):
    """
    Block to make filter taps created by the filter design tool available.
    """
    def __init__(self, fpath, verbose):
        gr.basic_block.__init__(self,
            name="file_taps_loader",
            in_sig=[],
            out_sig=[])
        self.taps = []
        self.params = []
        self.fpath = fpath
        self.verbose = verbose
        # Load taps
        self.taps_from_grc(fpath)

    def taps_from_grc(self, fpath):
      tfile = Path(fpath)
      if tfile.is_file():
        with open(fpath) as csvfile:
          readcsv = csv.reader(csvfile, delimiter=',')
          for row in readcsv:
            if row[0] == "taps":
              self.taps = np.array(row[1:], dtype=float)
            else:
              self.params.append(row)
        self.print_if("Loaded a filter with the following parameters (gr_filter_design format)\n")
        for param in self.params:
          self.print_if(param[0], ' ', param[1], '\n')
      else:
        raise RuntimeError("Can not open " + "\"" + fpath + "\"" + ".")

    def print_if(self, *tbps):
      if self.verbose:
        for tbp in tbps:
          sys.stdout.write(tbp)

    def get_taps(self):
      taps = self.taps
      return taps

    def get_params(self):
      params = self.params
      return params
