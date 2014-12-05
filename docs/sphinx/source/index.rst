gnuradio
========

.. toctree::
   :hidden:

   runtime <runtime>
   pmt <pmt>
   blocks_audio <audio_blocks>
   blocks_analog <analog_blocks>
   blocks_blocks <blocks_blocks>
   blocks_channels <channels_blocks>
   blocks_comedi <comedi_blocks>
   blocks_digital <digital_blocks>
   blocks_dtv <dtv_blocks>
   blocks_fcd <fcd_blocks>
   blocks_fec <fec_blocks>
   blocks_fft <fft_blocks>
   blocks_filter <filter_blocks>
   blocks_noaa <noaa_blocks>
   blocks_pager <pager_blocks>
   blocks_qtgui <qtgui_blocks>
   blocks_trellis <trellis_blocks>
   blocks_uhd <uhd_blocks>
   blocks_video_sdl <video_sdl_blocks>
   blocks_vocoder <vocoder_blocks>
   blocks_wavelet <wavelet_blocks>
   blocks_wxgui <wxgui_blocks>
   blocks_zeromq <zeromq_blocks>
   analog <analog>
   channels <channels>
   digital <digital>
   dtv <dtv>
   fec <fec>
   fft <fft>
   filter <filter>
   trellis <trellis>
   uhd <uhd>
   vocoder <vocoder>
   wxgui <wxgui>

.. automodule:: gnuradio
.. automodule:: pmt

Runtime
-------

.. autosummary::
   :nosignatures:

   gnuradio.gr.top_block
   gnuradio.gr.basic_block
   gnuradio.gr.block
   gnuradio.gr.sync_block
   gnuradio.gr.sync_decimator
   gnuradio.gr.sync_interpolator
   gnuradio.gr.tagged_stream_block
   gnuradio.gr.hier_block2
   gnuradio.gr.high_res_timer_now
   gnuradio.gr.high_res_timer_now_perfmon
   gnuradio.gr.high_res_timer_epoch
   gnuradio.gr.high_res_timer_tps
   gnuradio.gr.io_signature
   gnuradio.gr.io_signature2
   gnuradio.gr.io_signature3
   gnuradio.gr.io_signaturev
   gnuradio.gr.prefix
   gnuradio.gr.prefsdir
   gnuradio.gr.sysconfdir
   gnuradio.gr.version
   gnuradio.gr.major_version
   gnuradio.gr.api_version
   gnuradio.gr.minor_version
   gnuradio.gr.prefs
   gnuradio.gr.logger
   gnuradio.gr.logger_config
   gnuradio.gr.logger_get_names
   gnuradio.gr.logger_reset_config
   gnuradio.gr.tag_t
   gnuradio.gr.tag_t_offset_compare
   gnuradio.gr.tag_t_offset_compare_key
   gnuradio.gr.tag_to_pmt
   gnuradio.gr.tag_to_python
   gnuradio.gr.tag_utils
   gnuradio.gr.sizeof_gr_complex
   gnuradio.gr.sizeof_float
   gnuradio.gr.sizeof_int
   gnuradio.gr.sizeof_short
   gnuradio.gr.sizeof_char
   gnuradio.gr.sizeof_double
   gnuradio.gr.branchless_binary_slicer
   gnuradio.gr.binary_slicer
   gnuradio.gr.branchless_clip
   gnuradio.gr.clip
   gnuradio.gr.branchless_quad_0deg_slicer
   gnuradio.gr.quad_0deg_slicer
   gnuradio.gr.branchless_quad_45deg_slicer
   gnuradio.gr.quad_45deg_slicer
   gnuradio.gr.feval
   gnuradio.gr.feval_cc
   gnuradio.gr.feval_dd
   gnuradio.gr.feval_ll
   gnuradio.gr.feval_p
   gnuradio.gr.gateway


PMT
---

