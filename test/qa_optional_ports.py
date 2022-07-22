
import numpy as np
from gnuradio import gr, gr_unittest, blocks

#This test is a pure python block that inherits from sync_block
class add_optional(gr.sync_block):
    def __init__(self, shape=[1]):
        gr.sync_block.__init__(
            self,
            name="add 2 f32")

        self.add_port(gr.port_f("in1", gr.INPUT, shape))
        self.add_port(gr.port_f("in2", gr.INPUT, shape))
        self.add_port(gr.port_f("in3", gr.INPUT, shape, optional=True))
        self.add_port(gr.port_f("out", gr.OUTPUT, shape))

    def work(self, wio):
        noutput_items = wio.outputs()[0].n_items
        
        wio.outputs()[0].produce(noutput_items)

        inbuf1 = self.get_input_array(wio, 0)
        inbuf2 = self.get_input_array(wio, 1)
        if (len(wio.inputs()) > 2):
            inbuf3 = self.get_input_array(wio, 2)
        outbuf1 = self.get_output_array(wio, 0)

        outbuf1[:] = inbuf1 + inbuf2
        if (len(wio.inputs()) > 2):
            outbuf1[:] = outbuf1[:] + inbuf3

        return gr.work_return_t.WORK_OK

class test_optional_ports(gr_unittest.TestCase):

    def test_optional_absent(self):
        tb = gr.flowgraph()
        src0 = blocks.vector_source_f([1, 3, 5, 7, 9], False)
        src1 = blocks.vector_source_f([0, 2, 4, 6, 8], False)
        adder = add_optional()
        sink = blocks.vector_sink_f()
        tb.connect((src0, 0), (adder, 0))
        tb.connect((src1, 0), (adder, 1))
        tb.connect(adder, sink)
        tb.run()
        self.assertEqual(sink.data(), [1, 5, 9, 13, 17])

    def test_optional_present(self):
        tb = gr.flowgraph()
        src0 = blocks.vector_source_f([1, 3, 5, 7, 9], False)
        src1 = blocks.vector_source_f([0, 2, 4, 6, 8], False)
        src2 = blocks.vector_source_f([1, 2, 3, 4, 5], False)
        adder = add_optional()
        sink = blocks.vector_sink_f()
        tb.connect((src0, 0), (adder, 0))
        tb.connect((src1, 0), (adder, 1))
        tb.connect((src2, 0), (adder, 2))
        tb.connect(adder, sink)
        tb.run()
        self.assertEqual(sink.data(), [2, 7, 12, 17, 22])



if __name__ == '__main__':
    gr_unittest.run(test_optional_ports)
