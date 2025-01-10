include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(
    APPEND
    gr_python_files
    messages/msg_accepter_msgq_python.cc
    messages/msg_accepter_python.cc
    messages/msg_passing_python.cc
    messages/msg_producer_python.cc
    messages/msg_queue_python.cc
    # attributes_python.cc
    basic_block_python.cc
    block_python.cc
    block_gateway_python.cc
    # block_registry_python.cc
    buffer_python.cc
    buffer_reader_python.cc
    buffer_type_python.cc
    constants_python.cc
    dictionary_logger_backend_python.cc
    endianness_python.cc
    expj_python.cc
    flowgraph_python.cc
    fxpt_python.cc
    fxpt_nco_python.cc
    fxpt_vco_python.cc
    # gr_complex_python.cc
    hier_block2_python.cc
    high_res_timer_python.cc
    io_signature_python.cc
    logger_python.cc
    math_python.cc
    message_python.cc
    msg_accepter_python.cc
    # msg_accepter_msgq_python.cc
    # msg_passing_python.cc
    # msg_producer_python.cc
    msg_queue_python.cc
    # misc_python.cc
    msg_accepter_python.cc
    msg_handler_python.cc
    msg_queue_python.cc
    nco_python.cc
    pdu_python.cc
    prefs_python.cc
    # pycallback_object_python.cc
    random_python.cc
    realtime_python.cc
    runtime_types_python.cc
    sincos_python.cc
    sptr_magic_python.cc
    sync_block_python.cc
    sync_decimator_python.cc
    sync_interpolator_python.cc
    sys_paths_python.cc
    tagged_stream_block_python.cc
    tags_python.cc
    # thread_python.cc
    # thread_body_wrapper_python.cc
    # thread_group_python.cc
    top_block_python.cc
    # types_python.cc
    # unittests_python.cc
    # xoroshiro128p_python.cc
    python_bindings.cc)

if(ENABLE_GR_CTRLPORT)
    list(APPEND gr_python_files 
        # rpcbufferedget_python.cc
        # rpcmanager_base_python.cc
        # rpcpmtconverters_thrift_python.cc
        # rpcregisterhelpers_python.cc
        # rpcserver_aggregator_python.cc
        # rpcserver_base_python.cc
        # rpcserver_booter_aggregator_python.cc
        # rpcserver_booter_thrift_python.cc
        # rpcserver_selector_python.cc
        # rpcserver_thrift_python.cc
        # thrift_application_base_python.cc
        # thrift_server_template_python.cc
        rpccallbackregister_base_python.cc
        rpcmanager_python.cc
        rpcserver_booter_base_python.cc
        )
endif()

gr_pybind_make_check_hash(gr ../../../.. gr::gr "${gr_python_files}")

if(ENABLE_GR_CTRLPORT)
    target_compile_definitions(gr_python PUBLIC GR_HAVE_CTRLPORT)
endif()

install(
    TARGETS gr_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/gr
    COMPONENT pythonapi)