.. autosummary::
   :nosignatures:

   pmt.acons
   pmt.any_ref
   pmt.any_set
   pmt.assoc
   pmt.assq
   pmt.assv
   pmt.blob_data
   pmt.blob_length
   pmt.c32vector_elements
   pmt.c32vector_ref
   pmt.c32vector_set
   pmt.c64vector_elements
   pmt.c64vector_ref
   pmt.c64vector_set
   pmt.caar
   pmt.cadddr
   pmt.caddr
   pmt.cadr
   pmt.car
   pmt.cdar
   pmt.cddr
   pmt.cdr
   pmt.cons
   pmt.deserialize
   pmt.deserialize_str
   pmt.dict_add
   pmt.dict_delete
   pmt.dict_has_key
   pmt.dict_items
   pmt.dict_keys
   pmt.dict_ref
   pmt.dict_update
   pmt.dict_values
   pmt.dump_sizeof
   pmt.eq
   pmt.equal
   pmt.eqv
   pmt.f32vector_elements
   pmt.f32vector_ref
   pmt.f32vector_set
   pmt.f64vector_elements
   pmt.f64vector_ref
   pmt.f64vector_set
   pmt.from_bool
   pmt.from_complex
   pmt.from_double
   pmt.from_float
   pmt.from_long
   pmt.from_uint64
   pmt.get_PMT_EOF
   pmt.get_PMT_F
   pmt.get_PMT_NIL
   pmt.get_PMT_T
   pmt.init_c32vector
   pmt.init_c64vector
   pmt.init_f32vector
   pmt.init_f64vector
   pmt.init_s16vector
   pmt.init_s32vector
   pmt.init_s8vector
   pmt.init_u16vector
   pmt.init_u32vector
   pmt.init_u8vector
   pmt.intern
   pmt.is_any
   pmt.is_blob
   pmt.is_bool
   pmt.is_c32vector
   pmt.is_c64vector
   pmt.is_complex
   pmt.is_dict
   pmt.is_eof_object
   pmt.is_f32vector
   pmt.is_f64vector
   pmt.is_false
   pmt.is_integer
   pmt.is_msg_accepter
   pmt.is_null
   pmt.is_number
   pmt.is_pair
   pmt.is_real
   pmt.is_s16vector
   pmt.is_s32vector
   pmt.is_s64vector
   pmt.is_s8vector
   pmt.is_symbol
   pmt.is_true
   pmt.is_tuple
   pmt.is_u16vector
   pmt.is_u32vector
   pmt.is_u64vector
   pmt.is_u8vector
   pmt.is_uint64
   pmt.is_uniform_vector
   pmt.is_vector
   pmt.length
   pmt.list1
   pmt.list2
   pmt.list3
   pmt.list4
   pmt.list5
   pmt.list6
   pmt.list_add
   pmt.list_has
   pmt.list_rm
   pmt.make_any
   pmt.make_blob
   pmt.make_c32vector
   pmt.make_c64vector
   pmt.make_dict
   pmt.make_f32vector
   pmt.make_f64vector
   pmt.make_msg_accepter
   pmt.make_rectangular
   pmt.make_s16vector
   pmt.make_s32vector
   pmt.make_s64vector
   pmt.make_s8vector
   pmt.make_tuple
   pmt.make_u16vector
   pmt.make_u32vector
   pmt.make_u64vector
   pmt.make_u8vector
   pmt.make_vector
   pmt.map
   pmt.member
   pmt.memq
   pmt.memv
   pmt.msg_accepter_ref
   pmt.nth
   pmt.nthcdr
   pmt.pmt_vector_cdouble
   pmt.pmt_vector_cfloat
   pmt.pmt_vector_double
   pmt.pmt_vector_float
   pmt.pmt_vector_int16
   pmt.pmt_vector_int32
   pmt.pmt_vector_int8
   pmt.pmt_vector_uint16
   pmt.pmt_vector_uint32
   pmt.pmt_vector_uint8
   pmt.read
   pmt.reverse
   pmt.reverse_x
   pmt.s16vector_elements
   pmt.s16vector_ref
   pmt.s16vector_set
   pmt.s32vector_elements
   pmt.s32vector_ref
   pmt.s32vector_set
   pmt.s64vector_ref
   pmt.s64vector_set
   pmt.s8vector_elements
   pmt.s8vector_ref
   pmt.s8vector_set
   pmt.serialize
   pmt.serialize_str
   pmt.set_car
   pmt.set_cdr
   pmt.string_to_symbol
   pmt.subsetp
   pmt.symbol_to_string
   pmt.to_bool
   pmt.to_complex
   pmt.to_double
   pmt.to_float
   pmt.to_long
   pmt.to_pmt
   pmt.to_python
   pmt.to_tuple
   pmt.to_uint64
   pmt.tuple_ref
   pmt.u16vector_elements
   pmt.u16vector_ref
   pmt.u16vector_set
   pmt.u32vector_elements
   pmt.u32vector_ref
   pmt.u32vector_set
   pmt.u64vector_ref
   pmt.u64vector_set
   pmt.u8vector_elements
   pmt.u8vector_ref
   pmt.u8vector_set
   pmt.uniform_vector_elements
   pmt.uniform_vector_itemsize
   pmt.vector_fill
   pmt.vector_ref
   pmt.vector_set
   pmt.write
   pmt.write_string


Audio Signals
-------------

.. autosummary::
   :nosignatures:

   gnuradio.audio.sink
   gnuradio.audio.source
   gnuradio.vocoder.alaw_decode_bs
   gnuradio.vocoder.alaw_encode_sb
   gnuradio.vocoder.codec2_decode_ps
   gnuradio.vocoder.codec2_encode_sp
   gnuradio.vocoder.cvsd_decode_bs
   gnuradio.vocoder.cvsd_encode_sb
   gnuradio.vocoder.g721_decode_bs
   gnuradio.vocoder.g721_encode_sb
   gnuradio.vocoder.g723_24_decode_bs
   gnuradio.vocoder.g723_24_encode_sb
   gnuradio.vocoder.g723_40_decode_bs
   gnuradio.vocoder.g723_40_encode_sb
   gnuradio.vocoder.gsm_fr_decode_ps
   gnuradio.vocoder.gsm_fr_encode_sp
   gnuradio.vocoder.ulaw_decode_bs
   gnuradio.vocoder.ulaw_encode_sb
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

Channelizers
------------

.. autosummary::
   :nosignatures:

   gnuradio.filter.freq_xlating_fir_filter_ccc
   gnuradio.filter.freq_xlating_fir_filter_ccf
   gnuradio.filter.freq_xlating_fir_filter_fcc
   gnuradio.filter.freq_xlating_fir_filter_fcf
   gnuradio.filter.freq_xlating_fir_filter_scc
   gnuradio.filter.freq_xlating_fir_filter_scf
   gnuradio.filter.pfb_channelizer_ccf
   gnuradio.filter.pfb_decimator_ccf
   gnuradio.filter.pfb_interpolator_ccf
   gnuradio.filter.pfb_synthesizer_ccf

Channel Models
--------------

.. autosummary::
   :nosignatures:

   gnuradio.channels.channel_model
   gnuradio.channels.channel_model2
   gnuradio.channels.fading_model
   gnuradio.channels.selective_fading_model
   gnuradio.channels.dynamic_channel_model
   gnuradio.channels.cfo_model
   gnuradio.channels.sro_model


Coding Blocks
-------------

.. autosummary::
   :nosignatures:

   gnuradio.digital.additive_scrambler_bb
   gnuradio.digital.descrambler_bb
   gnuradio.digital.scrambler_bb


ControlPort Blocks
------------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.ctrlport_probe2_b
   gnuradio.blocks.ctrlport_probe2_c
   gnuradio.blocks.ctrlport_probe2_f
   gnuradio.blocks.ctrlport_probe2_i
   gnuradio.blocks.ctrlport_probe2_s
   gnuradio.blocks.ctrlport_probe_c
   gnuradio.fft.ctrlport_probe_psd


Debug Blocks
------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.message_debug
   gnuradio.blocks.message_strobe
   gnuradio.blocks.message_strobe_random
   gnuradio.blocks.tag_debug
   gnuradio.blocks.tags_strobe
   gnuradio.blocks.vector_sink_b
   gnuradio.blocks.vector_sink_c
   gnuradio.blocks.vector_sink_f
   gnuradio.blocks.vector_sink_i
   gnuradio.blocks.vector_sink_s
   gnuradio.blocks.random_pdu


