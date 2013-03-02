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

   gnuradio.gr.lfsr_32k_source_s
   gnuradio.gr.null_source
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
   gnuradio.gr.null_sink
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


Information Coding and Decoding
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.fake_channel_encoder_pp
   gnuradio.gr.fake_channel_decoder_pp


Type Conversions
^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.complex_to_interleaved_short
   gnuradio.gr.complex_to_float
   gnuradio.gr.complex_to_real
   gnuradio.gr.complex_to_imag
   gnuradio.gr.complex_to_mag
   gnuradio.gr.complex_to_mag_squared
   gnuradio.gr.complex_to_arg

Signal Level Control (AGC)
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.peak_detector_fb
   gnuradio.gr.peak_detector_ib
   gnuradio.gr.peak_detector_sb

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
   gnuradio.gr.kludge_copy
   gnuradio.gr.nop
   gnuradio.gr.pa_2x2_phase_combiner
   gnuradio.gr.channel_model

Slicing and Dicing Streams
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.head
   gnuradio.gr.skiphead

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

