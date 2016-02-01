#!/usr/bin/env python
#
# Copyright 2009,2010,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import optfir, math

from gnuradio import gr, fft
import filter_swig as filter

try:
    from gnuradio import blocks
except ImportError:
    import blocks_swig as blocks

class channelizer_ccf(gr.hier_block2):
    '''
    Make a Polyphase Filter channelizer (complex in, complex out, floating-point taps)

    This simplifies the interface by allowing a single input stream to connect to this block.
    It will then output a stream for each channel.
    '''
    def __init__(self, numchans, taps=None, oversample_rate=1, atten=100):
        gr.hier_block2.__init__(self, "pfb_channelizer_ccf",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(numchans, numchans, gr.sizeof_gr_complex))

        self._nchans = numchans
        self._oversample_rate = oversample_rate

        if (taps is not None) and (len(taps) > 0):
            self._taps = taps
        else:
            # Create a filter that covers the full bandwidth of the input signal
            bw = 0.4
            tb = 0.2
            ripple = 0.1
            made = False
            while not made:
                try:
                    self._taps = optfir.low_pass(1, self._nchans, bw, bw+tb, ripple, atten)
                    made = True
                except RuntimeError:
                    ripple += 0.01
                    made = False
                    print("Warning: set ripple to %.4f dB. If this is a problem, adjust the attenuation or create your own filter taps." % (ripple))

                    # Build in an exit strategy; if we've come this far, it ain't working.
                    if(ripple >= 1.0):
                        raise RuntimeError("optfir could not generate an appropriate filter.")

        self.s2ss = blocks.stream_to_streams(gr.sizeof_gr_complex, self._nchans)
        self.pfb = filter.pfb_channelizer_ccf(self._nchans, self._taps,
                                              self._oversample_rate)
        self.connect(self, self.s2ss)

        for i in xrange(self._nchans):
            self.connect((self.s2ss,i), (self.pfb,i))
            self.connect((self.pfb,i), (self,i))

    def set_channel_map(self, newmap):
        self.pfb.set_channel_map(newmap)

    def set_taps(self, taps):
        self.pfb.set_taps(taps)

    def taps(self):
        return self.pfb.taps()

    def declare_sample_delay(self, delay):
        self.pfb.declare_sample_delay(delay)


class interpolator_ccf(gr.hier_block2):
    '''
    Make a Polyphase Filter interpolator (complex in, complex out, floating-point taps)

    The block takes a single complex stream in and outputs a single complex
    stream out. As such, it requires no extra glue to handle the input/output
    streams. This block is provided to be consistent with the interface to the
    other PFB block.
    '''
    def __init__(self, interp, taps=None, atten=100):
        gr.hier_block2.__init__(self, "pfb_interpolator_ccf",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(1, 1, gr.sizeof_gr_complex))

        self._interp = interp
        self._taps = taps

        if (taps is not None) and (len(taps) > 0):
            self._taps = taps
        else:
            # Create a filter that covers the full bandwidth of the input signal
            bw = 0.4
            tb = 0.2
            ripple = 0.99
            made = False
            while not made:
                try:
                    self._taps = optfir.low_pass(self._interp, self._interp, bw, bw+tb, ripple, atten)
                    made = True
                except RuntimeError:
                    ripple += 0.01
                    made = False
                    print("Warning: set ripple to %.4f dB. If this is a problem, adjust the attenuation or create your own filter taps." % (ripple))

                    # Build in an exit strategy; if we've come this far, it ain't working.
                    if(ripple >= 1.0):
                        raise RuntimeError("optfir could not generate an appropriate filter.")

        self.pfb = filter.pfb_interpolator_ccf(self._interp, self._taps)

        self.connect(self, self.pfb)
        self.connect(self.pfb, self)

    def set_taps(self, taps):
        self.pfb.set_taps(taps)

    def declare_sample_delay(self, delay):
        self.pfb.declare_sample_delay(delay)


