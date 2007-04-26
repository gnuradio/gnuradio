#!/usr/bin/env python

from gnuradio import gr

class my_graph(gr.flow_graph):
    def __init__(self):
        gr.flow_graph.__init__(self)

        length = 101

        data_r = range(length)
        data_i = range(length,2*length)
        src_r = gr.vector_source_s(data_r, False)
        src_i = gr.vector_source_s(data_i, False)
        s2f_r = gr.short_to_float()
        s2f_i = gr.short_to_float()
        f2c = gr.float_to_complex()
        s2v = gr.stream_to_vector(gr.sizeof_gr_complex, length)

        shift = True
        ifft = gr.fft_vcc(length, False, [], shift)
        fft  = gr.fft_vcc(length, True, [], shift)
        
        v2s = gr.vector_to_stream(gr.sizeof_gr_complex, length)
        snk_in = gr.file_sink(gr.sizeof_gr_complex, "fftshift.in")
        snk_out = gr.file_sink(gr.sizeof_gr_complex, "fftshift.out")

        self.connect(src_r, s2f_r, (f2c,0))
        self.connect(src_i, s2f_i, (f2c,1))
        self.connect(f2c, snk_in)
        self.connect(f2c, s2v, ifft, fft, v2s, snk_out)
        

def main():
    fg = my_graph()
    fg.start()
    fg.wait()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
    
