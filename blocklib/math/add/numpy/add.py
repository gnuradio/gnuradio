from gnuradio import math
from gnuradio import gr

class add_blk:
    def __init__(self, blk, **kwargs):
        self.blk = blk
        self.nports = kwargs["nports"]

    def work(self, wio):
        out = wio.outputs()[0]
        noutput_items = out.n_items
        out.produce(noutput_items)

        inbuf1 = gr.get_input_array(self.blk, wio, 0)
        outbuf1 = gr.get_output_array(self.blk, wio, 0)

        outbuf1[:] = inbuf1
        for i in range(1, self.nports):
            inbufN = gr.get_input_array(self.blk, wio, i)
            outbuf1[:] += inbufN

        return gr.work_return_t.OK 

class add_cc(add_blk):
    def __init__(self, blk, **kwargs):
        super().__init__(blk, **kwargs)

class add_ff(add_blk):
    def __init__(self, blk, **kwargs):
        super().__init__(blk, **kwargs)

class add_ii(add_blk):
    def __init__(self, blk, **kwargs):
        super().__init__(blk, **kwargs)

class add_ss(add_blk):
    def __init__(self, blk, **kwargs):
        super().__init__(blk, **kwargs)

class add_bb(add_blk):
    def __init__(self, blk, **kwargs):
        super().__init__(blk, **kwargs)
