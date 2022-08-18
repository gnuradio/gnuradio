include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(
    APPEND
    analog_python_files
    agc_python.cc
    # agc2_python.cc
    agc2_cc_python.cc
    agc2_ff_python.cc
    agc3_cc_python.cc
    agc_cc_python.cc
    agc_ff_python.cc
    cpfsk_bc_python.cc
    cpm_python.cc
    ctcss_squelch_ff_python.cc
    dpll_bb_python.cc
    fastnoise_source_python.cc
    feedforward_agc_cc_python.cc
    fmdet_cf_python.cc
    frequency_modulator_fc_python.cc
    noise_source_python.cc
    noise_type_python.cc
    phase_modulator_fc_python.cc
    pll_carriertracking_cc_python.cc
    pll_freqdet_cf_python.cc
    pll_refout_cc_python.cc
    probe_avg_mag_sqrd_c_python.cc
    probe_avg_mag_sqrd_cf_python.cc
    probe_avg_mag_sqrd_f_python.cc
    pwr_squelch_cc_python.cc
    pwr_squelch_ff_python.cc
    quadrature_demod_cf_python.cc
    rail_ff_python.cc
    random_uniform_source_python.cc
    sig_source_python.cc
    sig_source_waveform_python.cc
    simple_squelch_cc_python.cc
    squelch_base_cc_python.cc
    squelch_base_ff_python.cc
    python_bindings.cc)

gr_pybind_make_check_hash(analog ../../.. gr::analog "${analog_python_files}")

install(
    TARGETS analog_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/analog
    COMPONENT pythonapi)
