#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import division
from __future__ import unicode_literals
class GrayCodeGenerator(object):
    """
    Generates and caches gray codes.
    """

    def __init__(self):
        self.gcs = [0, 1]
        # The last power of two passed through.
        self.lp2 = 2
        # The next power of two that will be passed through.
        self.np2 = 4
        # Current index
        self.i = 2

    def get_gray_code(self, length):
        """
        Returns a list of gray code of given length.
        """
        if len(self.gcs) < length:
            self.generate_new_gray_code(length)
        return self.gcs[:length]

    def generate_new_gray_code(self, length):
        """
        Generates new gray code and places into cache.
        """
        while len(self.gcs) < length:
            if self.i == self.lp2:
                # if i is a power of two then gray number is of form 1100000...
                result = self.i + self.i // 2
            else:
                # if not we take advantage of the symmetry of all but the last bit
                # around a power of two.
                result = self.gcs[2*self.lp2-1-self.i] + self.lp2
            self.gcs.append(result)
            self.i += 1
            if self.i == self.np2:
                self.lp2 = self.i
                self.np2 = self.i*2

_gray_code_generator = GrayCodeGenerator()

gray_code = _gray_code_generator.get_gray_code

