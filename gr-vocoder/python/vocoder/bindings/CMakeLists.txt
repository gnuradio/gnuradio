########################################################################
# Python Bindings
########################################################################

pybind11_add_module(vocoder_python
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
    target_sources(vocoder_python PRIVATE
    codec2_python.cc
    codec2_decode_ps_python.cc
    codec2_encode_sp_python.cc
    )
endif(LIBCODEC2_FOUND)

if(LIBCODEC2_HAS_FREEDV_API)
    target_sources(vocoder_python PRIVATE
    freedv_rx_ss_python.cc
    freedv_tx_ss_python.cc
    )
endif(LIBCODEC2_HAS_FREEDV_API)

if(LIBGSM_FOUND)
    target_sources(vocoder_python PRIVATE
    gsm_fr_decode_ps_python.cc
    gsm_fr_encode_sp_python.cc
    )
endif(LIBGSM_FOUND)

target_sources(vocoder_python PRIVATE
    python_bindings.cc)

configure_file(${CMAKE_SOURCE_DIR}/docs/doxygen/pydoc_macros.h ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

if(ENABLE_DOXYGEN)
    add_custom_command( 
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docstring_status
        COMMAND python3 ${CMAKE_SOURCE_DIR}/docs/doxygen/update_pydoc.py "sub"
        "--json_path" ${CMAKE_BINARY_DIR}/docs/doxygen/gnuradio_docstrings.json
        "--bindings_dir" ${CMAKE_CURRENT_SOURCE_DIR}/docstrings
        "--output_dir" ${CMAKE_CURRENT_BINARY_DIR}
        "--filter" gr::vocoder
        COMMENT "Adding docstrings into vocoder pybind headers ..."
        DEPENDS gnuradio_docstrings gnuradio-vocoder)
    add_custom_target(vocoder_docstrings ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/docstring_status)
else(ENABLE_DOXYGEN)
    add_custom_command( 
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docstring_status
        COMMAND python3 ${CMAKE_SOURCE_DIR}/docs/doxygen/update_pydoc.py "copy"
        "--bindings_dir" ${CMAKE_CURRENT_SOURCE_DIR}/docstrings
        "--output_dir" ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Copying vocoder docstring templates as pybind headers ...")
    add_custom_target(vocoder_docstrings ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/docstring_status)
endif(ENABLE_DOXYGEN)

target_include_directories(vocoder_python PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PYTHON_NUMPY_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${PYBIND11_INCLUDE_DIR}
)
target_link_libraries(vocoder_python PUBLIC ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} gnuradio-runtime gnuradio-vocoder)
target_compile_options(vocoder_python PRIVATE -Wno-unused-variable) # disable warnings for docstring templates
add_dependencies(vocoder_python vocoder_docstrings)

install(TARGETS vocoder_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/vocoder COMPONENT pythonapi)
