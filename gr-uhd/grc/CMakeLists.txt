# Copyright 2011,2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Rules for generating the source and sink yml wrappers
########################################################################
include(GrPython)

macro(GEN_BLOCK_YML _generator _yml_block)
    set(generator ${CMAKE_CURRENT_SOURCE_DIR}/${_generator})
    set(yml_block ${CMAKE_CURRENT_BINARY_DIR}/${_yml_block})
    list(APPEND yml_blocks ${yml_block})
    add_custom_command(
        DEPENDS ${generator}
        OUTPUT ${yml_block}
        COMMAND ${PYTHON_EXECUTABLE} ${generator} ${yml_block})
endmacro(GEN_BLOCK_YML)

GEN_BLOCK_YML(gen_rfnoc_ddc_block_yml.py uhd_rfnoc_ddc.block.yml)
GEN_BLOCK_YML(gen_rfnoc_duc_block_yml.py uhd_rfnoc_duc.block.yml)
GEN_BLOCK_YML(gen_rfnoc_fir_filter_block_yml.py uhd_rfnoc_fir_filter.block.yml)
GEN_BLOCK_YML(gen_rfnoc_replay_block_yml.py uhd_rfnoc_replay.block.yml)
GEN_BLOCK_YML(gen_rfnoc_rx_radio_block_yml.py uhd_rfnoc_rx_radio.block.yml)
GEN_BLOCK_YML(gen_rfnoc_siggen_block_yml.py uhd_rfnoc_siggen.block.yml)
GEN_BLOCK_YML(gen_rfnoc_tx_radio_block_yml.py uhd_rfnoc_tx_radio.block.yml)
GEN_BLOCK_YML(gen_rfnoc_window_block_yml.py uhd_rfnoc_window.block.yml)
GEN_BLOCK_YML(gen_uhd_usrp_blocks.py uhd_usrp_sink.block.yml)
GEN_BLOCK_YML(gen_uhd_usrp_blocks.py uhd_usrp_source.block.yml)

add_custom_target(uhd_grc_yml_blocks ALL DEPENDS ${yml_blocks})

install(FILES ${yml_blocks} uhd_amsg_source.block.yml uhd.tree.yml
        DESTINATION ${GRC_BLOCKS_DIR})

if(ENABLE_UHD_RFNOC)
    install(FILES
        # GRC Bindings for GNU Radio flow graphs
        uhd_rfnoc_addsub.block.yml
        uhd_rfnoc_fft.block.yml
        uhd_rfnoc_fosphor.block.yml
        uhd_rfnoc_graph.block.yml
        uhd_rfnoc_keep_one_in_n.block.yml
        uhd_rfnoc_logpwr.block.yml
        uhd_rfnoc_moving_avg.block.yml
        uhd_rfnoc_null_src_sink.block.yml
        uhd_rfnoc_rx_streamer.block.yml
        uhd_rfnoc_split_stream.block.yml
        uhd_rfnoc_switchboard.block.yml
        uhd_rfnoc_tx_streamer.block.yml
        uhd_rfnoc_vector_iir.block.yml
        # Domain for RFNoC block connections in GNU Radio flow graphs
        rfnoc.domain.yml
        # GRC Bindings for RFNoC Image Builder files (blocks)
        uhd_fpga_addsub.block.yml
        uhd_fpga_chdr_dma_ta.block.yml
        uhd_fpga_ddc.block.yml
        uhd_fpga_device_bsp.block.yml
        uhd_fpga_device_none.block.yml
        uhd_fpga_duc.block.yml
        uhd_fpga_eth_ta.block.yml
        uhd_fpga_fft.block.yml
        uhd_fpga_fir_filter.block.yml
        uhd_fpga_fosphor.block.yml
        uhd_fpga_keep_one_in_n.block.yml
        uhd_fpga_null_src_sink.block.yml
        uhd_fpga_radio.block.yml
        uhd_fpga_replay.block.yml
        uhd_fpga_sep.block.yml
        uhd_fpga_siggen.block.yml
        uhd_fpga_split_stream.block.yml
        uhd_fpga_switchboard.block.yml
        # GRC Bindings for RFNoC Image Builder files (domains)
        uhd_fpga_clk.domain.yml
        uhd_fpga_ctrl.domain.yml
        uhd_fpga_data.domain.yml
        uhd_fpga_io_axi4_mm.domain.yml
        uhd_fpga_io_ctrl_port.domain.yml
        uhd_fpga_io_gpio.domain.yml
        uhd_fpga_io_radio.domain.yml
        uhd_fpga_io_time_keeper.domain.yml
        uhd_fpga_io_xport.domain.yml
        # GRC workflow
        rfnoc_image_builder.workflow.yml
        DESTINATION ${GRC_BLOCKS_DIR})
    if(PYQT5_FOUND)
        install (FILES uhd_msgpushbutton.block.yml DESTINATION ${GRC_BLOCKS_DIR})
    endif(PYQT5_FOUND)
endif(ENABLE_UHD_RFNOC)
