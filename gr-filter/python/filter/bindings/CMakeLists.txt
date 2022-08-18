########################################################################
# Python Bindings
########################################################################

list(
    APPEND
    filter_python_files
    dc_blocker_cc_python.cc
    dc_blocker_ff_python.cc
    fft_filter_python.cc
    fft_filter_ccc_python.cc
    fft_filter_ccf_python.cc
    fft_filter_fff_python.cc
    filter_delay_fc_python.cc
    filterbank_python.cc
    filterbank_vcvcf_python.cc
    fir_filter_python.cc
    fir_filter_blk_python.cc
    fir_filter_with_buffer_python.cc
    firdes_python.cc
    freq_xlating_fir_filter_python.cc
    hilbert_fc_python.cc
    # iir_filter_python.cc
    iir_filter_ccc_python.cc
    iir_filter_ccd_python.cc
    iir_filter_ccf_python.cc
    iir_filter_ccz_python.cc
    iir_filter_ffd_python.cc
    # interp_differentiator_taps_python.cc
    interp_fir_filter_python.cc
    # interpolator_taps_python.cc
    ival_decimator_python.cc
    mmse_fir_interpolator_cc_python.cc
    mmse_fir_interpolator_ff_python.cc
    mmse_interp_differentiator_cc_python.cc
    mmse_interp_differentiator_ff_python.cc
    mmse_resampler_cc_python.cc
    mmse_resampler_ff_python.cc
    pfb_arb_resampler_python.cc
    pfb_arb_resampler_ccc_python.cc
    pfb_arb_resampler_ccf_python.cc
    pfb_arb_resampler_fff_python.cc
    pfb_channelizer_ccf_python.cc
    pfb_decimator_ccf_python.cc
    pfb_interpolator_ccf_python.cc
    pfb_synthesizer_ccf_python.cc
    pm_remez_python.cc
    polyphase_filterbank_python.cc
    rational_resampler_python.cc
    single_pole_iir_python.cc
    single_pole_iir_filter_cc_python.cc
    single_pole_iir_filter_ff_python.cc
    python_bindings.cc)

gr_pybind_make_check_hash(filter ../../.. gr::filter "${filter_python_files}")

install(
    TARGETS filter_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/filter
    COMPONENT pythonapi)
