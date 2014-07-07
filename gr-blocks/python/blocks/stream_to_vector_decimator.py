#
# Copyright 2008 Free Software Foundation, Inc.
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

import blocks_swig as blocks
from gnuradio import gr

class stream_to_vector_decimator(gr.hier_block2):
    """
    Convert the stream to a vector, decimate the vector stream to achieve the vector rate.
    """

    def __init__(self, item_size, sample_rate, vec_rate, vec_len):
        """
        Create the block chain.

        Args:
            item_size: the number of bytes per sample
            sample_rate: the rate of incoming samples
            vec_rate: the rate of outgoing vectors (same units as sample_rate)
            vec_len: the length of the outgoing vectors in items
        """
        self._vec_rate = vec_rate
        self._vec_len = vec_len
        self._sample_rate = sample_rate

        gr.hier_block2.__init__(self, "stream_to_vector_decimator",
                                gr.io_signature(1, 1, item_size),         # Input signature
                                gr.io_signature(1, 1, item_size*vec_len)) # Output signature

        s2v = blocks.stream_to_vector(item_size, vec_len)
        self.one_in_n = blocks.keep_one_in_n(item_size*vec_len, 1)
        self._update_decimator()
        self.connect(self, s2v, self.one_in_n, self)

    def set_sample_rate(self, sample_rate):
        """
        Set the new sampling rate and update the decimator.

        Args:
            sample_rate: the new rate
        """
        self._sample_rate = sample_rate
        self._update_decimator()

    def set_vec_rate(self, vec_rate):
        """
        Set the new vector rate and update the decimator.

        Args:
            vec_rate: the new rate
        """
        self._vec_rate = vec_rate
        self._update_decimator()

    def set_decimation(self, decim):
        """
        Set the decimation parameter directly.

        Args:
            decim: the new decimation
        """
        self._decim = max(1, int(round(decim)))
        self.one_in_n.set_n(self._decim)

    def _update_decimator(self):
        self.set_decimation(self._sample_rate/self._vec_len/self._vec_rate)

    def decimation(self):
        """
        Returns the actual decimation.
        """
        return self._decim

    def sample_rate(self):
        """
        Returns configured sample rate.
        """
        return self._sample_rate

    def frame_rate(self):
        """
        Returns actual frame rate
        """
        return self._sample_rate/self._vec_len/self._decim
