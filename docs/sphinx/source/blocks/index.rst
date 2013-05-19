gnuradio.blocks
===============

.. automodule:: gnuradio.blocks

Audio Signals
-------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.wavfile_sink
   gnuradio.blocks.wavfile_source

Boolean Operators
-----------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.and_bb
   gnuradio.blocks.and_const_bb
   gnuradio.blocks.and_const_ii
   gnuradio.blocks.and_const_ss
   gnuradio.blocks.and_ii
   gnuradio.blocks.and_ss
   gnuradio.blocks.not_bb
   gnuradio.blocks.not_ii
   gnuradio.blocks.not_ss
   gnuradio.blocks.or_bb
   gnuradio.blocks.or_ii
   gnuradio.blocks.or_ss
   gnuradio.blocks.xor_bb
   gnuradio.blocks.xor_ii
   gnuradio.blocks.xor_ss

Byte Operators
--------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.packed_to_unpacked_bb
   gnuradio.blocks.packed_to_unpacked_ii
   gnuradio.blocks.packed_to_unpacked_ss
   gnuradio.blocks.unpacked_to_packed_bb
   gnuradio.blocks.unpacked_to_packed_ii
   gnuradio.blocks.unpacked_to_packed_ss
   gnuradio.blocks.pack_k_bits_bb
   gnuradio.blocks.repack_bits_bb
   gnuradio.blocks.unpack_k_bits_bb

ControlPort
-----------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.ctrlport_probe2_c
   gnuradio.blocks.ctrlport_probe_c

Debug Tools
-----------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.vector_sink_b
   gnuradio.blocks.vector_sink_c
   gnuradio.blocks.vector_sink_f
   gnuradio.blocks.vector_sink_i
   gnuradio.blocks.vector_sink_s
   gnuradio.blocks.annotator_1to1
   gnuradio.blocks.annotator_alltoall
   gnuradio.blocks.annotator_raw
   gnuradio.blocks.message_debug
   gnuradio.blocks.random_pdu
   gnuradio.blocks.tag_debug

File Operators
--------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.file_descriptor_sink
   gnuradio.blocks.file_descriptor_source
   gnuradio.blocks.file_meta_sink
   gnuradio.blocks.file_meta_source
   gnuradio.blocks.file_sink
   gnuradio.blocks.file_source
   gnuradio.blocks.tagged_file_sink

Level Controllers
-----------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.moving_average_cc
   gnuradio.blocks.moving_average_ff
   gnuradio.blocks.moving_average_ii
   gnuradio.blocks.moving_average_ss
   gnuradio.blocks.mute_cc
   gnuradio.blocks.mute_ff
   gnuradio.blocks.mute_ii
   gnuradio.blocks.mute_ss
   gnuradio.blocks.sample_and_hold_bb
   gnuradio.blocks.sample_and_hold_ff
   gnuradio.blocks.sample_and_hold_ii
   gnuradio.blocks.sample_and_hold_ss
   gnuradio.blocks.threshold_ff

Math Operators
--------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.add_cc
   gnuradio.blocks.add_const_bb
   gnuradio.blocks.add_const_cc
   gnuradio.blocks.add_const_ff
   gnuradio.blocks.add_const_ii
   gnuradio.blocks.add_const_ss
   gnuradio.blocks.add_const_vbb
   gnuradio.blocks.add_const_vcc
   gnuradio.blocks.add_const_vff
   gnuradio.blocks.add_const_vii
   gnuradio.blocks.add_const_vss
   gnuradio.blocks.add_ii
   gnuradio.blocks.add_ss
   gnuradio.blocks.argmax_fs
   gnuradio.blocks.argmax_is
   gnuradio.blocks.argmax_ss
   gnuradio.blocks.divide_cc
   gnuradio.blocks.divide_ff
   gnuradio.blocks.divide_ii
   gnuradio.blocks.divide_ss
   gnuradio.blocks.integrate_cc
   gnuradio.blocks.integrate_ff
   gnuradio.blocks.integrate_ii
   gnuradio.blocks.integrate_ss
   gnuradio.blocks.max_ff
   gnuradio.blocks.max_ii
   gnuradio.blocks.max_ss
   gnuradio.blocks.multiply_const_ii
   gnuradio.blocks.multiply_const_ss
   gnuradio.blocks.multiply_const_vcc
   gnuradio.blocks.multiply_const_vff
   gnuradio.blocks.multiply_const_vii
   gnuradio.blocks.multiply_const_vss
   gnuradio.blocks.multiply_ii
   gnuradio.blocks.multiply_ss
   gnuradio.blocks.sub_cc
   gnuradio.blocks.sub_ff
   gnuradio.blocks.sub_ii
   gnuradio.blocks.sub_ss
   gnuradio.blocks.add_ff
   gnuradio.blocks.conjugate_cc
   gnuradio.blocks.multiply_cc
   gnuradio.blocks.multiply_conjugate_cc
   gnuradio.blocks.multiply_const_cc
   gnuradio.blocks.multiply_const_ff
   gnuradio.blocks.multiply_ff
   gnuradio.blocks.nlog10_ff
   gnuradio.blocks.rms_cf
   gnuradio.blocks.rms_ff
   gnuradio.blocks.transcendental

