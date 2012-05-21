gnuradio.gr
===========

.. automodule:: gnuradio.gr

Signal Processing Blocks
------------------------

Top Block and Hierarchical Block Base Classes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:
   
   gnuradio.gr.top_block
   gnuradio.gr.hier_block2

Signal Sources
^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.glfsr_source_b
   gnuradio.gr.glfsr_source_f
   gnuradio.gr.lfsr_32k_source_s
   gnuradio.gr.null_source
   gnuradio.gr.noise_source_c
   gnuradio.gr.noise_source_f
   gnuradio.gr.noise_source_i
   gnuradio.gr.noise_source_s
   gnuradio.gr.sig_source_c
   gnuradio.gr.sig_source_f
   gnuradio.gr.sig_source_i
   gnuradio.gr.sig_source_s
   gnuradio.gr.vector_source_b
   gnuradio.gr.vector_source_c
   gnuradio.gr.vector_source_f
   gnuradio.gr.vector_source_i
   gnuradio.gr.vector_source_s
   gnuradio.gr.file_descriptor_source
   gnuradio.gr.file_source
   gnuradio.gr.message_source
   gnuradio.gr.udp_source
   gnuradio.gr.wavfile_source

Signal Sinks
^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.bin_statistics_f
   gnuradio.gr.check_counting_s
   gnuradio.gr.check_lfsr_32k_s
   gnuradio.gr.framer_sink_1
   gnuradio.gr.null_sink
   gnuradio.gr.packet_sink
   gnuradio.gr.probe_avg_mag_sqrd_c
   gnuradio.gr.probe_avg_mag_sqrd_cf
   gnuradio.gr.probe_avg_mag_sqrd_f
   gnuradio.gr.probe_signal_f
   gnuradio.gr.vector_sink_b
   gnuradio.gr.vector_sink_c
   gnuradio.gr.vector_sink_f
   gnuradio.gr.vector_sink_i
   gnuradio.gr.vector_sink_s
   gnuradio.gr.file_descriptor_sink
   gnuradio.gr.file_sink
   gnuradio.gr.histo_sink_f
   gnuradio.gr.message_sink
   gnuradio.gr.oscope_sink_f
   gnuradio.gr.udp_sink
   gnuradio.gr.wavfile_sink

Filters
^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.fft_filter_ccc
   gnuradio.gr.fft_filter_fff
   gnuradio.gr.filter_delay_fc
   gnuradio.gr.fir_filter_ccc
   gnuradio.gr.fir_filter_ccf
   gnuradio.gr.fir_filter_fcc
   gnuradio.gr.fir_filter_fff
   gnuradio.gr.fir_filter_fsf
   gnuradio.gr.fir_filter_scc
   gnuradio.gr.fractional_interpolator_cc
   gnuradio.gr.fractional_interpolator_ff
   gnuradio.gr.freq_xlating_fir_filter_ccc
   gnuradio.gr.freq_xlating_fir_filter_ccf
   gnuradio.gr.freq_xlating_fir_filter_fcc
   gnuradio.gr.freq_xlating_fir_filter_fcf
   gnuradio.gr.freq_xlating_fir_filter_scc
   gnuradio.gr.freq_xlating_fir_filter_scf
   gnuradio.gr.hilbert_fc
   gnuradio.gr.iir_filter_ffd
   gnuradio.gr.interp_fir_filter_ccc
   gnuradio.gr.interp_fir_filter_ccf
   gnuradio.gr.interp_fir_filter_fcc
   gnuradio.gr.interp_fir_filter_fff
   gnuradio.gr.interp_fir_filter_fsf
   gnuradio.gr.interp_fir_filter_scc
   gnuradio.gr.rational_resampler_base_ccc
   gnuradio.gr.rational_resampler_base_ccf
   gnuradio.gr.rational_resampler_base_fcc
   gnuradio.gr.rational_resampler_base_fff
   gnuradio.gr.rational_resampler_base_fsf
   gnuradio.gr.rational_resampler_base_scc
   gnuradio.gr.single_pole_iir_filter_cc
   gnuradio.gr.single_pole_iir_filter_ff
   gnuradio.gr.moving_average_cc
   gnuradio.gr.moving_average_ff
   gnuradio.gr.moving_average_ii
   gnuradio.gr.moving_average_ss
   gnuradio.gr.pfb_arb_resampler_ccf
   gnuradio.gr.pfb_channelizer_ccf
   gnuradio.gr.pfb_clock_sync_ccf
   gnuradio.gr.pfb_clock_sync_fff
   gnuradio.gr.pfb_decimator_ccf
   gnuradio.gr.pfb_interpolator_ccf

