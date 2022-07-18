
#
# Copyright 2011-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import numpy as np
import cupy as cp
from gnuradio import gr, gr_unittest, blocks


class add_2_f32_1_f32(gr.cupy_sync_block):
    def __init__(self, shape=[1]):
        gr.cupy_sync_block.__init__(
            self,
            name="add 2 f32")

        self.add_port(gr.port_f("in1", gr.INPUT, shape))
        self.add_port(gr.port_f("in2", gr.INPUT, shape))
        self.add_port(gr.port_f("out", gr.OUTPUT, shape))

    def work(self, inputs, outputs):
        noutput_items = outputs[0].n_items
        
        outputs[0].produce(noutput_items)

        inbuf1 = self.get_input_array(inputs, 0)
        inbuf2 = self.get_input_array(inputs, 1)
        outbuf1 = self.get_output_array(outputs, 0)

        # print(inbuf1)
        # print(inbuf2)
        outbuf1[:] = inbuf1 + inbuf2


        return gr.work_return_t.WORK_OK


class copy_f32(gr.cupy_sync_block):
    def __init__(self, shape=[1]):
        gr.cupy_sync_block.__init__(
            self,
            name="add 2 f32")

        self.add_port(gr.port_f("in", gr.INPUT, shape))
        self.add_port(gr.port_f("out", gr.OUTPUT, shape))

    def work(self, inputs, outputs):
        noutput_items = outputs[0].n_items
        
        outputs[0].produce(noutput_items)

        print (noutput_items)
        inbuf1 = self.get_input_array(inputs, 0)
        outbuf1 = self.get_output_array(outputs, 0)


        # print(inbuf1)
        # print(inbuf2)
        outbuf1[:] = inbuf1


        return gr.work_return_t.WORK_OK

class test_block_gateway(gr_unittest.TestCase):

    def test_add_f32(self):
        tb = gr.flowgraph()
        input_data = list(range(1000))
        expected_output = [2*x for x in input_data]
        src0 = blocks.vector_source_f(input_data, False)
        src1 = blocks.vector_source_f(input_data, False)
        adder = add_2_f32_1_f32()
        copy_blks = []
        nblocks = 1
        bufsize = 1024*1024
        tb.connect((src0, 0), (adder, 0)).set_custom_buffer(gr.buffer_cuda_sm_properties.make(gr.buffer_cuda_sm_type.H2D).set_min_buffer_size(bufsize))
        tb.connect((src1, 0), (adder, 1)).set_custom_buffer(gr.buffer_cuda_sm_properties.make(gr.buffer_cuda_sm_type.H2D).set_min_buffer_size(bufsize))
        last_blk = (adder,0)
        for ii in range(nblocks):
            copy_blks.append(copy_f32())
            tb.connect(last_blk, (copy_blks[ii],0)).set_custom_buffer(gr.buffer_cuda_sm_properties.make(gr.buffer_cuda_sm_type.D2D).set_min_buffer_size(bufsize))
            last_blk = (copy_blks[ii], 0)
        sink = blocks.vector_sink_f()
        
        tb.connect(last_blk, (sink, 0)).set_custom_buffer(gr.buffer_cuda_sm_properties.make(gr.buffer_cuda_sm_type.D2H).set_min_buffer_size(bufsize))
        # tb.connect(adder, sink).set_custom_buffer(gr.buffer_cuda_sm_properties.make(gr.buffer_cuda_sm_type.D2H))

        tb.run()
        self.assertEqual(sink.data(), expected_output)


    # def test_add_f32_vector(self):
    #     tb = gr.flowgraph()
    #     src0 = blocks.vector_source_f(10*[1, 3, 5, 7, 9], False, 5)
    #     src1 = blocks.vector_source_f(10*[0, 2, 4, 6, 8], False, 5)
    #     adder = add_2_f32_1_f32(shape=[5])
    #     adder.name()
    #     sink = blocks.vector_sink_f(5)
    #     tb.connect((src0, 0), (adder, 0))
    #     tb.connect((src1, 0), (adder, 1))
    #     tb.connect(adder, sink)
    #     tb.run()
    #     self.assertEqual(sink.data(), 10*[1, 5, 9, 13, 17])


if __name__ == '__main__':
    gr_unittest.run(test_block_gateway)
