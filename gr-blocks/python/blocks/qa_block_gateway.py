
#
# Copyright 2011-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import numpy

import pmt

from gnuradio import gr, gr_unittest, blocks


class non_sync_block(gr.basic_block):
    def __init__(self):
        gr.basic_block.__init__(self,
                                name="non_sync_block",
                                in_sig=[numpy.float32],
                                out_sig=[numpy.float32, numpy.float32])

    def general_work(self, input_items, output_items):
        self.consume(0, len(input_items[0]))
        self.produce(0, 2)
        self.produce(1, 1)
        return gr.WORK_CALLED_PRODUCE


class add_2_f32_1_f32(gr.sync_block):
    def __init__(self):
        gr.sync_block.__init__(
            self,
            name="add 2 f32",
            in_sig=[numpy.float32, numpy.float32],
            out_sig=[numpy.float32],
        )

    def work(self, input_items, output_items):
        output_items[0][:] = input_items[0] + input_items[1]
        return len(output_items[0])


class add_2_fc32_1_fc32(gr.sync_block):
    def __init__(self):
        gr.sync_block.__init__(
            self,
            name="add 2 fc32",
            in_sig=[numpy.complex64, numpy.complex64],
            out_sig=[numpy.complex64],
        )

    def work(self, input_items, output_items):
        output_items[0][:] = input_items[0] + input_items[1]
        return len(output_items[0])


class convolve(gr.sync_block):
    """
    A demonstration using block history to properly perform a convolution.
    """

    def __init__(self):
        gr.sync_block.__init__(
            self,
            name="convolve",
            in_sig=[numpy.float32],
            out_sig=[numpy.float32]
        )
        self._taps = [1, 0, 0, 0]
        self.set_history(len(self._taps))

    def work(self, input_items, output_items):
        output_items[0][:] = numpy.convolve(
            input_items[0], self._taps, mode='valid')
        return len(output_items[0])


class decim2x(gr.decim_block):
    def __init__(self):
        gr.decim_block.__init__(
            self,
            name="decim2x",
            in_sig=[numpy.float32],
            out_sig=[numpy.float32],
            decim=2
        )

    def work(self, input_items, output_items):
        output_items[0][:] = input_items[0][::2]
        return len(output_items[0])


class interp2x(gr.interp_block):
    def __init__(self):
        gr.interp_block.__init__(
            self,
            name="interp2x",
            in_sig=[numpy.float32],
            out_sig=[numpy.float32],
            interp=2
        )

    def work(self, input_items, output_items):
        output_items[0][1::2] = input_items[0]
        output_items[0][::2] = input_items[0]
        return len(output_items[0])


class tag_source(gr.sync_block):
    def __init__(self):
        gr.sync_block.__init__(
            self,
            name="tag source",
            in_sig=None,
            out_sig=[numpy.float32],
        )

    def work(self, input_items, output_items):
        num_output_items = len(output_items[0])

        # put code here to fill the output items...
        if self.nitems_written(0) == 0:
            # skip tagging in the first work block
            return num_output_items

        # make a new tag on the middle element every time work is called
        count = self.nitems_written(0) + num_output_items // 2
        key = pmt.string_to_symbol("example_key")
        value = pmt.string_to_symbol("example_value")
        self.add_item_tag(0, count, key, value)

        return num_output_items


class tag_sink(gr.sync_block):
    def __init__(self):
        gr.sync_block.__init__(
            self,
            name="tag sink",
            in_sig=[numpy.float32],
            out_sig=None,
        )
        self.key = None

    def work(self, input_items, output_items):
        num_input_items = len(input_items[0])

        # put code here to process the input items...

        # print all the tags received in this work call
        nread = self.nitems_read(0)
        tags = self.get_tags_in_range(0, nread, nread + num_input_items)
        for tag in tags:
            # print tag.offset
            # print pmt.symbol_to_string(tag.key)
            # print pmt.symbol_to_string(tag.value)
            self.key = pmt.symbol_to_string(tag.key)

        return num_input_items


class tag_sink_win(gr.sync_block):
    def __init__(self):
        gr.sync_block.__init__(self, name="tag sink",
                               in_sig=[numpy.float32],
                               out_sig=None)
        self.key = None

    def work(self, input_items, output_items):
        num_input_items = len(input_items[0])
        tags = self.get_tags_in_window(0, 0, num_input_items)
        for tag in tags:
            self.key = pmt.symbol_to_string(tag.key)
        return num_input_items


class fc32_to_f32_2(gr.sync_block):
    def __init__(self):
        gr.sync_block.__init__(
            self,
            name="fc32_to_f32_2",
            in_sig=[numpy.complex64],
            out_sig=[(numpy.float32, 2)],
        )

    def work(self, input_items, output_items):
        output_items[0][::, 0] = numpy.real(input_items[0])
        output_items[0][::, 1] = numpy.imag(input_items[0])
        return len(output_items[0])


