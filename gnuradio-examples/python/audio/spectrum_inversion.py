#!/usr/bin/env python
#
# Gang - Here's a simple script that demonstrates spectrum inversion
# using the multiply by [1,-1] method (mixing with Nyquist frequency).
# Requires nothing but a sound card, and sounds just like listening
# to a SSB signal on the wrong sideband.
#

from gnuradio import gr
from gnuradio import audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser

class my_graph(gr.flow_graph):

    def __init__(self):
        gr.flow_graph.__init__(self)

        parser = OptionParser(option_class=eng_option)
        parser.add_option("-I", "--audio-input", type="string", default="",
                          help="pcm input device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-r", "--sample-rate", type="eng_float", default=8000,
                          help="set sample rate to RATE (8000)")
        (options, args) = parser.parse_args ()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        sample_rate = int(options.sample_rate)
        src = audio.source (sample_rate, options.audio_input)
        dst = audio.sink (sample_rate, options.audio_output)

	vec1 = [1, -1]
	vsource = gr.vector_source_f(vec1, True)
	multiply = gr.multiply_ff()

	self.connect(src, (multiply, 0))
	self.connect(vsource, (multiply, 1))
	self.connect(multiply, dst)


if __name__ == '__main__':
    try:
        my_graph().run()
    except KeyboardInterrupt:
        pass