class decimator_ccf(gr.hier_block2):
    '''
    Make a Polyphase Filter decimator (complex in, complex out, floating-point taps)

    This simplifies the interface by allowing a single input stream to connect to this block.
    It will then output a stream that is the decimated output stream.
    '''
    def __init__(self, decim, taps=None, channel=0, atten=100,
                 use_fft_rotators=True, use_fft_filters=True):
	gr.hier_block2.__init__(self, "pfb_decimator_ccf",
				gr.io_signature(1, 1, gr.sizeof_gr_complex),
				gr.io_signature(1, 1, gr.sizeof_gr_complex))

        self._decim = decim
        self._channel = channel

        if (taps is not None) and (len(taps) > 0):
            self._taps = taps
        else:
            # Create a filter that covers the full bandwidth of the input signal
            bw = 0.4
            tb = 0.2
            ripple = 0.1
            made = False
            while not made:
                try:
                    self._taps = optfir.low_pass(1, self._decim, bw, bw+tb, ripple, atten)
                    made = True
                except RuntimeError:
                    ripple += 0.01
                    made = False
                    print("Warning: set ripple to %.4f dB. If this is a problem, adjust the attenuation or create your own filter taps." % (ripple))

                    # Build in an exit strategy; if we've come this far, it ain't working.
                    if(ripple >= 1.0):
                        raise RuntimeError("optfir could not generate an appropriate filter.")

        self.s2ss = blocks.stream_to_streams(gr.sizeof_gr_complex, self._decim)
        self.pfb = filter.pfb_decimator_ccf(self._decim, self._taps, self._channel,
                                            use_fft_rotators, use_fft_filters)

        self.connect(self, self.s2ss)

        for i in xrange(self._decim):
            self.connect((self.s2ss,i), (self.pfb,i))

        self.connect(self.pfb, self)

    def set_taps(self, taps):
        self.pfb.set_taps(taps)

    def set_channel(self, chan):
        self.pfb.set_channel(chan)

    def declare_sample_delay(self, delay):
        self.pfb.declare_sample_delay(delay)