class vector_to_stream(gr.interp_block):
    def __init__(self, itemsize, nitems_per_block):
        gr.interp_block.__init__(
            self,
            name="vector_to_stream",
            in_sig=[(itemsize, nitems_per_block)],
            out_sig=[itemsize],
            interp=nitems_per_block
        )
        self.block_size = nitems_per_block

    def work(self, input_items, output_items):
        n = 0
        for i in range(len(input_items[0])):
            for j in range(self.block_size):
                output_items[0][n] = input_items[0][i][j]
                n += 1

        return len(output_items[0])


class test_block_gateway(gr_unittest.TestCase):

    def test_add_f32(self):
        tb = gr.top_block()
        src0 = blocks.vector_source_f([1, 3, 5, 7, 9], False)
        src1 = blocks.vector_source_f([0, 2, 4, 6, 8], False)
        adder = add_2_f32_1_f32()
        adder.name()
        sink = blocks.vector_sink_f()
        tb.connect((src0, 0), (adder, 0))
        tb.connect((src1, 0), (adder, 1))
        tb.connect(adder, sink)
        tb.run()
        self.assertEqual(sink.data(), [1, 5, 9, 13, 17])

    def test_add_fc32(self):
        tb = gr.top_block()
        src0 = blocks.vector_source_c([1, 3j, 5, 7j, 9], False)
        src1 = blocks.vector_source_c([0, 2j, 4, 6j, 8], False)
        adder = add_2_fc32_1_fc32()
        sink = blocks.vector_sink_c()
        tb.connect((src0, 0), (adder, 0))
        tb.connect((src1, 0), (adder, 1))
        tb.connect(adder, sink)
        tb.run()
        self.assertEqual(sink.data(), [1, 5j, 9, 13j, 17])

    def test_convolve(self):
        tb = gr.top_block()
        src = blocks.vector_source_f([1, 2, 3, 4, 5, 6, 7, 8], False)
        cv = convolve()
        sink = blocks.vector_sink_f()
        tb.connect(src, cv, sink)
        tb.run()
        self.assertEqual(sink.data(), [1, 2, 3, 4, 5, 6, 7, 8])

    def test_decim2x(self):
        tb = gr.top_block()
        src = blocks.vector_source_f([1, 2, 3, 4, 5, 6, 7, 8], False)
        d2x = decim2x()
        sink = blocks.vector_sink_f()
        tb.connect(src, d2x, sink)
        tb.run()
        self.assertEqual(sink.data(), [1, 3, 5, 7])

    def test_interp2x(self):
        tb = gr.top_block()
        src = blocks.vector_source_f([1, 3, 5, 7, 9], False)
        i2x = interp2x()
        sink = blocks.vector_sink_f()
        tb.connect(src, i2x, sink)
        tb.run()
        self.assertEqual(sink.data(), [1, 1, 3, 3, 5, 5, 7, 7, 9, 9])

    def test_tags(self):
        src = tag_source()
        sink = tag_sink()
        # should be enough items to get a tag through
        head = blocks.head(gr.sizeof_float, 50000)
        tb = gr.top_block()
        tb.connect(src, head, sink)
        tb.run()
        self.assertEqual(sink.key, "example_key")

    def test_tags_win(self):
        src = tag_source()
        sink = tag_sink_win()
        # should be enough items to get a tag through
        head = blocks.head(gr.sizeof_float, 50000)
        tb = gr.top_block()
        tb.connect(src, head, sink)
        tb.run()
        self.assertEqual(sink.key, "example_key")

    def test_fc32_to_f32_2(self):
        tb = gr.top_block()
        src = blocks.vector_source_c(
            [1 + 2j, 3 + 4j, 5 + 6j, 7 + 8j, 9 + 10j], False)
        convert = fc32_to_f32_2()
        v2s = vector_to_stream(numpy.float32, 2)
        sink = blocks.vector_sink_f()
        tb.connect(src, convert, v2s, sink)
        tb.run()
        self.assertEqual(sink.data(), [1, 2, 3, 4, 5, 6, 7, 8, 9, 10])

    def test_non_sync_block(self):
        tb = gr.top_block()
        src = blocks.vector_source_f(range(1000000))
        sinks = [blocks.vector_sink_f(), blocks.vector_sink_f()]
        dut = non_sync_block()
        tb.connect(src, dut)
        tb.connect((dut, 0), sinks[0])
        tb.connect((dut, 1), sinks[1])
        tb.run()
        self.assertEqual(len(sinks[0].data()), 2 * len(sinks[1].data()))


if __name__ == '__main__':
    gr_unittest.run(test_block_gateway)
