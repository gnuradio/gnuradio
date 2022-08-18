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

gen_block_yml(gen_uhd_usrp_blocks.py uhd_usrp_source.block.yml)
gen_block_yml(gen_uhd_usrp_blocks.py uhd_usrp_sink.block.yml)

add_custom_target(uhd_grc_yml_blocks ALL DEPENDS ${yml_blocks})

install(FILES ${yml_blocks} uhd_amsg_source.block.yml uhd.tree.yml
        DESTINATION ${GRC_BLOCKS_DIR})

if(ENABLE_UHD_RFNOC)
    install(
        FILES rfnoc.domain.yml
              uhd_fpga_clk.domain.yml
              uhd_fpga_ctrl.domain.yml
              uhd_fpga_data.domain.yml
              uhd_fpga_ddc.block.yml
              uhd_fpga_duc.block.yml
              uhd_fpga_fft.block.yml
              uhd_fpga_io_ctrl_port.domain.yml
              uhd_fpga_io_radio.domain.yml
              uhd_fpga_io_time_keeper.domain.yml
              uhd_fpga_radio.block.yml
              uhd_fpga_sep.block.yml
              uhd_fpga_x310.block.yml
              uhd_rfnoc_ddc.block.yml
              uhd_rfnoc_duc.block.yml
              uhd_rfnoc_fft.block.yml
              uhd_rfnoc_graph.block.yml
              uhd_rfnoc_rx_radio.block.yml
              uhd_rfnoc_rx_streamer.block.yml
              uhd_rfnoc_tx_radio.block.yml
              uhd_rfnoc_tx_streamer.block.yml
        DESTINATION ${GRC_BLOCKS_DIR})
endif(ENABLE_UHD_RFNOC)
