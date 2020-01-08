#!/usr/bin/env python
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

from gnuradio import gr, gr_unittest, blocks
from multiprocessing import Process
import numpy


# This block just exists to pass data through and throw an exception
class except_block(gr.sync_block):
    def __init__(self, throw_except, except_count=10000):
        gr.sync_block.__init__(
            self,
            name="except_block",
            in_sig=[numpy.complex64],
            out_sig=[numpy.complex64],
        )
        self.throw_except = throw_except
        self.except_count = except_count
        self.count = 0

    def work(self, input_items, output_items):
        output_items[0][:] = input_items[0]
        self.count += len(output_items[0])
        if self.count >= self.except_count:
            raise RuntimeError("Error in except_block")
        return len(output_items[0])


def process_func(catch_exceptions):
    tb = gr.top_block(catch_exceptions=catch_exceptions)
    # some test data
    src_data = [complex(x, x + 1) for x in range(65536)]
    src = blocks.vector_source_c(src_data)
    src.set_repeat(True)

    e_block_1 = except_block(False)
    e_block_2 = except_block(True)

    sink_1 = blocks.null_sink(gr.sizeof_gr_complex)
    sink_2 = blocks.null_sink(gr.sizeof_gr_complex)

    tb.connect(src, e_block_1)
    tb.connect(src, e_block_2)
    tb.connect(e_block_1, sink_1)
    tb.connect(e_block_2, sink_2)
    tb.run()


class test_uncaught_exception(gr_unittest.TestCase):

    def test_exception_throw_uncaught(self):
        # Test to ensure that throwing an exception causes the
        # process running top_block to exit
        p = Process(target=process_func, args=(False,))
        p.daemon = True
        p.start()
        p.join(2.5)
        exit_code = p.exitcode
        self.assertIsNotNone(
            exit_code, "exception did not cause flowgraph exit")

    def test_exception_throw_caught(self):
        # Test to ensure that throwing an exception does not cause the process
        # running top_block to exit (in catch_exceptions mode)
        p = Process(target=process_func, args=(True,))
        p.daemon = True
        p.start()
        p.join(2.5)
        exit_code = p.exitcode
        self.assertIsNone(
            exit_code, "exception caused flowgraph exit")


if __name__ == '__main__':
    gr_unittest.run(test_uncaught_exception, "test_uncaught_exception.xml")
