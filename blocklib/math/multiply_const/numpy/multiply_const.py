from gnuradio import math
from gnuradio import gr

class multiply_const_ff:
    def __init__(self, blk, **kwargs):
    
        # In the future we should be able to use the block parameters
        #  But the pmts are not properly pybinded yet
        #  For now, just grab the arg
        self.blk = blk

        self.k = kwargs['k']
        self.vlen = kwargs['vlen']


    def work(self, wio):
        out = wio.outputs()[0]
        noutput_items = out.n_items

        inbuf1 = gr.get_input_array(self.blk, wio, 0)
        outbuf1 = gr.get_output_array(self.blk, wio, 0)

        outbuf1[:] = inbuf1 * self.k

        wio.produce_each(noutput_items)
        return gr.work_return_t.OK 