DTV Blocks
------------

.. autosummary::
   :nosignatures:

   gnuradio.dtv.atsc_deinterleaver
   gnuradio.dtv.atsc_depad
   gnuradio.dtv.atsc_derandomizer
   gnuradio.dtv.atsc_equalizer
   gnuradio.dtv.atsc_field_sync_mux
   gnuradio.dtv.atsc_fpll
   gnuradio.dtv.atsc_fs_checker
   gnuradio.dtv.atsc_interleaver
   gnuradio.dtv.atsc_pad
   gnuradio.dtv.atsc_randomizer
   gnuradio.dtv.atsc_rs_decoder
   gnuradio.dtv.atsc_rs_encoder
   gnuradio.dtv.atsc_sync
   gnuradio.dtv.atsc_trellis_encoder
   gnuradio.dtv.atsc_viterbi_decoder
   gnuradio.dtv.dvb_bbheader_bb
   gnuradio.dtv.dvb_bbscrambler_bb
   gnuradio.dtv.dvb_bch_bb
   gnuradio.dtv.dvb_ldpc_bb
   gnuradio.dtv.dvbs2_interleaver_bb
   gnuradio.dtv.dvbs2_modulator_bc
   gnuradio.dtv.dvbs2_physical_cc
   gnuradio.dtv.dvbt2_cellinterleaver_cc
   gnuradio.dtv.dvbt2_framemapper_cc
   gnuradio.dtv.dvbt2_freqinterleaver_cc
   gnuradio.dtv.dvbt2_interleaver_bb
   gnuradio.dtv.dvbt2_miso_cc
   gnuradio.dtv.dvbt2_modulator_bc
   gnuradio.dtv.dvbt2_p1insertion_cc
   gnuradio.dtv.dvbt2_paprtr_cc
   gnuradio.dtv.dvbt2_pilotgenerator_cc
   gnuradio.dtv.dvbt_bit_inner_interleaver
   gnuradio.dtv.dvbt_convolutional_interleaver
   gnuradio.dtv.dvbt_energy_dispersal
   gnuradio.dtv.dvbt_inner_coder
   gnuradio.dtv.dvbt_map
   gnuradio.dtv.dvbt_reed_solomon_enc
   gnuradio.dtv.dvbt_reference_signals
   gnuradio.dtv.dvbt_symbol_inner_interleaver


Equalizer Blocks
----------------

.. autosummary::
   :nosignatures:

   gnuradio.digital.cma_equalizer_cc
   gnuradio.digital.lms_dd_equalizer_cc
   gnuradio.digital.kurtotic_equalizer_cc

Error Coding Blocks
-------------------

.. autosummary::
   :nosignatures:

   gnuradio.fec.async_decoder
   gnuradio.fec.async_encoder
   gnuradio.fec.ber_bf
   gnuradio.fec.conv_bit_corr_bb
   gnuradio.fec.decode_ccsds_27_fb
   gnuradio.fec.decoder
   gnuradio.fec.depuncture_bb
   gnuradio.fec.encode_ccsds_27_bb
   gnuradio.fec.encoder
   gnuradio.fec.generic_decoder
   gnuradio.fec.generic_encoder
   gnuradio.fec.puncture_bb
   gnuradio.fec.puncture_ff
   gnuradio.fec.tagged_decoder
   gnuradio.fec.tagged_encoder


FCD Blocks
----------

.. autosummary::
   :nosignatures:

   gnuradio.fcd.source_c


File Operator Blocks
--------------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.file_descriptor_sink
   gnuradio.blocks.file_descriptor_source
   gnuradio.blocks.file_meta_sink
   gnuradio.blocks.file_meta_source
   gnuradio.blocks.file_sink
   gnuradio.blocks.file_source
   gnuradio.blocks.tagged_file_sink


Filter Blocks
-------------

.. autosummary::
   :nosignatures:

   gnuradio.filter.dc_blocker_cc
   gnuradio.filter.dc_blocker_ff
   gnuradio.filter.fft_filter_ccc
   gnuradio.filter.fft_filter_ccf
   gnuradio.filter.fft_filter_fff
   gnuradio.filter.filter_delay_fc
   gnuradio.filter.filterbank_vcvcf
   gnuradio.filter.fir_filter_ccc
   gnuradio.filter.fir_filter_ccf
   gnuradio.filter.fir_filter_fcc
   gnuradio.filter.fir_filter_fff
   gnuradio.filter.fir_filter_fsf
   gnuradio.filter.fir_filter_scc
   gnuradio.filter.fractional_interpolator_cc
   gnuradio.filter.fractional_interpolator_ff
   gnuradio.filter.fractional_resampler_cc
   gnuradio.filter.fractional_resampler_ff
   gnuradio.filter.freq_xlating_fir_filter_ccc
   gnuradio.filter.freq_xlating_fir_filter_ccf
   gnuradio.filter.freq_xlating_fir_filter_fcc
   gnuradio.filter.freq_xlating_fir_filter_fcf
   gnuradio.filter.freq_xlating_fir_filter_scc
   gnuradio.filter.freq_xlating_fir_filter_scf
   gnuradio.filter.hilbert_fc
   gnuradio.filter.iir_filter_ccc
   gnuradio.filter.iir_filter_ccd
   gnuradio.filter.iir_filter_ccf
   gnuradio.filter.iir_filter_ccz
   gnuradio.filter.iir_filter_ffd
   gnuradio.filter.interp_fir_filter_ccc
   gnuradio.filter.interp_fir_filter_ccf
   gnuradio.filter.interp_fir_filter_fcc
   gnuradio.filter.interp_fir_filter_fff
   gnuradio.filter.interp_fir_filter_fsf
   gnuradio.filter.interp_fir_filter_scc
   gnuradio.filter.pfb_arb_resampler_ccc
   gnuradio.filter.pfb_arb_resampler_ccf
   gnuradio.filter.pfb_arb_resampler_fff
   gnuradio.filter.pfb_channelizer_ccf
   gnuradio.filter.pfb_decimator_ccf
   gnuradio.filter.pfb_interpolator_ccf
   gnuradio.filter.pfb_synthesizer_ccf
   gnuradio.filter.rational_resampler_base_ccc
   gnuradio.filter.rational_resampler_base_ccf
   gnuradio.filter.rational_resampler_base_fcc
   gnuradio.filter.rational_resampler_base_fff
   gnuradio.filter.rational_resampler_base_fsf
   gnuradio.filter.rational_resampler_base_scc
   gnuradio.filter.single_pole_iir_filter_cc
   gnuradio.filter.single_pole_iir_filter_ff