class arb_resampler_ccf(gr.hier_block2):
    '''
    Convenience wrapper for the polyphase filterbank arbitrary resampler.

    The block takes a single complex stream in and outputs a single complex
    stream out. As such, it requires no extra glue to handle the input/output
    streams. This block is provided to be consistent with the interface to the
    other PFB block.
    '''
    def __init__(self, rate, taps=None, flt_size=32, atten=100):
        gr.hier_block2.__init__(self, "pfb_arb_resampler_ccf",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        self._rate = rate
        self._size = flt_size

        if (taps is not None) and (len(taps) > 0):
            self._taps = taps
        else:
            # Create a filter that covers the full bandwidth of the output signal

            # If rate >= 1, we need to prevent images in the output,
            # so we have to filter it to less than half the channel
            # width of 0.5.  If rate < 1, we need to filter to less
            # than half the output signal's bw to avoid aliasing, so
            # the half-band here is 0.5*rate.
            percent = 0.80
            if(self._rate < 1):
                halfband = 0.5*self._rate
                bw = percent*halfband
                tb = (percent/2.0)*halfband
                ripple = 0.1

                # As we drop the bw factor, the optfir filter has a harder time converging;
                # using the firdes method here for better results.
                self._taps = filter.firdes.low_pass_2(self._size, self._size, bw, tb, atten,
                                                      filter.firdes.WIN_BLACKMAN_HARRIS)
            else:
                halfband = 0.5
                bw = percent*halfband
                tb = (percent/2.0)*halfband
                ripple = 0.1

                made = False
                while not made:
                    try:
                        self._taps = optfir.low_pass(self._size, self._size, bw, bw+tb, ripple, atten)
                        made = True
                    except RuntimeError:
                        ripple += 0.01
                        made = False
                        print("Warning: set ripple to %.4f dB. If this is a problem, adjust the attenuation or create your own filter taps." % (ripple))

                        # Build in an exit strategy; if we've come this far, it ain't working.
                        if(ripple >= 1.0):
                            raise RuntimeError("optfir could not generate an appropriate filter.")

        self.pfb = filter.pfb_arb_resampler_ccf(self._rate, self._taps, self._size)
        #print "PFB has %d taps\n" % (len(self._taps),)

        self.connect(self, self.pfb)
        self.connect(self.pfb, self)

    # Note -- set_taps not implemented in base class yet
    def set_taps(self, taps):
        self.pfb.set_taps(taps)

    def set_rate(self, rate):
        self.pfb.set_rate(rate)

    def declare_sample_delay(self, delay):
        self.pfb.declare_sample_delay(delay)

class arb_resampler_fff(gr.hier_block2):
    '''
    Convenience wrapper for the polyphase filterbank arbitrary resampler.

    The block takes a single float stream in and outputs a single float
    stream out. As such, it requires no extra glue to handle the input/output
    streams. This block is provided to be consistent with the interface to the
    other PFB block.
    '''
    def __init__(self, rate, taps=None, flt_size=32, atten=100):
        gr.hier_block2.__init__(self, "pfb_arb_resampler_fff",
                                gr.io_signature(1, 1, gr.sizeof_float), # Input signature
                                gr.io_signature(1, 1, gr.sizeof_float)) # Output signature

        self._rate = rate
        self._size = flt_size

        if (taps is not None) and (len(taps) > 0):
            self._taps = taps
        else:
            # Create a filter that covers the full bandwidth of the input signal

            # If rate >= 1, we need to prevent images in the output,
            # so we have to filter it to less than half the channel
            # width of 0.5.  If rate < 1, we need to filter to less
            # than half the output signal's bw to avoid aliasing, so
            # the half-band here is 0.5*rate.
            percent = 0.80
            if(self._rate < 1):
                halfband = 0.5*self._rate
                bw = percent*halfband
                tb = (percent/2.0)*halfband
                ripple = 0.1

                # As we drop the bw factor, the optfir filter has a harder time converging;
                # using the firdes method here for better results.
                self._taps = filter.firdes.low_pass_2(self._size, self._size, bw, tb, atten,
                                                      filter.firdes.WIN_BLACKMAN_HARRIS)
            else:
                halfband = 0.5
                bw = percent*halfband
                tb = (percent/2.0)*halfband
                ripple = 0.1

                made = False
                while not made:
                    try:
                        self._taps = optfir.low_pass(self._size, self._size, bw, bw+tb, ripple, atten)
                        made = True
                    except RuntimeError:
                        ripple += 0.01
                        made = False
                        print("Warning: set ripple to %.4f dB. If this is a problem, adjust the attenuation or create your own filter taps." % (ripple))

                        # Build in an exit strategy; if we've come this far, it ain't working.
                        if(ripple >= 1.0):
                            raise RuntimeError("optfir could not generate an appropriate filter.")

        self.pfb = filter.pfb_arb_resampler_fff(self._rate, self._taps, self._size)
        #print "PFB has %d taps\n" % (len(self._taps),)

        self.connect(self, self.pfb)
        self.connect(self.pfb, self)

    # Note -- set_taps not implemented in base class yet
    def set_taps(self, taps):
        self.pfb.set_taps(taps)

    def set_rate(self, rate):
        self.pfb.set_rate(rate)

    def declare_sample_delay(self, delay):
        self.pfb.declare_sample_delay(delay)

class arb_resampler_ccc(gr.hier_block2):
    '''
    Convenience wrapper for the polyphase filterbank arbitrary resampler.

    The block takes a single complex stream in and outputs a single complex
    stream out. As such, it requires no extra glue to handle the input/output
    streams. This block is provided to be consistent with the interface to the
    other PFB block.
    '''
    def __init__(self, rate, taps=None, flt_size=32, atten=100):
        gr.hier_block2.__init__(self, "pfb_arb_resampler_ccc",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        self._rate = rate
        self._size = flt_size

        if (taps is not None) and (len(taps) > 0):
            self._taps = taps
        else:
            # Create a filter that covers the full bandwidth of the input signal
            bw = 0.4
            tb = 0.2
            ripple = 0.1
            #self._taps = filter.firdes.low_pass_2(self._size, self._size, bw, tb, atten)
            made = False
            while not made:
                try:
                    self._taps = optfir.low_pass(self._size, self._size, bw, bw+tb, ripple, atten)
                    made = True
                except RuntimeError:
                    ripple += 0.01
                    made = False
                    print("Warning: set ripple to %.4f dB. If this is a problem, adjust the attenuation or create your own filter taps." % (ripple))

                    # Build in an exit strategy; if we've come this far, it ain't working.
                    if(ripple >= 1.0):
                        raise RuntimeError("optfir could not generate an appropriate filter.")

        self.pfb = filter.pfb_arb_resampler_ccc(self._rate, self._taps, self._size)
        #print "PFB has %d taps\n" % (len(self._taps),)

        self.connect(self, self.pfb)
        self.connect(self.pfb, self)

    # Note -- set_taps not implemented in base class yet
    def set_taps(self, taps):
        self.pfb.set_taps(taps)

    def set_rate(self, rate):
        self.pfb.set_rate(rate)

    def declare_sample_delay(self, delay):
        self.pfb.declare_sample_delay(delay)


class channelizer_hier_ccf(gr.hier_block2):
    """
    Make a Polyphase Filter channelizer (complex in, complex out, floating-point taps)

    Args:
    n_chans: The number of channels to split into.
    n_filterbanks: The number of filterbank blocks to use (default=2).
    taps: The taps to use.  If this is `None` then taps are generated using optfir.low_pass.
    outchans: Which channels to output streams for (a list of integers) (default is all channels).
    atten: Stop band attenuation.
    bw: The fraction of the channel you want to keep.
    tb: Transition band with as fraction of channel width.
    ripple: Pass band ripple in dB.
    """

    def __init__(self, n_chans, n_filterbanks=1, taps=None, outchans=None,
                 atten=100, bw=1.0, tb=0.2, ripple=0.1):
        if n_filterbanks > n_chans:
            n_filterbanks = n_chans
        if outchans is None:
            outchans = range(n_chans)
        gr.hier_block2.__init__(
            self, "pfb_channelizer_hier_ccf",
            gr.io_signature(1, 1, gr.sizeof_gr_complex),
            gr.io_signature(len(outchans), len(outchans), gr.sizeof_gr_complex))
        if taps is None:
            taps = optfir.low_pass(1, n_chans, bw, bw+tb, ripple, atten)
        taps = list(taps)
        extra_taps = int(math.ceil(1.0*len(taps)/n_chans)*n_chans - len(taps))
        taps = taps + [0] * extra_taps
        # Make taps for each channel
        chantaps = [list(reversed(taps[i: len(taps): n_chans])) for i in range(0, n_chans)]
        # Convert the input stream into a stream of vectors.
        self.s2v = blocks.stream_to_vector(gr.sizeof_gr_complex, n_chans)
        # Create a mapping to separate out each filterbank (a group of channels to be processed together)
        # And a list of sets of taps for each filterbank.
        low_cpp = int(n_chans/n_filterbanks)
        extra = n_chans - low_cpp*n_filterbanks
        cpps = [low_cpp+1]*extra + [low_cpp]*(n_filterbanks-extra)
        splitter_mapping = []
        filterbanktaps = []
        total = 0
        for cpp in cpps:
            splitter_mapping.append([(0, i) for i in range(total, total+cpp)])
            filterbanktaps.append(chantaps[total: total+cpp])
            total += cpp
        assert(total == n_chans)
        # Split the stream of vectors in n_filterbanks streams of vectors.
        self.splitter = blocks.vector_map(gr.sizeof_gr_complex, [n_chans], splitter_mapping)
        # Create the filterbanks
        self.fbs = [filter.filterbank_vcvcf(taps) for taps in filterbanktaps]
        # Combine the streams of vectors back into a single stream of vectors.
        combiner_mapping = [[]]
        for i, cpp in enumerate(cpps):
            for j in range(cpp):
                combiner_mapping[0].append((i, j))
        self.combiner = blocks.vector_map(gr.sizeof_gr_complex, cpps, combiner_mapping)
        # Add the final FFT to the channelizer.
        self.fft = fft.fft_vcc(n_chans, forward=True, window=[1.0]*n_chans)
        # Select the desired channels
        if outchans != range(n_chans):
            selector_mapping = [[(0, i) for i in outchans]]
            self.selector = blocks.vector_map(gr.sizeof_gr_complex, [n_chans], selector_mapping)
        # Convert stream of vectors to a normal stream.
        self.v2ss = blocks.vector_to_streams(gr.sizeof_gr_complex, len(outchans))
        self.connect(self, self.s2v, self.splitter)
        for i in range(0, n_filterbanks):
            self.connect((self.splitter, i), self.fbs[i], (self.combiner, i))
        self.connect(self.combiner, self.fft)
        if outchans != range(n_chans):
            self.connect(self.fft, self.selector, self.v2ss)
        else:
            self.connect(self.fft, self.v2ss)
        for i in range(0, len(outchans)):
            self.connect((self.v2ss, i), (self, i))
