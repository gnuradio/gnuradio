#!/usr/bin/env python

from math import pi
from gnuradio import gr, gru, usrp, optfir, audio, eng_notation, blks, pager
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from string import split, join, printable

class usrp_source_c(gr.hier_block):
    """
    Create a USRP source object supplying complex floats.
    
    Selects user supplied subdevice or chooses first available one.

    Calibration value is the offset from the tuned frequency to 
    the actual frequency.       
    """
    def __init__(self, fg, subdev_spec, decim, gain=None, calibration=0.0):
        self._decim = decim
        self._src = usrp.source_c()
        if subdev_spec is None:
            subdev_spec = usrp.pick_rx_subdevice(self._src)
        self._subdev = usrp.selected_subdev(self._src, subdev_spec)
        self._src.set_mux(usrp.determine_rx_mux_value(self._src, subdev_spec))
        self._src.set_decim_rate(self._decim)

        # If no gain specified, set to midrange
        if gain is None:
            g = self._subdev.gain_range()
            gain = (g[0]+g[1])/2.0

        self._subdev.set_gain(gain)
        self._cal = calibration

        gr.hier_block.__init__(self, fg, self._src, self._src)

    def tune(self, freq):
        result = usrp.tune(self._src, 0, self._subdev, freq+self._cal)
        # TODO: deal with residual

    def rate(self):
        return self._src.adc_rate()/self._decim

class channelizer(blks.analysis_filterbank):
    def __init__(self,
		 fg,		# Flow graph
		 if_rate,	# IF input sample rate (complex)
		 channel_rate,	# Final channel sample rate (complex)
		 channel_pass,	# Occupied spectrum for narrowband channel
		 channel_stop):	# Total channel + guard band

        num_channels = int(if_rate/channel_rate)
        taps = optfir.low_pass(1.0, if_rate, channel_pass, channel_stop, 0.1, 60)
        blks.analysis_filterbank.__init__(self, fg, num_channels, taps)

class app_flow_graph(gr.flow_graph):
    def __init__(self, options, args, queue):
	gr.flow_graph.__init__(self)
	self.options = options
	self.args = args

	# FIXME: Parameterize
	#
	# Difference between upper and lower must evenly divide into USRP sample rate
	# and also must be divisible by 25000
	options.channel_rate =     25000
        options.channel_pass =      8000
        options.channel_stop =     10000
        	
	if_rate = options.upper_freq - options.lower_freq
	center_freq = options.lower_freq + if_rate/2
	num_channels = int(if_rate/options.channel_rate)
	decim = int(64000000/if_rate)
		
	self.SRC = usrp_source_c(self, options.rx_board, decim, options.gain, options.calibration)
        self.CHAN = channelizer(self, if_rate, options.channel_rate, options.channel_pass, options.channel_stop)

        self.connect(self.SRC, self.CHAN)
	for i in range(num_channels):
	    freq = options.lower_freq + i*options.channel_rate
	    if freq > center_freq:
		freq -= if_rate/2
	    else:
		freq += if_rate/2
            FLEX = pager.flex_demod(self, options.channel_rate, queue)
	    self.connect((self.CHAN, i), FLEX.INPUT)

	self.SRC.tune(center_freq)

def make_printable(data):
    return "".join(char for char in data if char in printable)

def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("",   "--upper-freq",   type="eng_float", help="lower Rx frequency", metavar="LOWER")
    parser.add_option("",   "--lower-freq",   type="eng_float", help="upper Rx frequency", metavar="UPPER")
    parser.add_option("-R", "--rx-board",     type="subdev", help="select USRP Rx side A or B (default=first daughterboard found)", metavar="SIDE")
    parser.add_option("-c", "--calibration",  type="eng_float", default=0.0, help="set frequency offset to Hz", metavar="Hz")
    parser.add_option("-g", "--gain", 	    type="int", help="set RF gain", metavar="dB")
    (options, args) = parser.parse_args()

    # FIXME: parameter sanity checks

    queue = gr.msg_queue()
    fg = app_flow_graph(options, args, queue)
    try:
        fg.start()
	while 1:
	    msg = queue.delete_head() # Blocking read
	    fields = split(msg.to_string(), chr(128))
	    print join([make_printable(field) for field in fields], '|')

    except KeyboardInterrupt:
        fg.stop()

if __name__ == "__main__":
    main()