Fourier Analysis
----------------------

.. autosummary::
   :nosignatures:

   gnuradio.fft.fft_vcc
   gnuradio.fft.fft_vfc
   gnuradio.fft.goertzel_fc


Impairment Model Blocks
-----------------------

.. autosummary::
   :nosignatures:

   gnuradio.channels.amp_bal
   gnuradio.channels.conj_fs_iqcorr
   gnuradio.channels.distortion_2_gen
   gnuradio.channels.distortion_3_gen
   gnuradio.channels.impairments
   gnuradio.channels.iqbal_gen
   gnuradio.channels.phase_bal
   gnuradio.channels.phase_noise_gen
   gnuradio.channels.quantizer


Instrumentation Blocks
----------------------

.. autosummary::
   :nosignatures:

   gnuradio.qtgui.ber_sink_b
   gnuradio.qtgui.const_sink_c
   gnuradio.qtgui.freq_sink_c
   gnuradio.qtgui.freq_sink_f
   gnuradio.qtgui.histogram_sink_f
   gnuradio.qtgui.number_sink
   gnuradio.qtgui.sink_c
   gnuradio.qtgui.sink_f
   gnuradio.qtgui.time_raster_sink_b
   gnuradio.qtgui.time_raster_sink_f
   gnuradio.qtgui.time_sink_c
   gnuradio.qtgui.time_sink_f
   gnuradio.qtgui.vector_sink_f
   gnuradio.qtgui.waterfall_sink_c
   gnuradio.qtgui.waterfall_sink_f
   gnuradio.wxgui.histo_sink_f
   gnuradio.wxgui.oscope_sink_f



Level Control Blocks
--------------------

.. autosummary::
   :nosignatures:

   gnuradio.analog.agc2_cc
   gnuradio.analog.agc2_ff
   gnuradio.analog.agc3_cc
   gnuradio.analog.agc_cc
   gnuradio.analog.agc_ff
   gnuradio.analog.ctcss_squelch_ff
   gnuradio.analog.feedforward_agc_cc
   gnuradio.blocks.moving_average_cc
   gnuradio.blocks.moving_average_ff
   gnuradio.blocks.moving_average_ii
   gnuradio.blocks.moving_average_ss
   gnuradio.blocks.mute_cc
   gnuradio.blocks.mute_ff
   gnuradio.blocks.mute_ii
   gnuradio.blocks.mute_ss
   gnuradio.analog.pwr_squelch_cc
   gnuradio.analog.pwr_squelch_ff
   gnuradio.analog.rail_ff
   gnuradio.blocks.sample_and_hold_bb
   gnuradio.blocks.sample_and_hold_ff
   gnuradio.blocks.sample_and_hold_ii
   gnuradio.blocks.sample_and_hold_ss
   gnuradio.analog.simple_squelch_cc
   gnuradio.blocks.threshold_ff


Math Operator Blocks
--------------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.abs_ff
   gnuradio.blocks.abs_ii
   gnuradio.blocks.abs_ss
   gnuradio.blocks.add_cc
   gnuradio.blocks.add_ff
   gnuradio.blocks.add_ii
   gnuradio.blocks.add_ss
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
   gnuradio.blocks.argmax_fs
   gnuradio.blocks.argmax_is
   gnuradio.blocks.argmax_ss
   gnuradio.blocks.conjugate_cc
   gnuradio.blocks.divide_cc
   gnuradio.blocks.divide_ff
   gnuradio.blocks.divide_ii
   gnuradio.blocks.divide_ss
   gnuradio.blocks.integrate_cc
   gnuradio.blocks.integrate_ff
   gnuradio.blocks.integrate_ii
   gnuradio.blocks.integrate_ss
   gnuradio.blocks.nlog10_ff
   gnuradio.blocks.max_ff
   gnuradio.blocks.max_ii
   gnuradio.blocks.max_ss
   gnuradio.blocks.min_ff
   gnuradio.blocks.min_ii
   gnuradio.blocks.min_ss
   gnuradio.blocks.multiply_cc
   gnuradio.blocks.multiply_ff
   gnuradio.blocks.multiply_ii
   gnuradio.blocks.multiply_ss
   gnuradio.blocks.multiply_matrix_ff
   gnuradio.blocks.multiply_conjugate_cc
   gnuradio.blocks.multiply_const_cc
   gnuradio.blocks.multiply_const_ff
   gnuradio.blocks.multiply_const_ii
   gnuradio.blocks.multiply_const_ss
   gnuradio.blocks.multiply_const_vcc
   gnuradio.blocks.multiply_const_vff
   gnuradio.blocks.multiply_const_vii
   gnuradio.blocks.multiply_const_vss
   gnuradio.blocks.rms_cf
   gnuradio.blocks.rms_ff
   gnuradio.blocks.rotator_cc
   gnuradio.blocks.sub_cc
   gnuradio.blocks.sub_ff
   gnuradio.blocks.sub_ii
   gnuradio.blocks.sub_ss
   gnuradio.blocks.transcendental


Measurement Tool Blocks
----------------------_

.. autosummary::
   :nosignatures:

   gnuradio.digital.mpsk_snr_est_cc
   gnuradio.digital.probe_mpsk_snr_est_c
   gnuradio.digital.probe_density_b
   gnuradio.blocks.probe_rate
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


Message Tool Blocks
-------------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.message_burst_source
   gnuradio.blocks.message_debug
   gnuradio.blocks.message_sink
   gnuradio.blocks.message_source
   gnuradio.blocks.message_strobe
   gnuradio.blocks.message_strobe_random
   gnuradio.blocks.pdu_filter
   gnuradio.blocks.pdu_remove
   gnuradio.blocks.pdu_set
   gnuradio.blocks.pdu_to_tagged_stream
   gnuradio.blocks.tagged_stream_multiply_length
   gnuradio.blocks.tagged_stream_to_pdu