Mathematics
^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.conjugate_cc
   gnuradio.gr.nlog10_ff
   gnuradio.gr.rms_cf
   gnuradio.gr.rms_ff
   gnuradio.gr.add_cc
   gnuradio.gr.add_const_cc
   gnuradio.gr.add_const_ff
   gnuradio.gr.add_const_ii
   gnuradio.gr.add_const_sf
   gnuradio.gr.add_const_ss
   gnuradio.gr.add_const_vcc
   gnuradio.gr.add_const_vff
   gnuradio.gr.add_const_vii
   gnuradio.gr.add_const_vss
   gnuradio.gr.add_ff
   gnuradio.gr.add_ii
   gnuradio.gr.add_ss
   gnuradio.gr.and_bb
   gnuradio.gr.and_const_bb
   gnuradio.gr.and_const_ii
   gnuradio.gr.and_const_ss
   gnuradio.gr.and_ii
   gnuradio.gr.and_ss
   gnuradio.gr.divide_cc
   gnuradio.gr.divide_ff
   gnuradio.gr.divide_ii
   gnuradio.gr.divide_ss
   gnuradio.gr.integrate_cc
   gnuradio.gr.integrate_ff
   gnuradio.gr.integrate_ii
   gnuradio.gr.integrate_ss
   gnuradio.gr.multiply_cc
   gnuradio.gr.multiply_const_cc
   gnuradio.gr.multiply_const_ff
   gnuradio.gr.multiply_const_ii
   gnuradio.gr.multiply_const_ss
   gnuradio.gr.multiply_const_vcc
   gnuradio.gr.multiply_const_vff
   gnuradio.gr.multiply_const_vii
   gnuradio.gr.multiply_const_vss
   gnuradio.gr.multiply_ff
   gnuradio.gr.multiply_ii
   gnuradio.gr.multiply_ss
   gnuradio.gr.not_bb
   gnuradio.gr.not_ii
   gnuradio.gr.not_ss
   gnuradio.gr.or_bb
   gnuradio.gr.or_ii
   gnuradio.gr.or_ss
   gnuradio.gr.sub_cc
   gnuradio.gr.sub_ff
   gnuradio.gr.sub_ii
   gnuradio.gr.sub_ss
   gnuradio.gr.xor_bb
   gnuradio.gr.xor_ii
   gnuradio.gr.xor_ss

Modulation
^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.cpfsk_bc
   gnuradio.gr.frequency_modulator_fc
   gnuradio.gr.phase_modulator_fc

Demodulation
^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.quadrature_demod_cf

Information Coding and Decoding
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.additive_scrambler_bb
   gnuradio.gr.descrambler_bb
   gnuradio.gr.diff_decoder_bb
   gnuradio.gr.diff_encoder_bb
   gnuradio.gr.fake_channel_encoder_pp
   gnuradio.gr.fake_channel_decoder_pp
   gnuradio.gr.map_bb
   gnuradio.gr.scrambler_bb

Synchronization
^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.pll_carriertracking_cc
   gnuradio.gr.pll_freqdet_cf
   gnuradio.gr.pll_refout_cc
   gnuradio.gr.pn_correlator_cc
   gnuradio.gr.simple_correlator
   gnuradio.gr.simple_framer

