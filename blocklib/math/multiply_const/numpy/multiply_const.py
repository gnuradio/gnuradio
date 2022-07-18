from gnuradio import math
from gnuradio import gr

class multiply_const_ff(math.multiply_const_ff):
    def __init__(self, *args, **kwargs):
        math.multiply_const_ff.__init__(self, *args, **kwargs, 
            impl = math.multiply_const_ff.available_impl.pyshell)
        self.set_pyblock_detail(gr.pyblock_detail(self))
    
        # In the future we should be able to use the block parameters
        #  But the pmts are not properly pybinded yet
        #  For now, just grab the arg

        self.k = kwargs['k'] if 'k' in kwargs else args[0] # should throw 
        self.vlen = kwargs['vlen'] if 'vlen' in kwargs else 1


    def work(self, wio):
        out = wio.outputs()[0]
        noutput_items = out.n_items
        out.produce(noutput_items)

        inbuf1 = gr.get_input_array(self, wio, 0)
        outbuf1 = gr.get_output_array(self, wio, 0)

        outbuf1[:] = inbuf1 * self.k

        return gr.work_return_t.WORK_OK 