Misc Blocks
-----------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.copy
   gnuradio.blocks.delay
   gnuradio.blocks.head
   gnuradio.blocks.nop
   gnuradio.blocks.null_sink
   gnuradio.blocks.null_source
   gnuradio.blocks.skiphead
   gnuradio.blocks.throttle
   gnuradio.blocks.vector_source_b
   gnuradio.blocks.vector_source_c
   gnuradio.blocks.vector_source_f
   gnuradio.blocks.vector_source_i
   gnuradio.blocks.vector_source_s


Modulator Blocks
----------------

.. autosummary::
   :nosignatures:

   gnuradio.analog.am_demod_cf
   gnuradio.analog.cpm
   gnuradio.analog.cpfsk_bc
   gnuradio.analog.frequency_modulator_fc
   gnuradio.analog.fm_demod_cf
   gnuradio.analog.demod_20k0f3e_cf
   gnuradio.analog.demod_200kf3e_cf
   gnuradio.analog.fm_deemph
   gnuradio.analog.fm_preemph
   gnuradio.analog.nbfm_rx
   gnuradio.analog.nbfm_tx
   gnuradio.analog.phase_modulator_fc
   gnuradio.analog.quadrature_demod_cf
   gnuradio.analog.wfm_rcv_fmdet
   gnuradio.analog.wfm_rcv_pll
   gnuradio.analog.wfm_rcv
   gnuradio.analog.wfm_tx


Networking Tools Blocks
-----------------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.socket_pdu
   gnuradio.blocks.tcp_server_sink
   gnuradio.blocks.udp_sink
   gnuradio.blocks.udp_source


NOAA Blocks
-----------

.. autosummary::
   :nosignatures:

   gnuradio.noaa.hrpt_decoder
   gnuradio.noaa.hrpt_deframer
   gnuradio.noaa.hrpt_pll_cf


OFDM Blocks
-----------

.. autosummary::
   :nosignatures:

   gnuradio.digital.ofdm_carrier_allocator_cvc
   gnuradio.digital.ofdm_chanest_vcvc
   gnuradio.digital.ofdm_cyclic_prefixer
   gnuradio.digital.ofdm_equalizer_base
   gnuradio.digital.ofdm_equalizer_simpledfe
   gnuradio.digital.ofdm_equalizer_static
   gnuradio.digital.ofdm_frame_acquisition
   gnuradio.digital.ofdm_frame_equalizer_vcvc
   gnuradio.digital.ofdm_frame_sink
   gnuradio.digital.ofdm_insert_preamble
   gnuradio.digital.ofdm_sampler
   gnuradio.digital.ofdm_serializer_vcc
   gnuradio.digital.ofdm_sync_sc_cfb


Packet Operator Blocks
----------------------

.. autosummary::
   :nosignatures:

   gnuradio.digital.crc32_async_bb
   gnuradio.digital.crc32_bb
   gnuradio.digital.correlate_access_code_bb
   gnuradio.digital.correlate_access_code_bb_ts
   gnuradio.digital.correlate_access_code_ff_ts
   gnuradio.digital.correlate_access_code_tag_bb
   gnuradio.digital.framer_sink_1
   gnuradio.digital.hdlc_deframer_bp
   gnuradio.digital.hdlc_framer_pb
   gnuradio.digital.header_payload_demux
   gnuradio.digital.packet_header_default
   gnuradio.digital.packet_headergenerator_bb
   gnuradio.digital.packet_sink
   gnuradio.digital.simple_correlator
   gnuradio.digital.simple_framer


Pager Blocks
------------

.. autosummary::
   :nosignatures:

   gnuradio.pager.flex_deinterleave
   gnuradio.pager.flex_frame
   gnuradio.pager.flex_parse
   gnuradio.pager.flex_sync
   gnuradio.pager.slicer_fb


Peak Detector Blocks
--------------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.burst_tagger
   gnuradio.blocks.peak_detector2_fb
   gnuradio.blocks.peak_detector_fb
   gnuradio.blocks.peak_detector_ib
   gnuradio.blocks.peak_detector_sb
   gnuradio.blocks.plateau_detector_fb


Resampler Blocks
----------------

.. autosummary::
   :nosignatures:

   gnuradio.filter.fractional_resampler_cc
   gnuradio.filter.fractional_resampler_ff
   gnuradio.filter.pfb.arb_resampler_ccf
   gnuradio.filter.pfb.arb_resampler_fff
   gnuradio.filter.pfb.arb_resampler_ccc
   gnuradio.filter.pfb_arb_resampler_ccc
   gnuradio.filter.pfb_arb_resampler_ccf
   gnuradio.filter.pfb_arb_resampler_fff
   gnuradio.filter.rational_resampler_fff
   gnuradio.filter.rational_resampler_ccf
   gnuradio.filter.rational_resampler_ccc
   gnuradio.filter.rational_resampler_base_ccc
   gnuradio.filter.rational_resampler_base_ccf
   gnuradio.filter.rational_resampler_base_fcc
   gnuradio.filter.rational_resampler_base_fff
   gnuradio.filter.rational_resampler_base_fsf
   gnuradio.filter.rational_resampler_base_scc


Stream Operator Blocks
----------------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.deinterleave
   gnuradio.blocks.endian_swap
   gnuradio.blocks.keep_m_in_n
   gnuradio.blocks.keep_one_in_n
   gnuradio.blocks.patterned_interleaver
   gnuradio.blocks.regenerate_bb
   gnuradio.blocks.repeat
   gnuradio.blocks.stream_mux
   gnuradio.blocks.stream_to_streams
   gnuradio.blocks.stream_to_tagged_stream
   gnuradio.blocks.stream_to_vector
   gnuradio.blocks.streams_to_stream
   gnuradio.blocks.streams_to_vector
   gnuradio.blocks.stretch_ff
   gnuradio.blocks.tagged_stream_mux
   gnuradio.blocks.vector_insert_b
   gnuradio.blocks.vector_insert_c
   gnuradio.blocks.vector_insert_f
   gnuradio.blocks.vector_insert_i
   gnuradio.blocks.vector_insert_s
   gnuradio.blocks.vector_to_stream
   gnuradio.blocks.vector_to_streams


