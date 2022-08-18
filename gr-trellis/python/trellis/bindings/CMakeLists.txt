include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(
    APPEND
    trellis_python_files
    # base_python.cc
    # calc_metric_python.cc
    constellation_metrics_cf_python.cc
    # core_algorithms_python.cc
    encoder_python.cc
    fsm_python.cc
    interleaver_python.cc
    metrics_python.cc
    pccc_decoder_blk_python.cc
    pccc_decoder_combined_blk_python.cc
    pccc_encoder_python.cc
    permutation_python.cc
    quicksort_index_python.cc
    sccc_decoder_blk_python.cc
    sccc_decoder_combined_blk_python.cc
    sccc_encoder_python.cc
    siso_combined_f_python.cc
    siso_f_python.cc
    siso_type_python.cc
    viterbi_python.cc
    viterbi_combined_python.cc
    python_bindings.cc)

gr_pybind_make_check_hash(trellis ../../.. gr::trellis "${trellis_python_files}")

install(
    TARGETS trellis_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/trellis
    COMPONENT pythonapi)
