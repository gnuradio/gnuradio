#!/usr/bin/env python

from gnuradio import gr
import scipy

def main():
    data = scipy.arange(0, 32000, 1).tolist()
    trig = 100*[0,] + 100*[1,]

    src = gr.vector_source_s(data, True)
    trigger = gr.vector_source_s(trig, True)

    thr = gr.throttle(gr.sizeof_short, 10e3)
    ann = gr.annotator_alltoall(1000000, gr.sizeof_short)
    tagger = gr.burst_tagger(gr.sizeof_short)

    fsnk = gr.tagged_file_sink(gr.sizeof_short)

    tb = gr.top_block()
    tb.connect(src, thr, (tagger, 0))
    tb.connect(trigger, (tagger, 1))
    tb.connect(tagger, fsnk)

    tb.run()

if __name__ == "__main__":
    main()

    