Stream Tag Tool Blocks
----------------------

.. autosummary::
   :nosignatures:

   gnuradio.blocks.stream_to_tagged_stream
   gnuradio.blocks.tag_gate
   gnuradio.blocks.tagged_stream_align
   gnuradio.blocks.tagged_stream_multiply_length
   gnuradio.blocks.tagged_stream_mux


Symbol Coding Blocks
--------------------

.. autosummary::
   :nosignatures:

   gnuradio.digital.binary_slicer_fb
   gnuradio.digital.chunks_to_symbols_bc
   gnuradio.digital.chunks_to_symbols_bf
   gnuradio.digital.chunks_to_symbols_ic
   gnuradio.digital.chunks_to_symbols_if
   gnuradio.digital.chunks_to_symbols_sc
   gnuradio.digital.chunks_to_symbols_sf
   gnuradio.digital.constellation_decoder_cb
   gnuradio.digital.constellation_soft_decoder_cf
   gnuradio.digital.diff_decoder_bb
   gnuradio.digital.diff_encoder_bb
   gnuradio.digital.diff_phasor_cc
   gnuradio.digital.map_bb


Synchronizer Blocks
--------------------

.. autosummary::
   :nosignatures:

   gnuradio.digital.clock_recovery_mm_cc
   gnuradio.digital.clock_recovery_mm_ff
   gnuradio.digital.correlate_and_sync_cc
   gnuradio.digital.corr_est_cc
   gnuradio.digital.costas_loop_cc
   gnuradio.digital.fll_band_edge_cc
   gnuradio.digital.mpsk_receiver_cc
   gnuradio.digital.msk_timing_recovery_cc
   gnuradio.analog.pll_carriertracking_cc
   gnuradio.analog.pll_freqdet_cf
   gnuradio.analog.pll_refout_cc
   gnuradio.digital.pn_correlator_cc
   gnuradio.digital.pfb_clock_sync_ccf
   gnuradio.digital.pfb_clock_sync_fff


Trellis Coding Blocks
---------------------

.. autosummary::
   :nosignatures:

   gnuradio.trellis.constellation_metrics_cf
   gnuradio.trellis.encoder_bb
   gnuradio.trellis.encoder_bi
   gnuradio.trellis.encoder_bs
   gnuradio.trellis.encoder_ii
   gnuradio.trellis.encoder_si
   gnuradio.trellis.encoder_ss
   gnuradio.trellis.metrics_c
   gnuradio.trellis.metrics_f
   gnuradio.trellis.metrics_i
   gnuradio.trellis.metrics_s
   gnuradio.trellis.pccc_decoder_b
   gnuradio.trellis.pccc_decoder_combined_cb
   gnuradio.trellis.pccc_decoder_combined_ci
   gnuradio.trellis.pccc_decoder_combined_cs
   gnuradio.trellis.pccc_decoder_combined_fb
   gnuradio.trellis.pccc_decoder_combined_fi
   gnuradio.trellis.pccc_decoder_combined_fs
   gnuradio.trellis.pccc_decoder_i
   gnuradio.trellis.pccc_decoder_s
   gnuradio.trellis.pccc_encoder_bb
   gnuradio.trellis.pccc_encoder_bi
   gnuradio.trellis.pccc_encoder_bs
   gnuradio.trellis.pccc_encoder_ii
   gnuradio.trellis.pccc_encoder_si
   gnuradio.trellis.pccc_encoder_ss
   gnuradio.trellis.permutation
   gnuradio.trellis.sccc_decoder_b
   gnuradio.trellis.sccc_decoder_combined_cb
   gnuradio.trellis.sccc_decoder_combined_ci
   gnuradio.trellis.sccc_decoder_combined_cs
   gnuradio.trellis.sccc_decoder_combined_fb
   gnuradio.trellis.sccc_decoder_combined_fi
   gnuradio.trellis.sccc_decoder_combined_fs
   gnuradio.trellis.sccc_decoder_i
   gnuradio.trellis.sccc_decoder_s
   gnuradio.trellis.sccc_encoder_bb
   gnuradio.trellis.sccc_encoder_bi
   gnuradio.trellis.sccc_encoder_bs
   gnuradio.trellis.sccc_encoder_ii
   gnuradio.trellis.sccc_encoder_si
   gnuradio.trellis.sccc_encoder_ss
   gnuradio.trellis.siso_combined_f
   gnuradio.trellis.siso_f
   gnuradio.trellis.viterbi_b
   gnuradio.trellis.viterbi_combined_cb
   gnuradio.trellis.viterbi_combined_ci
   gnuradio.trellis.viterbi_combined_cs
   gnuradio.trellis.viterbi_combined_fb
   gnuradio.trellis.viterbi_combined_fi
   gnuradio.trellis.viterbi_combined_fs
   gnuradio.trellis.viterbi_combined_ib
   gnuradio.trellis.viterbi_combined_ii
   gnuradio.trellis.viterbi_combined_is
   gnuradio.trellis.viterbi_combined_sb
   gnuradio.trellis.viterbi_combined_si
   gnuradio.trellis.viterbi_combined_ss
   gnuradio.trellis.viterbi_i
   gnuradio.trellis.viterbi_s



Type Converter Blocks
---------------------

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
   gnuradio.blocks.interleaved_char_to_complex
   gnuradio.blocks.interleaved_short_to_complex
   gnuradio.blocks.short_to_char
   gnuradio.blocks.short_to_float
   gnuradio.blocks.uchar_to_float



UHD Blocks
----------

.. autosummary::
   :nosignatures:

   gnuradio.uhd.amsg_source
   gnuradio.uhd.usrp_sink
   gnuradio.uhd.usrp_source


Video Blocks
------------

.. autosummary::
   :nosignatures:

   gnuradio.video_sdl.sink_s
   gnuradio.video_sdl.sink_uc



