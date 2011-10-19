#
# Copyright 2005,2006 Free Software Foundation, Inc.
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

from gnuradio import gr
from gnuradio.blks2impl.fm_emph import fm_deemph
import math

class wfm_rcv_fmdet(gr.hier_block2):
    def __init__ (self, demod_rate, audio_decimation):
        """
        Hierarchical block for demodulating a broadcast FM signal.
        
        The input is the downconverted complex baseband signal
        (gr_complex).  The output is two streams of the demodulated
        audio (float) 0=Left, 1=Right.
        
        @param demod_rate: input sample rate of complex baseband input.
        @type demod_rate: float
        @param audio_decimation: how much to decimate demod_rate to get to audio.
        @type audio_decimation: integer
        """
	gr.hier_block2.__init__(self, "wfm_rcv_fmdet",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(2, 2, gr.sizeof_float))      # Output signature
        lowfreq = -125e3/demod_rate
	highfreq = 125e3/demod_rate
        audio_rate = demod_rate / audio_decimation

        # We assign to self so that outsiders can grab the demodulator
        # if they need to.  E.g., to plot its output.
        #
        # input: complex; output: float
        
        self.fm_demod = gr.fmdet_cf (demod_rate, lowfreq, highfreq, 0.05)

        # input: float; output: float
        self.deemph_Left  = fm_deemph (audio_rate)
        self.deemph_Right = fm_deemph (audio_rate)
        
        # compute FIR filter taps for audio filter
        width_of_transition_band = audio_rate / 32
        audio_coeffs = gr.firdes.low_pass (1.0 ,         # gain
                                           demod_rate,      # sampling rate
                                           15000 ,
                                           width_of_transition_band,
                                           gr.firdes.WIN_HAMMING)

        # input: float; output: float
        self.audio_filter = gr.fir_filter_fff (audio_decimation, audio_coeffs)
        if 1:
            # Pick off the stereo carrier/2 with this filter. It
            # attenuated 10 dB so apply 10 dB gain We pick off the
            # negative frequency half because we want to base band by
            # it!
            ##  NOTE THIS WAS HACKED TO OFFSET INSERTION LOSS DUE TO
            ##  DEEMPHASIS

            stereo_carrier_filter_coeffs = gr.firdes.complex_band_pass(10.0,
                                                                       demod_rate,
                                                                       -19020,
                                                                       -18980,
                                                                       width_of_transition_band,
                                                                       gr.firdes.WIN_HAMMING)
            
            #print "len stereo carrier filter = ",len(stereo_carrier_filter_coeffs)
            #print "stereo carrier filter ", stereo_carrier_filter_coeffs
            #print "width of transition band = ",width_of_transition_band, " audio rate = ", audio_rate

            # Pick off the double side band suppressed carrier
            # Left-Right audio. It is attenuated 10 dB so apply 10 dB
            # gain

            stereo_dsbsc_filter_coeffs = gr.firdes.complex_band_pass(20.0,
                                                                     demod_rate,
                                                                     38000-15000/2,
                                                                     38000+15000/2,
                                                                     width_of_transition_band,
                                                                     gr.firdes.WIN_HAMMING)
            #print "len stereo dsbsc filter = ",len(stereo_dsbsc_filter_coeffs)
            #print "stereo dsbsc filter ", stereo_dsbsc_filter_coeffs

            # construct overlap add filter system from coefficients
            # for stereo carrier
            self.stereo_carrier_filter = gr.fir_filter_fcc(audio_decimation,
                                                           stereo_carrier_filter_coeffs)

            # carrier is twice the picked off carrier so arrange to do
            # a commplex multiply
            self.stereo_carrier_generator = gr.multiply_cc();

            # Pick off the rds signal
            stereo_rds_filter_coeffs = gr.firdes.complex_band_pass(30.0,
                                                                   demod_rate,
                                                                   57000 - 1500,
                                                                   57000 + 1500,
                                                                   width_of_transition_band,
                                                                   gr.firdes.WIN_HAMMING)
            #print "len stereo dsbsc filter = ",len(stereo_dsbsc_filter_coeffs)
            #print "stereo dsbsc filter ", stereo_dsbsc_filter_coeffs
            # construct overlap add filter system from coefficients for stereo carrier

	    self.rds_signal_filter = gr.fir_filter_fcc(audio_decimation,
                                                       stereo_rds_filter_coeffs)
	    self.rds_carrier_generator = gr.multiply_cc();
	    self.rds_signal_generator = gr.multiply_cc();
	    self_rds_signal_processor = gr.null_sink(gr.sizeof_gr_complex);

            loop_bw = 2*math.pi/100.0
            max_freq = -2.0*math.pi*18990/audio_rate;
            min_freq = -2.0*math.pi*19010/audio_rate;          
            self.stereo_carrier_pll_recovery = gr.pll_refout_cc(loop_bw,
                                                                max_freq,
                                                                min_freq);

            #self.stereo_carrier_pll_recovery.squelch_enable(False)
            ##pll_refout does not have squelch yet, so disabled for
            #now
            
            # set up mixer (multiplier) to get the L-R signal at
            # baseband

            self.stereo_basebander = gr.multiply_cc();

            # pick off the real component of the basebanded L-R
            # signal.  The imaginary SHOULD be zero

            self.LmR_real = gr.complex_to_real();
            self.Make_Left = gr.add_ff();
            self.Make_Right = gr.sub_ff();
            
            self.stereo_dsbsc_filter = gr.fir_filter_fcc(audio_decimation,
                                                         stereo_dsbsc_filter_coeffs)


        if 1:

            # send the real signal to complex filter to pick off the
            # carrier and then to one side of a multiplier
            self.connect (self, self.fm_demod, self.stereo_carrier_filter,
                          self.stereo_carrier_pll_recovery, 
                          (self.stereo_carrier_generator,0))

            # send the already filtered carrier to the otherside of the carrier
            # the resulting signal from this multiplier is the carrier
            # with correct phase but at -38000 Hz.
            self.connect (self.stereo_carrier_pll_recovery, (self.stereo_carrier_generator,1))

            # send the new carrier to one side of the mixer (multiplier)
            self.connect (self.stereo_carrier_generator, (self.stereo_basebander,0))

            # send the demphasized audio to the DSBSC pick off filter,  the complex
            # DSBSC signal at +38000 Hz is sent to the other side of the mixer/multiplier
            # the result is BASEBANDED DSBSC with phase zero!
            self.connect (self.fm_demod,self.stereo_dsbsc_filter, (self.stereo_basebander,1))

            # Pick off the real part since the imaginary is
            # theoretically zero and then to one side of a summer
            self.connect (self.stereo_basebander, self.LmR_real, (self.Make_Left,0))

            #take the same real part of the DSBSC baseband signal and
            #send it to negative side of a subtracter
            self.connect (self.LmR_real,(self.Make_Right,1))

	    # Make rds carrier by taking the squared pilot tone and
	    # multiplying by pilot tone
	    self.connect (self.stereo_basebander,(self.rds_carrier_generator,0))
            self.connect (self.stereo_carrier_pll_recovery,(self.rds_carrier_generator,1)) 

	    # take signal, filter off rds, send into mixer 0 channel
	    self.connect (self.fm_demod,self.rds_signal_filter,(self.rds_signal_generator,0))

            # take rds_carrier_generator output and send into mixer 1
            # channel
	    self.connect (self.rds_carrier_generator,(self.rds_signal_generator,1))

	    # send basebanded rds signal and send into "processor"
	    # which for now is a null sink
	    self.connect (self.rds_signal_generator,self_rds_signal_processor)
	    

        if 1:
            # pick off the audio, L+R that is what we used to have and
            # send it to the summer
            self.connect(self.fm_demod, self.audio_filter, (self.Make_Left, 1))

            # take the picked off L+R audio and send it to the PLUS
            # side of the subtractor
            self.connect(self.audio_filter,(self.Make_Right, 0))

            # The result of  Make_Left  gets    (L+R) +  (L-R) and results in 2*L
            # The result of Make_Right gets  (L+R) - (L-R) and results in 2*R
            self.connect(self.Make_Left , self.deemph_Left, (self, 0))
            self.connect(self.Make_Right, self.deemph_Right, (self, 1))

        # NOTE: mono support will require variable number of outputs in hier_block2s
        # See ticket:174 in Trac database
        #else:
        #    self.connect (self.fm_demod, self.audio_filter, self)
