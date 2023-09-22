########################################################################
# Python Bindings
########################################################################

list(APPEND vocoder_python_files
    alaw_decode_bs_python.cc
    alaw_encode_sb_python.cc
    cvsd_decode_bs_python.cc
    cvsd_encode_sb_python.cc
    g721_decode_bs_python.cc
    g721_encode_sb_python.cc
    g723_24_decode_bs_python.cc
    g723_24_encode_sb_python.cc
    g723_40_decode_bs_python.cc
    g723_40_encode_sb_python.cc
    ulaw_decode_bs_python.cc
    ulaw_encode_sb_python.cc)

if(LIBCODEC2_FOUND)
    list(APPEND vocoder_python_files
    codec2_python.cc
    codec2_decode_ps_python.cc
    codec2_encode_sp_python.cc
    )
endif(LIBCODEC2_FOUND)

if(LIBCODEC2_HAS_FREEDV_API)
    list(APPEND vocoder_python_files
    freedv_api_python.cc
    freedv_rx_ss_python.cc
    freedv_tx_ss_python.cc
    )
endif(LIBCODEC2_HAS_FREEDV_API)

if(LIBGSM_FOUND)
    list(APPEND vocoder_python_files
    gsm_fr_decode_ps_python.cc
    gsm_fr_encode_sp_python.cc
    )
endif(LIBGSM_FOUND)

list(APPEND vocoder_python_files python_bindings.cc)

GR_PYBIND_MAKE_CHECK_HASH(vocoder
   ../../..
   gr::vocoder
   "${vocoder_python_files}")

if(LIBCODEC2_FOUND)
    target_compile_definitions(vocoder_python PRIVATE LIBCODEC2_FOUND)
endif(LIBCODEC2_FOUND)

if(LIBCODEC2_HAS_FREEDV_API)
    target_compile_definitions(vocoder_python PRIVATE LIBCODEC2_HAS_FREEDV_API)
endif(LIBCODEC2_HAS_FREEDV_API)

if(LIBGSM_FOUND)
    target_compile_definitions(vocoder_python PRIVATE LIBGSM_FOUND)
endif(LIBGSM_FOUND)

install(TARGETS vocoder_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/vocoder COMPONENT pythonapi)