Type Conversions
^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.bytes_to_syms
   gnuradio.gr.char_to_float
   gnuradio.gr.complex_to_interleaved_short
   gnuradio.gr.complex_to_float
   gnuradio.gr.complex_to_real
   gnuradio.gr.complex_to_imag
   gnuradio.gr.complex_to_mag
   gnuradio.gr.complex_to_mag_squared
   gnuradio.gr.complex_to_arg
   gnuradio.gr.float_to_char
   gnuradio.gr.float_to_complex
   gnuradio.gr.float_to_short
   gnuradio.gr.float_to_uchar
   gnuradio.gr.interleaved_short_to_complex
   gnuradio.gr.short_to_float
   gnuradio.gr.uchar_to_float
   gnuradio.gr.unpack_k_bits_bb
   gnuradio.gr.chunks_to_symbols_bc
   gnuradio.gr.chunks_to_symbols_bf
   gnuradio.gr.chunks_to_symbols_ic
   gnuradio.gr.chunks_to_symbols_if
   gnuradio.gr.chunks_to_symbols_sc
   gnuradio.gr.chunks_to_symbols_sf
   gnuradio.gr.packed_to_unpacked_bb
   gnuradio.gr.packed_to_unpacked_ii
   gnuradio.gr.packed_to_unpacked_ss
   gnuradio.gr.unpacked_to_packed_bb
   gnuradio.gr.unpacked_to_packed_ii
   gnuradio.gr.unpacked_to_packed_ss

Signal Level Control (AGC)
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.agc2_cc
   gnuradio.gr.agc2_ff
   gnuradio.gr.agc_cc
   gnuradio.gr.agc_ff
   gnuradio.gr.ctcss_squelch_ff
   gnuradio.gr.dpll_bb
   gnuradio.gr.feedforward_agc_cc
   gnuradio.gr.peak_detector2_fb
   gnuradio.gr.pwr_squelch_cc
   gnuradio.gr.pwr_squelch_ff
   gnuradio.gr.regenerate_bb
   gnuradio.gr.simple_squelch_cc
   gnuradio.gr.mute_cc
   gnuradio.gr.mute_ff
   gnuradio.gr.mute_ii
   gnuradio.gr.mute_ss
   gnuradio.gr.peak_detector_fb
   gnuradio.gr.peak_detector_ib
   gnuradio.gr.peak_detector_sb
   gnuradio.gr.sample_and_hold_bb
   gnuradio.gr.sample_and_hold_ff
   gnuradio.gr.sample_and_hold_ii
   gnuradio.gr.sample_and_hold_ss

Fourier Transform
^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.goertzel_fc
   gnuradio.gr.fft_vcc
   gnuradio.gr.fft_vfc

Miscellaneous Blocks
^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.copy
   gnuradio.gr.delay
   gnuradio.gr.kludge_copy
   gnuradio.gr.nop
   gnuradio.gr.pa_2x2_phase_combiner
   gnuradio.gr.repeat
   gnuradio.gr.threshold_ff
   gnuradio.gr.throttle
   gnuradio.gr.channel_model

Slicing and Dicing Streams
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.deinterleave
   gnuradio.gr.head
   gnuradio.gr.interleave
   gnuradio.gr.keep_one_in_n
   gnuradio.gr.skiphead
   gnuradio.gr.stream_to_streams
   gnuradio.gr.stream_to_vector
   gnuradio.gr.streams_to_stream
   gnuradio.gr.streams_to_vector
   gnuradio.gr.vector_to_stream
   gnuradio.gr.vector_to_streams

Digital Filter Design
---------------------

.. autosummary::
   :nosignatures:

   gnuradio.gr.firdes
   gnuradio.gr.remez

Miscellaneous
-------------

.. autosummary::
   :nosignatures:

   gnuradio.gr.feval_dd
   gnuradio.gr.feval_cc
   gnuradio.gr.feval_ll
   gnuradio.gr.feval
   gnuradio.gr.prefs
   gnuradio.gr.test
   gnuradio.gr.message
   gnuradio.gr.msg_queue
   gnuradio.gr.enable_realtime_scheduling

Implementation Details
----------------------

.. autosummary::
   :nosignatures:

   gnuradio.gr.block_detail
   gnuradio.gr.buffer
   gnuradio.gr.dispatcher
   gnuradio.gr.single_threaded_scheduler

