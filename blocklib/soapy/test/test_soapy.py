from gnuradio import gr, soapy, blocks, streamops

samp_rate = 250000

src = soapy.source_c('driver=rtlsdr',1)
src.set_sample_rate(0, samp_rate)
src.set_gain_mode(0, False)
src.set_frequency(0, 90500000)
src.set_frequency_correction(0, 0)
src.set_gain(0, 'TUNER', 40)

hd = streamops.head(100000)
snk = blocks.vector_sink_c()

fg = gr.flowgraph()

fg.connect(src,0,hd,0)
fg.connect(hd,0,snk,0)

fg.start()
fg.wait()

from matplotlib import pyplot as plt
import numpy as np
plt.plot(np.real(snk.data()))
plt.plot(np.imag(snk.data()))
plt.show()