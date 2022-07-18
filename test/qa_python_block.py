
#
# Copyright 2011-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import numpy as np
from gnuradio import gr, gr_unittest, blocks, math

#This test is a pure python block that inherits from sync_block
class add_2_f32_1_f32(gr.sync_block):
    def __init__(self, shape=[1]):
        gr.sync_block.__init__(
            self,
            name="add 2 f32")

        self.add_port_f("in1", gr.INPUT, shape)
        self.add_port_f("in2", gr.INPUT, shape)
        self.add_port_f("out", gr.OUTPUT, shape)

    def work(self, wio):
        noutput_items = wio.outputs()[0].n_items
        
        wio.outputs()[0].produce(noutput_items)

        inbuf1 = self.get_input_array(wio, 0)
        inbuf2 = self.get_input_array(wio, 1)
        outbuf1 = self.get_output_array(wio, 0)

        outbuf1[:] = inbuf1 + inbuf2

        return gr.work_return_t.WORK_OK

class add_2_f32_1_f32_named(gr.sync_block):
    def __init__(self, shape=[1]):
        gr.sync_block.__init__(
            self,
            name="add 2 f32 named")

        self.add_port_f("in1", gr.INPUT, shape)
        self.add_port_f("in2", gr.INPUT, shape)
        self.add_port_f("out", gr.OUTPUT, shape)

    def work(self, wio):
        out = wio.outputs()["out"]
        
        inbuf1 = self.get_input_array(wio, "in1")
        inbuf2 = self.get_input_array(wio, "in2")
        outbuf1 = self.get_output_array(wio, "out")

        outbuf1[:] = inbuf1 + inbuf2

        out.produce(out.n_items)
        return gr.work_return_t.WORK_OK

# This test extends the existing add_ff block by adding a custom python implementation
class add_ff_numpy(math.add_ff):
    def __init__(self, shape=[1]):
        math.add_ff.__init__(self, impl = math.add_ff.available_impl.pyshell)
        self.set_pyblock_detail(gr.pyblock_detail(self))

    def work(self, wio):
        noutput_items = wio.outputs()[0].n_items
        
        wio.outputs()[0].produce(noutput_items)

        inbuf1 = gr.get_input_array(self, wio, 0)
        inbuf2 = gr.get_input_array(self, wio, 1)
        outbuf1 = gr.get_output_array(self, wio, 0)

        outbuf1[:] = inbuf1 + inbuf2

        return gr.work_return_t.WORK_OK

class test_block_gateway(gr_unittest.TestCase):

    def test_add_ff_deriv(self):
        tb = gr.flowgraph()
        rt = gr.runtime()
        src0 = blocks.vector_source_f([1, 3, 5, 7, 9], False)
        src1 = blocks.vector_source_f([0, 2, 4, 6, 8], False)
        adder = add_ff_numpy()
        sink = blocks.vector_sink_f()
        tb.connect((src0, 0), (adder, 0))
        tb.connect((src1, 0), (adder, 1))
        tb.connect(adder, sink)
        rt.initialize(tb)
        rt.run()
        self.assertEqual(sink.data(), [1, 5, 9, 13, 17])

    # def test_add_f32(self):
    #     tb = gr.flowgraph()
    #     src0 = blocks.vector_source_f([1, 3, 5, 7, 9], False)
    #     src1 = blocks.vector_source_f([0, 2, 4, 6, 8], False)
    #     adder = add_2_f32_1_f32()
    #     sink = blocks.vector_sink_f()
    #     tb.connect((src0, 0), (adder, 0))
    #     tb.connect((src1, 0), (adder, 1))
    #     tb.connect(adder, sink)
    #     tb.run()
    #     self.assertEqual(sink.data(), [1, 5, 9, 13, 17])


    # def test_add_f32_vector(self):
    #     tb = gr.flowgraph()
    #     src0 = blocks.vector_source_f(10*[1, 3, 5, 7, 9], False, 5)
    #     src1 = blocks.vector_source_f(10*[0, 2, 4, 6, 8], False, 5)
    #     adder = add_2_f32_1_f32(shape=[5])
    #     sink = blocks.vector_sink_f(5)
    #     tb.connect((src0, 0), (adder, 0))
    #     tb.connect((src1, 0), (adder, 1))
    #     tb.connect(adder, sink)
    #     tb.run()
    #     self.assertEqual(sink.data(), 10*[1, 5, 9, 13, 17])

    # def test_add_f32_named(self):
    #     tb = gr.flowgraph()
    #     src0 = blocks.vector_source_f([1, 3, 5, 7, 9], False)
    #     src1 = blocks.vector_source_f([0, 2, 4, 6, 8], False)
    #     adder = add_2_f32_1_f32_named()
    #     sink = blocks.vector_sink_f()
    #     tb.connect((src0, 0), (adder, 0))
    #     tb.connect((src1, 0), (adder, 1))
    #     tb.connect(adder, sink)
    #     tb.run()
    #     self.assertEqual(sink.data(), [1, 5, 9, 13, 17])



if __name__ == '__main__':
    gr_unittest.run(test_block_gateway)
