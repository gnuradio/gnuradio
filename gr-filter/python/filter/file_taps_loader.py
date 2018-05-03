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
import numpy as np
import csv, sys, re, os

class file_taps_loader(gr.basic_block):
    """
    Block to make filter taps created by the filter design tool available in grc.
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
        self.taps_from_design_tool(fpath)

    def taps_from_design_tool(self, fpath):
      if os.path.isfile(self.fpath):
        with open(fpath) as csvfile:
          readcsv = csv.reader(csvfile, delimiter=',')
          for row in readcsv:
            if row[0] == "taps":
                regex = re.findall("[+-]?\d+\.*\d*[Ee]?[-+]?\d+j", row[1])
                if(len(regex) > 0): # it's a complex
                  cpx_row = map(lambda cpx_str: complex(cpx_str), row[1:]) # string to complex so numpy eats the taps
                  self.taps = tuple(np.array(cpx_row, dtype=complex))
                  self.print_if(self.name() + ": Found complex taps in the file provided.\n")
                else:
                  self.taps = tuple(np.array(row[1:], dtype=float))
                  self.print_if(self.name() + ": Found real taps in the file provided.\n")
            else:
              regex = re.findall("[+-]?\d+\.*\d*[Ee]?[-+]?\d+j", row[0])
              if(len(regex) > 0): # it's a complex
                cpx_row = map(lambda cpx_str: complex(cpx_str), row[0:]) # string to complex so numpy eats the taps
                self.taps = tuple(np.array(cpx_row, dtype=complex))
                self.print_if(self.name() + ": Found complex taps in the file provided.\n")
              else:
                try:
                  self.taps = tuple(np.array(row[0:], dtype=float))
                  self.print_if(self.name() + ": Found real taps in the file provided.\n")
                except ValueError:
                  self.params.append(row)
        self.print_if(self.name() + ": Loaded a filter with the following parameters (gr_filter_design format): \n")
        for param in self.params:
          self.print_if(param[0], ' ', param[1], '\n')
      else:
        raise RuntimeError(self.name() + ": Can not open " + "\"" + fpath + "\"" + ".")

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

    def get_path(self):
      path = self.fpath
      return path