Waveform Generator Blocks
-------------------------

   gnuradio.analog.fastnoise_source_c
   gnuradio.analog.fastnoise_source_f
   gnuradio.analog.fastnoise_source_i
   gnuradio.analog.fastnoise_source_s
   gnuradio.analog.noise_source_c
   gnuradio.analog.noise_source_f
   gnuradio.analog.noise_source_i
   gnuradio.analog.noise_source_s
   gnuradio.digital.glfsr_source_b
   gnuradio.digital.glfsr_source_f
   gnuradio.analog.sig_source_c
   gnuradio.analog.sig_source_f
   gnuradio.analog.sig_source_i
   gnuradio.analog.sig_source_s


ZeroMQ Interface Blocks
-----------------------

.. autosummary::
   :nosignatures:

   gnuradio.zeromq.pub_msg_sink
   gnuradio.zeromq.pub_sink
   gnuradio.zeromq.pull_msg_source
   gnuradio.zeromq.pull_source
   gnuradio.zeromq.push_msg_sink
   gnuradio.zeromq.push_sink
   gnuradio.zeromq.rep_msg_sink
   gnuradio.zeromq.rep_sink
   gnuradio.zeromq.req_msg_source
   gnuradio.zeromq.req_source
   gnuradio.zeromq.sub_msg_source
   gnuradio.zeromq.sub_source


Helper Classes: Analog
----------------------

.. autosummary::
   :nosignatures:

   gnuradio.analog.cpm
   gnuradio.analog.squelch_base_cc
   gnuradio.analog.squelch_base_ff
   gnuradio.analog.cpm
   gnuradio.analog.squelch_base_cc
   gnuradio.analog.squelch_base_ff
   gnuradio.analog.am_demod_cf
   gnuradio.analog.demod_10k0a3e_cf
   gnuradio.analog.fm_demod_cf
   gnuradio.analog.demod_20k0f3e_cf
   gnuradio.analog.demod_200kf3e_cf
   gnuradio.analog.fm_deemph
   gnuradio.analog.fm_preemph
   gnuradio.analog.nbfm_rx
   gnuradio.analog.nbfm_tx
   gnuradio.analog.ctcss_gen_f
   gnuradio.analog.standard_squelch
   gnuradio.analog.wfm_rcv_fmdet
   gnuradio.analog.wfm_rcv_pll
   gnuradio.analog.wfm_rcv
   gnuradio.analog.wfm_tx


Helper Classes: Digital
-----------------------

.. autosummary::
   :nosignatures:

   gnuradio.digital.constellation
   gnuradio.digital.lfsr
   gnuradio.digital.mpsk_snr_est
   gnuradio.digital.simple_framer
   gnuradio.digital.crc32
   gnuradio.digital.update_crc32
   gnuradio.digital.bpsk_mod
   gnuradio.digital.bpsk_demod
   gnuradio.digital.dbpsk_mod
   gnuradio.digital.dbpsk_demod
   gnuradio.digital.constellation_map_generator
   gnuradio.digital.cpm_mod
   gnuradio.digital.gen_and_append_crc32
   gnuradio.digital.check_crc32
   gnuradio.digital.generic_mod
   gnuradio.digital.generic_demod
   gnuradio.digital.gfsk_mod
   gnuradio.digital.gfsk_demod
   gnuradio.digital.gmsk_mod
   gnuradio.digital.gmsk_demod
   gnuradio.digital.type_1_mods
   gnuradio.digital.add_type_1_mod
   gnuradio.digital.type_1_demods
   gnuradio.digital.add_type_1_demod
   gnuradio.digital.type_1_constellations
   gnuradio.digital.add_type_1_constellation
   gnuradio.digital.extract_kwargs_from_options
   gnuradio.digital.extract_kwargs_from_options_for_class
   gnuradio.digital.ofdm_packet_utils.conv_packed_binary_string_to_1_0_string
   gnuradio.digital.ofdm_packet_utils.conv_1_0_string_to_packed_binary_string
   gnuradio.digital.ofdm_packet_utils.is_1_0_string
   gnuradio.digital.ofdm_packet_utils.string_to_hex_list
   gnuradio.digital.ofdm_packet_utils.whiten
   gnuradio.digital.ofdm_packet_utils.dewhiten
   gnuradio.digital.ofdm_packet_utils.make_header
   gnuradio.digital.ofdm_packet_utils.make_packet
   gnuradio.digital.ofdm_packet_utils.unmake_packet
   gnuradio.digital.ofdm_mod
   gnuradio.digital.ofdm_demod
   gnuradio.digital.ofdm_receiver
   gnuradio.digital.ofdm_sync_fixed
   gnuradio.digital.ofdm_sync_ml
   gnuradio.digital.ofdm_sync_pnac
   gnuradio.digital.ofdm_sync_pn
   gnuradio.digital.ofdm_tx
   gnuradio.digital.ofdm_rx
   gnuradio.digital.packet_utils.conv_packed_binary_string_to_1_0_string
   gnuradio.digital.packet_utils.conv_1_0_string_to_packed_binary_string
   gnuradio.digital.packet_utils.is_1_0_string
   gnuradio.digital.packet_utils.string_to_hex_list
   gnuradio.digital.packet_utils.whiten
   gnuradio.digital.packet_utils.dewhiten
   gnuradio.digital.packet_utils.make_header
   gnuradio.digital.packet_utils.make_packet
   gnuradio.digital.packet_utils.unmake_packet
   gnuradio.digital.mod_pkts
   gnuradio.digital.demod_pkts
   gnuradio.digital.psk_2_0x0
   gnuradio.digital.psk_2_0x1
   gnuradio.digital.sd_psk_2_0x0
   gnuradio.digital.sd_psk_2_0x1
   gnuradio.digital.psk_4_0x0_0_1
   gnuradio.digital.psk_4_0x1_0_1
   gnuradio.digital.psk_4_0x2_0_1
   gnuradio.digital.psk_4_0x3_0_1
   gnuradio.digital.psk_4_0x0_1_0
   gnuradio.digital.psk_4_0x1_1_0
   gnuradio.digital.psk_4_0x2_1_0
   gnuradio.digital.psk_4_0x3_1_0
   gnuradio.digital.sd_psk_4_0x0_0_1
   gnuradio.digital.sd_psk_4_0x1_0_1
   gnuradio.digital.sd_psk_4_0x2_0_1
   gnuradio.digital.sd_psk_4_0x3_0_1
   gnuradio.digital.sd_psk_4_0x0_1_0
   gnuradio.digital.sd_psk_4_0x1_1_0
   gnuradio.digital.sd_psk_4_0x2_1_0
   gnuradio.digital.sd_psk_4_0x3_1_0
   gnuradio.digital.psk_constellation
   gnuradio.digital.psk_mod
   gnuradio.digital.psk_demod
   gnuradio.digital.qam_16_0x0_0_1_2_3
   gnuradio.digital.qam_16_0x1_0_1_2_3
   gnuradio.digital.qam_16_0x2_0_1_2_3
   gnuradio.digital.qam_16_0x3_0_1_2_3
   gnuradio.digital.qam_16_0x0_1_0_2_3
   gnuradio.digital.qam_16_0x1_1_0_2_3
   gnuradio.digital.qam_16_0x2_1_0_2_3
   gnuradio.digital.qam_16_0x3_1_0_2_3
   gnuradio.digital.sd_qam_16_0x0_0_1_2_3
   gnuradio.digital.sd_qam_16_0x1_0_1_2_3
   gnuradio.digital.sd_qam_16_0x2_0_1_2_3
   gnuradio.digital.sd_qam_16_0x3_0_1_2_3
   gnuradio.digital.sd_qam_16_0x0_1_0_2_3
   gnuradio.digital.sd_qam_16_0x1_1_0_2_3
   gnuradio.digital.sd_qam_16_0x2_1_0_2_3
   gnuradio.digital.sd_qam_16_0x3_1_0_2_3
   gnuradio.digital.qam32_holeinside_constellation
   gnuradio.digital.make_differential_constellation
   gnuradio.digital.make_non_differential_constellation
   gnuradio.digital.qam_constellation
   gnuradio.digital.qam_mod
   gnuradio.digital.qam_demod
   gnuradio.digital.qpsk_constellation
   gnuradio.digital.qpsk_mod
   gnuradio.digital.qpsk_demod
   gnuradio.digital.dqpsk_constellation
   gnuradio.digital.dqpsk_mod
   gnuradio.digital.dqpsk_demod
   gnuradio.digital.soft_dec_table_generator
   gnuradio.digital.soft_dec_table
   gnuradio.digital.calc_soft_dec_from_table
   gnuradio.digital.calc_soft_dec
   gnuradio.digital.show_table

