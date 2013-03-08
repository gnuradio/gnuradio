#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
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
        # Curent index
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
                result = self.i + self.i/2
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

