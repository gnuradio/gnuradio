from gnuradio import gr, soapy, blocks, streamops

samp_rate = 250000

src = soapy.source_cc('driver=rtlsdr',1)
src.set_sample_rate(0, samp_rate)
src.set_gain_mode(0, False)
src.set_frequency(0, 99500000)
src.set_frequency_correction(0, 0)
src.set_gain(0, 'TUNER', 20)

hd = streamops.head(gr.sizeof_gr_complex,100000)
snk = blocks.null_sink(gr.sizeof_gr_complex)

fg = gr.flowgraph()

fg.connect(src,0,hd,0)
fg.connect(hd,0,snk,0)

fg.validate()

fg.start()
fg.wait()