Helper Classes: FEC
-------------------

.. autosummary::
   :nosignatures:

   gnuradio.fec.cc_decoder
   gnuradio.fec.cc_encoder
   gnuradio.fec.ccsds_encoder
   gnuradio.fec.dummy_decoder
   gnuradio.fec.dummy_encoder
   gnuradio.fec.ldpc_decoder
   gnuradio.fec.ldpc_encoder
   gnuradio.fec.repetition_decoder
   gnuradio.fec.repetition_encoder
   gnuradio.fec.tpc_decoder
   gnuradio.fec.tpc_encoder
   gnuradio.fec.bercurve_generator
   gnuradio.fec.bitreverse
   gnuradio.fec.bitflip
   gnuradio.fec.read_bitlist
   gnuradio.fec.read_big_bitlist
   gnuradio.fec.generate_symmetries
   gnuradio.fec.capillary_threaded_decoder
   gnuradio.fec.capillary_threaded_encoder
   gnuradio.fec.extended_async_encoder
   gnuradio.fec.extended_decoder
   gnuradio.fec.extended_encoder
   gnuradio.fec.extended_tagged_decoder
   gnuradio.fec.extended_tagged_encoder
   gnuradio.fec.fec_test
   gnuradio.fec.threaded_decoder
   gnuradio.fec.threaded_encoder

Helper Classes: FFT
-------------------

.. autosummary::
   :nosignatures:

   gnuradio.fft.window


Helper Classes: Filter
----------------------

.. autosummary::
   :nosignatures:

   gnuradio.filter.filterbank.analysis_filterbank
   gnuradio.filter.filterbank.synthesis_filterbank
   gnuradio.filter.firdes
   gnuradio.filter.pm_remez
   gnuradio.filter.synthesis_filterbank
   gnuradio.filter.analysis_filterbank
   gnuradio.filter.freq_xlating_fft_filter_ccc
   gnuradio.filter.optfir.low_pass
   gnuradio.filter.optfir.band_pass
   gnuradio.filter.optfir.complex_band_pass
   gnuradio.filter.optfir.band_reject
   gnuradio.filter.optfir.stopband_atten_to_dev
   gnuradio.filter.optfir.passband_ripple_to_dev
   gnuradio.filter.optfir.remezord
   gnuradio.filter.optfir.lporder
   gnuradio.filter.optfir.bporder
   gnuradio.filter.pfb.channelizer_ccf
   gnuradio.filter.pfb.interpolator_ccf
   gnuradio.filter.pfb.decimator_ccf
   gnuradio.filter.pfb.arb_resampler_ccf
   gnuradio.filter.pfb.arb_resampler_fff
   gnuradio.filter.pfb.arb_resampler_ccc
   gnuradio.filter.pfb.channelizer_hier_ccf
   gnuradio.filter.rational_resampler_fff
   gnuradio.filter.rational_resampler_ccf
   gnuradio.filter.rational_resampler_ccc


Helper Classes: Trellis
-----------------------

.. autosummary::
   :nosignatures:

   gnuradio.trellis.fsm
   gnuradio.trellis.interleaver


Helper Classes: UHD
-------------------

.. autosummary::
   :nosignatures:

   gnuradio.uhd.usrp_block


Helper Classes: Vocoder
-----------------------

.. autosummary::
   :nosignatures:

   gnuradio.vocoder.codec2
   gnuradio.vocoder.cvsd_encode_fb
   gnuradio.vocoder.cvsd_decode_bf


Helper Classes: WXGUI
---------------------

.. autosummary::
   :nosignatures:

   gnuradio.wxgui.oscope_sink_x
   gnuradio.wxgui.histo_sink_f
