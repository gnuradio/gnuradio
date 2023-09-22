#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import csv
import sys
import re
import os
import numpy as np
from gnuradio import gr


class file_taps_loader(gr.basic_block):
    """
    Block to make filter taps created by the filter design tool available in grc.
    """

    def __init__(self, fpath, verbose):
        gr.basic_block.__init__(
            self,
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
        """
        Load a file that was generated with the filter design tool
        """
        if not os.path.isfile(self.fpath):
            raise RuntimeError(
                self.name() + ": Can not open " + "\"" + fpath + "\"" + ".")
        with open(fpath) as csvfile:
            readcsv = csv.reader(csvfile, delimiter=',')
            for row in readcsv:
                if row[0] == "taps":
                    regex = re.findall(r"[+-]?\d+\.*\d*[Ee]?[-+]?\d+j", row[1])
                    if regex:  # it's a complex
                        # string to complex so numpy eats the taps
                        cpx_row = [complex(x) for x in row[1:]]
                        self.taps = tuple(np.array(cpx_row, dtype=complex))
                        self.print_if(
                            self.name() + ": Found complex taps in the file provided.\n")
                    else:
                        self.taps = tuple(np.array(row[1:], dtype=float))
                        self.print_if(
                            self.name() + ": Found real taps in the file provided.\n")
                else:
                    regex = re.findall(r"[+-]?\d+\.*\d*[Ee]?[-+]?\d+j", row[0])
                    if regex:  # it's a complex
                        # string to complex so numpy eats the taps
                        cpx_row = [complex(x) for x in row[0:]]
                        self.taps = tuple(np.array(cpx_row, dtype=complex))
                        self.print_if(
                            self.name() + ": Found complex taps in the file provided.\n")
                    else:
                        try:
                            self.taps = tuple(np.array(row[0:], dtype=float))
                            self.print_if(
                                self.name() + ": Found real taps in the file provided.\n")
                        except ValueError:
                            self.params.append(row)
        self.print_if(
            self.name() +
            ": Loaded a filter with the following parameters (gr_filter_design format): \n")
        for param in self.params:
            self.print_if(param[0], ' ', param[1], '\n')

    def print_if(self, *tbps):
        if self.verbose:
            for tbp in tbps:
                sys.stdout.write(tbp)

    def get_taps(self):
        " Return taps "
        return self.taps

    def get_params(self):
        " Return params "
        return self.params

    def get_path(self):
        " Return path "
        return self.fpath
