include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND fec_python_files
    #alist_python.cc
    async_decoder_python.cc
    async_encoder_python.cc
    #awgn_bp_python.cc
    ber_bf_python.cc
    cc_common_python.cc
    cc_decoder_python.cc
    cc_encoder_python.cc
    ccsds_encoder_python.cc
    #cldpc_python.cc
    conv_bit_corr_bb_python.cc
    decode_ccsds_27_fb_python.cc
    decoder_python.cc
    depuncture_bb_python.cc
    dummy_decoder_python.cc
    dummy_encoder_python.cc
    encode_ccsds_27_bb_python.cc
    encoder_python.cc    
    generic_decoder_python.cc
    generic_encoder_python.cc
    #gf2mat_python.cc
    #gf2vec_python.cc
    ldpc_decoder_python.cc
    ldpc_encoder_python.cc
    #maxstar_python.cc
    polar_common_python.cc
    polar_decoder_common_python.cc
    polar_decoder_sc_python.cc
    polar_decoder_sc_list_python.cc
    polar_decoder_sc_systematic_python.cc
    polar_encoder_python.cc
    polar_encoder_systematic_python.cc
    puncture_bb_python.cc
    puncture_ff_python.cc
    repetition_decoder_python.cc
    repetition_encoder_python.cc
    #rs_python.cc
    tagged_decoder_python.cc
    tagged_encoder_python.cc
    # tpc_common_python.cc
    tpc_decoder_python.cc
    tpc_encoder_python.cc
    # #viterbi_python.cc
    )

if(GSL_FOUND)
    list(APPEND fec_python_files
        fec_mtrx_python.cc
        ldpc_G_matrix_python.cc
        ldpc_H_matrix_python.cc
        ldpc_bit_flip_decoder_python.cc
        ldpc_gen_mtrx_encoder_python.cc
        ldpc_par_mtrx_encoder_python.cc)
endif(GSL_FOUND)


list(APPEND fec_python_files
    python_bindings.cc)

GR_PYBIND_MAKE_CHECK_HASH(fec 
   ../../.. 
   gr::fec
   "${fec_python_files}") 

target_link_libraries(fec_python PUBLIC gnuradio-runtime gnuradio-blocks)
if(GSL_FOUND)
    target_compile_definitions(fec_python PUBLIC -DHAVE_GSL)
endif(GSL_FOUND)
install(TARGETS fec_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/fec COMPONENT pythonapi)
