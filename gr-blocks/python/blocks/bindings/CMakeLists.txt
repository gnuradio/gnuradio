include(GrPybind)

########################################################################
# Python Bindings
########################################################################
list(APPEND blocks_python_files
    abs_blk_python.cc 56dfda2d14a2aa7945b7b44cd10aa8a0 abs_blk.h
    add_blk_python.cc 7a377b71d887e8365cea70fa948eb781 add_blk.h
    add_const_bb_python.cc 12a3e0160a90ec0bf2237c7d318536c1 add_const_bb.h
    add_const_cc_python.cc 7415677461c812df752b11ea03fedc93 add_const_cc.h
    add_const_ff_python.cc 38f26cf5ffa1dccfded03ad4d67d7639 add_const_ff.h
    add_const_ii_python.cc 07dcb9a0dbb2ac9d6c4cb355594bd587 add_const_ii.h
    add_const_ss_python.cc dcb9f59db6f890ca70750f6d8e694a7f add_const_ss.h
    add_const_v_python.cc 17a5a0f4ceba6f827de7f9712c2da4a7 add_const_v.h
    and_blk_python.cc 2dff82c26e72210a4974591027d4d3ee and_blk.h
    and_const_python.cc 25f3749f66b4023cb6da59760d8a32a4 and_const.h
    annotator_1to1_python.cc fc2f58c4139c529c420fb00edf3d02cb annotator_1to1.h
    annotator_alltoall_python.cc 7f52cd6a04ed3c0db7a7daad5e620f9d annotator_alltoall.h
    annotator_raw_python.cc b7f1c3039dd27a3627908e88bb327aec annotator_raw.h
    argmax_python.cc d1f696b01777c3a394cab7eb839fa6b8 argmax.h
    burst_tagger_python.cc 45216b99c8f33d8a28cfef305929ee40 burst_tagger.h
    char_to_float_python.cc 2a181135e2930c76b18504bdb3ad76d3 char_to_float.h
    char_to_short_python.cc d0e6f2d45d0c8709759ca94d3fee7c8c char_to_short.h
    check_lfsr_32k_s_python.cc d603a0d15bc00d003a54581d21ba8b6c check_lfsr_32k_s.h
    complex_to_arg_python.cc 35dd8ab3256dd3539aa987e049bf05fd complex_to_arg.h
    complex_to_float_python.cc ecf25117b2e4b17610895ba582125f13 complex_to_float.h
    complex_to_imag_python.cc 0bfce7099bc7438e992f0cf95e067675 complex_to_imag.h
    complex_to_interleaved_char_python.cc 81c976146990590a8f8410f3f3e15589 complex_to_interleaved_char.h
    complex_to_interleaved_short_python.cc 7675863b566929be1be108249ccc4d08 complex_to_interleaved_short.h
    complex_to_mag_python.cc e223ad51580e7947d6e4bbcdc2df9cb5 complex_to_mag.h
    complex_to_mag_squared_python.cc bbc07c4648cb21825962ff680bf463b9 complex_to_mag_squared.h
    complex_to_magphase_python.cc 23c8edcc7a570fa727bde5379fed60be complex_to_magphase.h
    complex_to_real_python.cc 6c6ab2d8ebe9cc86ef4bd69ac9c221f4 complex_to_real.h
    conjugate_cc_python.cc 08e60dc91d49e52b278c1f657fb5c86e conjugate_cc.h
    control_loop_python.cc 26a23d368e328fb578707a7c18cafbe1 control_loop.h
    copy_python.cc 2bcca715e65b5b54394fe4fe08d7afa0 copy.h
    correctiq_python.cc b7a70d6a642844c078eadb3808f6ab21 correctiq.h
    correctiq_auto_python.cc 89cb798d694089e6c41ec905400c2555 correctiq_auto.h
    correctiq_man_python.cc a2671fc2a0e6c23d0265f059383d03c1 correctiq_man.h
    correctiq_swapiq_python.cc fe94f1e37a636f114ac56074111b937a correctiq_swapiq.h
    count_bits_python.cc da855d7f097f40a02197ebb030104cfc count_bits.h
    # ctrlport_probe2_b_python.cc 68ceb4f932be9cf0706cb20399d317db ctrlport_probe2_b.h
    # ctrlport_probe2_c_python.cc 97afe362bce3aff913d17babd0742a11 ctrlport_probe2_c.h
    # ctrlport_probe2_f_python.cc 53341903651f4aff84dcc42606d3e346 ctrlport_probe2_f.h
    # ctrlport_probe2_i_python.cc ce4e413b7a813ec65fd2887c50f27ea0 ctrlport_probe2_i.h
    # ctrlport_probe2_s_python.cc eeb578353e26770ec017cce77b17ecaa ctrlport_probe2_s.h
    # ctrlport_probe_c_python.cc 953e3c69bfcbde28327778d11ce02638 ctrlport_probe_c.h
    deinterleave_python.cc 2c0c4637acbf4e7d50538042b03cbcd3 deinterleave.h
    delay_python.cc 70c56a7741afcf16e8584454a0520342 delay.h
    divide_python.cc aba6049c528454ca838f70d73f072609 divide.h
    endian_swap_python.cc c613a81f29aa0d91d9d71514f1b08464 endian_swap.h
    exponentiate_const_cci_python.cc 50028657ded73693b9e0c62e27064b38 exponentiate_const_cci.h
    file_descriptor_sink_python.cc 044bae02323044b086e3bef7067598f9 file_descriptor_sink.h
    file_descriptor_source_python.cc f2d96fddb065808e7d03d8111e2c0e36 file_descriptor_source.h
    file_meta_sink_python.cc 124869add242e8f8a52cc8ae3a40da7f file_meta_sink.h
    file_meta_source_python.cc 689524753f7d659f05fd462802b55cac file_meta_source.h
    file_sink_python.cc 7d4fa47d3509d715bbf1dd940304cadf file_sink.h
    file_sink_base_python.cc 3ecad3bebf54f73e2fa57cb95d5154c8 file_sink_base.h
    file_source_python.cc 3ff3bf035951c8fd06dbb7456e7c7ba3 file_source.h
    float_to_char_python.cc 808c8d5e27971c1192a6d5090fb5074b float_to_char.h
    float_to_complex_python.cc a4ac419b65473f8c4130c7d53e4e70ee float_to_complex.h
    float_to_int_python.cc 2206f897d77a22c9b55605c35fc037c8 float_to_int.h
    float_to_short_python.cc 847d10fddd3eb75d0cbe403a313295d4 float_to_short.h
    float_to_uchar_python.cc 30cc9cfbb683f5c34d63bc65b4eb3e69 float_to_uchar.h
    head_python.cc a84eb75af0a93ad008abf4fd915a3c41 head.h
    int_to_float_python.cc 5cb27bada3d9184397017edcde96f063 int_to_float.h
    integrate_python.cc b6bcb8fd7c0af580007e18c72e7cbf5e integrate.h
    interleave_python.cc 554606d4e1f46e56fb61cb953b85bdb5 interleave.h
    interleaved_char_to_complex_python.cc 2740a76628c0e360672d28d8291c23bd interleaved_char_to_complex.h
    interleaved_short_to_complex_python.cc 7a3148b7b8425fff29e4e060b345e508 interleaved_short_to_complex.h
    keep_m_in_n_python.cc 4aa273b7eee4437ca89a45c3b0a56259 keep_m_in_n.h
    keep_one_in_n_python.cc 67da027d9b5c58de342296f623db1fe0 keep_one_in_n.h
    lfsr_15_1_0_python.cc 6b2bc514206aa15787a3e9d002269784 lfsr_15_1_0.h
    lfsr_32k_python.cc 4986998c4a85bc4255c729571c35f1af lfsr_32k.h
    lfsr_32k_source_s_python.cc 7c7db48cb74828d12745a2aae37a3be5 lfsr_32k_source_s.h
    # log2_const_python.cc f4afb8807dd68b6561896201a8489a0c log2_const.h
    magphase_to_complex_python.cc 9784aabc763faf927f5045c8b11dfa1b magphase_to_complex.h
    max_blk_python.cc 08425e1c067b66c33ac4ad0839cfa86f max_blk.h
    message_debug_python.cc d30bdecc7a85481cac1f2c6ff843509b message_debug.h
    message_strobe_python.cc 26fe83cd77443495d544b73038ddbc54 message_strobe.h
    message_strobe_random_python.cc a759e6e0483f67da0e1a9bdf8344f16e message_strobe_random.h
    min_blk_python.cc cd1a4a1e4517e5c6850fd500172218a1 min_blk.h
    moving_average_python.cc 6d76bcf0938271f111cd4bf9ee6803bd moving_average.h
    multiply_python.cc 2cb5abdaf264c2a83f1fb5d7ddf06601 multiply.h
    multiply_by_tag_value_cc_python.cc 83e16476793f2a5f6173d0bcd70cbf41 multiply_by_tag_value_cc.h
    multiply_conjugate_cc_python.cc 7998308f68231db2a3c906c86d88a193 multiply_conjugate_cc.h
    multiply_const_python.cc 86a7d253afa61b76a98f241a58e8104a multiply_const.h
    multiply_const_v_python.cc 41792403264b7a4d8f19670cc2945ed3 multiply_const_v.h
    multiply_matrix_python.cc e99d8b091b5c8078ced8bdc3933d211d multiply_matrix.h
    mute_python.cc 5f6137a7d522ab67d435e2ae8946b625 mute.h
    nlog10_ff_python.cc adf634cb0bafdebbdf595c09ab20aec9 nlog10_ff.h
    nop_python.cc c206b7435a51013204b4c6ef82caf92e nop.h
    not_blk_python.cc 0ae48ddf84b6a234e0696c1358e015d0 not_blk.h
    null_sink_python.cc 8e09529f4c20a55974e648e7cff8e8b7 null_sink.h
    null_source_python.cc a1c9234adbc8a74d674d27f6f702d04a null_source.h
    or_blk_python.cc 6e632b186f0f2cbe08578791fda647d8 or_blk.h
    pack_k_bits_python.cc 9417a28ce0ff8b790496cb1d33fca83f pack_k_bits.h
    pack_k_bits_bb_python.cc 36978280edb60b63713b9eef499f3873 pack_k_bits_bb.h
    packed_to_unpacked_python.cc ee978838329d1d8630428c4c7a7a402c packed_to_unpacked.h
    patterned_interleaver_python.cc 82fb740fd149d52a83278c47a2685541 patterned_interleaver.h
    pdu_python.cc 8c678dd50c4df86cbc74dc58b1b5010d pdu.h
    pdu_filter_python.cc 73336eabe264d9670a63e914e96742b1 pdu_filter.h
    pdu_remove_python.cc 62afe4d010e89d91f3276e3299231a4f pdu_remove.h
    pdu_set_python.cc 4716b531c993ca09cb8cdad1100e317a pdu_set.h
    pdu_to_tagged_stream_python.cc 122377a925d85481a5dd9f86e75e909b pdu_to_tagged_stream.h
    peak_detector_python.cc fb024c09ee4031c69e2b14a9d53bd6c9 peak_detector.h
    peak_detector2_fb_python.cc 7f3fbeb13715fe8bf8bcabe517fdc444 peak_detector2_fb.h
    # phase_shift_python.cc 5657bd1fe7366cf279f6b1e29ee4df6b phase_shift.h
    plateau_detector_fb_python.cc 8602ec9c626ae1fcba3aa2481f31dfd1 plateau_detector_fb.h
    probe_rate_python.cc 730ddf38e5152565930ff99c3ec187b6 probe_rate.h
    probe_signal_python.cc ea7457920b50e2aa80be0802cc74c757 probe_signal.h
    probe_signal_v_python.cc ae2b9e0357c56e58dc7eaba4fcc60339 probe_signal_v.h
    random_pdu_python.cc 31da08cfe8a287467bf91fd51612180f random_pdu.h
    regenerate_bb_python.cc 8172d7a94608583e0c259118f1864dd7 regenerate_bb.h
    repack_bits_bb_python.cc 1ebd11a8e196962e39d600285d88c967 repack_bits_bb.h
    repeat_python.cc e09b086b5c70662b6d397af93cdeb2e5 repeat.h
    rms_cf_python.cc 857196be4688540b40d75a53ab2a8919 rms_cf.h
    rms_ff_python.cc 1553aa52e02db6b444eb8e6bee68a2f3 rms_ff.h
    rotator_python.cc 00766f91fe492a48d5f40aa2082f31ff rotator.h
    rotator_cc_python.cc e127d40b39a00e6725b5f7214ef0e6d2 rotator_cc.h
    sample_and_hold_python.cc e2112809d670e502f4b8621039111a9f sample_and_hold.h
    selector_python.cc 1a1e76afdb837ef8d8bb28d6246efdb2 selector.h
    short_to_char_python.cc f4a0563327d4264db16d04514b38a738 short_to_char.h
    short_to_float_python.cc 7514aa143e4ccafbdb5263071965ea36 short_to_float.h
    skiphead_python.cc f323e03d857174538fe9bfe4f0128205 skiphead.h
    socket_pdu_python.cc 22b717395ed280a5f4f9ac0bd31d2986 socket_pdu.h
    stream_mux_python.cc e1b56bd092e0bc32cbbaa3b0688e6f55 stream_mux.h
    stream_to_streams_python.cc 80ded9031fc5493e8a87d4efde05b03e stream_to_streams.h
    stream_to_tagged_stream_python.cc c3cbfe6e43e3ab12170dcc91a5d067df stream_to_tagged_stream.h
    stream_to_vector_python.cc f128bc46c9d9e4de0207136c70b92de2 stream_to_vector.h
    streams_to_stream_python.cc edb5fbbac36c941884cc256fcef0af5a streams_to_stream.h
    streams_to_vector_python.cc 84587c2f1f7b6ec1a8f0da19a06555cc streams_to_vector.h
    stretch_ff_python.cc 80078e377cab0382f47419eb56896eb7 stretch_ff.h
    sub_python.cc f7474971ceeb31bc3e61111b0dad46d9 sub.h
    tag_debug_python.cc e0dab3de38b2e66a9c64f9af16535a7d tag_debug.h
    tag_gate_python.cc 34ea81b84d890dad9606b83101416d47 tag_gate.h
    tag_share_python.cc ddf2d42d7c903a68eb83efabdda39d2d tag_share.h
    tagged_file_sink_python.cc d30cd6ffad7c015e83199cc184780864 tagged_file_sink.h
    tagged_stream_align_python.cc 3197cf15ebe06b8ebd10e576722b187e tagged_stream_align.h
    tagged_stream_multiply_length_python.cc d35538f07d2fbd15d2b08a39de2090b3 tagged_stream_multiply_length.h
    tagged_stream_mux_python.cc 4b0618239f1da2693ba8632a5641ad63 tagged_stream_mux.h
    tagged_stream_to_pdu_python.cc b521afb9e6e75ef2df0909cebf85166e tagged_stream_to_pdu.h
    tags_strobe_python.cc 5341f2482a8549af151d3e13ac008e42 tags_strobe.h
    tcp_server_sink_python.cc 9f8a488c6a838cb20c718b515e039594 tcp_server_sink.h
    test_tag_variable_rate_ff_python.cc 1088dd79d0f93165dfc8e984b0963588 test_tag_variable_rate_ff.h
    threshold_ff_python.cc c84062668a3f19f25cfde9bd7b70e3ec threshold_ff.h
    throttle_python.cc e32468fa73eff1abd796a3fbf4dbb8e0 throttle.h
    transcendental_python.cc c9d5f211cd7186f80726caa5b7e0dd53 transcendental.h
    tsb_vector_sink_python.cc 45f65df7b4f05782ada6c21d6de13d2a tsb_vector_sink.h
    tuntap_pdu_python.cc b56c8af7086f7d50d4d768ab5e99b839 tuntap_pdu.h
    uchar_to_float_python.cc 066671763d5c224701465072ab82fa09 uchar_to_float.h
    udp_sink_python.cc b661fa9adabd289dfb26ffd01bab1a8d udp_sink.h
    udp_source_python.cc be808fc0ee5ca0d58c71e38a5734c955 udp_source.h
    unpack_k_bits_python.cc 28cc76516922bd2fae7b01a70c3fc9d1 unpack_k_bits.h
    unpack_k_bits_bb_python.cc 80ba4199aea9bdb398ffea40d2c88e98 unpack_k_bits_bb.h
    unpacked_to_packed_python.cc f38bf55a7331f33d1a4b4fffdc1a20fa unpacked_to_packed.h
    vco_c_python.cc 445e2e17cb46801f6438dde580176ef2 vco_c.h
    vco_f_python.cc 341cd117e68fcc9f4f8fd910f2c458e2 vco_f.h
    vector_insert_python.cc 3a72517895e75d2a08cc780a1da59acf vector_insert.h
    vector_map_python.cc 9c88a0b04c0e86a66c4c57ca843f1cd3 vector_map.h
    vector_sink_python.cc b92aa7ef4345ba35b812f6d053199c6c vector_sink.h
    vector_source_python.cc 1176d2a53ecb9fd3818f6f2559576a1e vector_source.h
    vector_to_stream_python.cc 4d47288cdcfe688510e55d95f64cea34 vector_to_stream.h
    vector_to_streams_python.cc e0a7722912ca2d7cc6dd3a89da23e038 vector_to_streams.h
    wavfile_python.cc 85a13746fd109dbc7b94ba38aaf61e4c wavfile.h
    wavfile_sink_python.cc b8184d24184e3bd7aa4a08f5fca8ac0d wavfile_sink.h
    wavfile_source_python.cc 01a042890c5fe1a8688deff99d957816 wavfile_source.h
    xor_blk_python.cc 9695bb06d84235a7cfcc67e7ada1f052 xor_blk.h
    python_bindings.cc)

GR_PYBIND_MAKE_CHECK_HASH(blocks 
  ../../.. 
  gr::blocks
  "${blocks_python_files}")

install(TARGETS blocks_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/blocks COMPONENT pythonapi)