Measurement Tools
-----------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.probe_signal_b
   gnuradio.blocks.probe_signal_c
   gnuradio.blocks.probe_signal_f
   gnuradio.blocks.probe_signal_i
   gnuradio.blocks.probe_signal_s
   gnuradio.blocks.probe_signal_vb
   gnuradio.blocks.probe_signal_vc
   gnuradio.blocks.probe_signal_vf
   gnuradio.blocks.probe_signal_vi
   gnuradio.blocks.probe_signal_vs
   gnuradio.blocks.ctrlport_probe2_c
      :noindex:
   gnuradio.blocks.ctrlport_probe_c
      :noindex:
   gnuradio.blocks.message_debug
      :noindex:
   gnuradio.blocks.probe_rate
   gnuradio.blocks.tag_debug
      :noindex:

Message Tools
-------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.message_burst_source
   gnuradio.blocks.message_debug
      :noindex:
   gnuradio.blocks.message_sink
   gnuradio.blocks.message_source
   gnuradio.blocks.message_strobe
   gnuradio.blocks.pdu_to_tagged_stream
   gnuradio.blocks.random_pdu
      :noindex:
   gnuradio.blocks.tagged_stream_to_pdu

Miscellaneous
-------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.vector_source_b
   gnuradio.blocks.vector_source_c
   gnuradio.blocks.vector_source_f
   gnuradio.blocks.vector_source_i
   gnuradio.blocks.vector_source_s
   gnuradio.blocks.bin_statistics_f
   gnuradio.blocks.check_lfsr_32k_s
   gnuradio.blocks.copy
   gnuradio.blocks.delay
   gnuradio.blocks.head
   gnuradio.blocks.lfsr_32k_source_s
   gnuradio.blocks.nop
   gnuradio.blocks.null_sink
   gnuradio.blocks.null_source
   gnuradio.blocks.skiphead
   gnuradio.blocks.throttle

Modulators and Demodulators
---------------------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.vco_f

Networking Tools
----------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.socket_pdu
   gnuradio.blocks.tuntap_pdu
   gnuradio.blocks.udp_sink
   gnuradio.blocks.udp_source

Peak Detectors
--------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.peak_detector_fb
   gnuradio.blocks.peak_detector_ib
   gnuradio.blocks.peak_detector_sb
   gnuradio.blocks.burst_tagger
   gnuradio.blocks.peak_detector2_fb
   gnuradio.blocks.plateau_detector_fb

Stream Operators
----------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.vector_insert_b
   gnuradio.blocks.vector_insert_c
   gnuradio.blocks.vector_insert_f
   gnuradio.blocks.vector_insert_i
   gnuradio.blocks.vector_insert_s
   gnuradio.blocks.deinterleave
   gnuradio.blocks.endian_swap
   gnuradio.blocks.interleave
   gnuradio.blocks.keep_m_in_n
   gnuradio.blocks.keep_one_in_n
   gnuradio.blocks.patterned_interleaver
   gnuradio.blocks.regenerate_bb
   gnuradio.blocks.repeat
   gnuradio.blocks.stream_mux
   gnuradio.blocks.stream_to_streams
   gnuradio.blocks.stream_to_vector
   gnuradio.blocks.streams_to_stream
   gnuradio.blocks.streams_to_vector
   gnuradio.blocks.stretch_ff
   gnuradio.blocks.tagged_stream_mux
   gnuradio.blocks.vector_map
   gnuradio.blocks.vector_to_stream
   gnuradio.blocks.vector_to_streams

Stream Tag Tools
----------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.burst_tagger
      :noindex:
   gnuradio.blocks.tag_debug
      :noindex:
   gnuradio.blocks.tagged_file_sink
      :noindex:

Data Type Converters
--------------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.char_to_float
   gnuradio.blocks.char_to_short
   gnuradio.blocks.complex_to_arg
   gnuradio.blocks.complex_to_float
   gnuradio.blocks.complex_to_imag
   gnuradio.blocks.complex_to_interleaved_short
   gnuradio.blocks.complex_to_mag
   gnuradio.blocks.complex_to_mag_squared
   gnuradio.blocks.complex_to_real
   gnuradio.blocks.float_to_char
   gnuradio.blocks.float_to_complex
   gnuradio.blocks.float_to_int
   gnuradio.blocks.float_to_short
   gnuradio.blocks.float_to_uchar
   gnuradio.blocks.int_to_float
   gnuradio.blocks.interleaved_short_to_complex
   gnuradio.blocks.short_to_char
   gnuradio.blocks.short_to_float
   gnuradio.blocks.uchar_to_float

