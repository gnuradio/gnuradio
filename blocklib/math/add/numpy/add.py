from gnuradio import math
from gnuradio import gr

class add_ff(math.add_ff):
    def __init__(self, *args, **kwargs):
        math.add_ff.__init__(self, *args, **kwargs, impl = math.add_ff.available_impl.pyshell)
        self.set_pyblock_detail(gr.pyblock_detail(self))
    
    def work(self, wio):
        out = wio.outputs()[0]
        noutput_items = out.n_items
        out.produce(noutput_items)

        inbuf1 = gr.get_input_array(self, wio, 0)
        inbuf2 = gr.get_input_array(self, wio, 1)
        outbuf1 = gr.get_output_array(self, wio, 0)

        outbuf1[:] = inbuf1 + inbuf2

        return gr.work_return_t.WORK_OK 