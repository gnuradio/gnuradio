--------------------------------------------------------------------------------
--     This file is owned and controlled by Xilinx and must be used           --
--     solely for design, simulation, implementation and creation of          --
--     design files limited to Xilinx devices or technologies. Use            --
--     with non-Xilinx devices or technologies is expressly prohibited        --
--     and immediately terminates your license.                               --
--                                                                            --
--     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"          --
--     SOLELY FOR USE IN DEVELOPING PROGRAMS AND SOLUTIONS FOR                --
--     XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION        --
--     AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE, APPLICATION            --
--     OR STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS              --
--     IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,                --
--     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE       --
--     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY               --
--     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE                --
--     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR         --
--     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF        --
--     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS        --
--     FOR A PARTICULAR PURPOSE.                                              --
--                                                                            --
--     Xilinx products are not intended for use in life support               --
--     appliances, devices, or systems. Use in such applications are          --
--     expressly prohibited.                                                  --
--                                                                            --
--     (c) Copyright 1995-2007 Xilinx, Inc.                                   --
--     All rights reserved.                                                   --
--------------------------------------------------------------------------------
-- You must compile the wrapper file fifo_xlnx_2Kx36_2clk.vhd when simulating
-- the core, fifo_xlnx_2Kx36_2clk. When compiling the wrapper file, be sure to
-- reference the XilinxCoreLib VHDL simulation library. For detailed
-- instructions, please refer to the "CORE Generator Help".

-- The synthesis directives "translate_off/translate_on" specified
-- below are supported by Xilinx, Mentor Graphics and Synplicity
-- synthesis tools. Ensure they are correct for your synthesis tool(s).

LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
-- synthesis translate_off
Library XilinxCoreLib;
-- synthesis translate_on
ENTITY fifo_xlnx_2Kx36_2clk IS
	port (
	din: IN std_logic_VECTOR(35 downto 0);
	rd_clk: IN std_logic;
	rd_en: IN std_logic;
	rst: IN std_logic;
	wr_clk: IN std_logic;
	wr_en: IN std_logic;
	dout: OUT std_logic_VECTOR(35 downto 0);
	empty: OUT std_logic;
	full: OUT std_logic;
	rd_data_count: OUT std_logic_VECTOR(11 downto 0);
	wr_data_count: OUT std_logic_VECTOR(11 downto 0));
END fifo_xlnx_2Kx36_2clk;

ARCHITECTURE fifo_xlnx_2Kx36_2clk_a OF fifo_xlnx_2Kx36_2clk IS
-- synthesis translate_off
component wrapped_fifo_xlnx_2Kx36_2clk
	port (
	din: IN std_logic_VECTOR(35 downto 0);
	rd_clk: IN std_logic;
	rd_en: IN std_logic;
	rst: IN std_logic;
	wr_clk: IN std_logic;
	wr_en: IN std_logic;
	dout: OUT std_logic_VECTOR(35 downto 0);
	empty: OUT std_logic;
	full: OUT std_logic;
	rd_data_count: OUT std_logic_VECTOR(11 downto 0);
	wr_data_count: OUT std_logic_VECTOR(11 downto 0));
end component;

-- Configuration specification 
	for all : wrapped_fifo_xlnx_2Kx36_2clk use entity XilinxCoreLib.fifo_generator_v4_3(behavioral)
		generic map(
			c_has_int_clk => 0,
			c_rd_freq => 1,
			c_wr_response_latency => 1,
			c_has_srst => 0,
			c_has_rd_data_count => 1,
			c_din_width => 36,
			c_has_wr_data_count => 1,
			c_full_flags_rst_val => 1,
			c_implementation_type => 2,
			c_family => "spartan3",
			c_use_embedded_reg => 0,
			c_has_wr_rst => 0,
			c_wr_freq => 1,
			c_use_dout_rst => 1,
			c_underflow_low => 0,
			c_has_meminit_file => 0,
			c_has_overflow => 0,
			c_preload_latency => 0,
			c_dout_width => 36,
			c_msgon_val => 1,
			c_rd_depth => 2048,
			c_default_value => "BlankString",
			c_mif_file_name => "BlankString",
			c_has_underflow => 0,
			c_has_rd_rst => 0,
			c_has_almost_full => 0,
			c_has_rst => 1,
			c_data_count_width => 12,
			c_has_wr_ack => 0,
			c_use_ecc => 0,
			c_wr_ack_low => 0,
			c_common_clock => 0,
			c_rd_pntr_width => 11,
			c_use_fwft_data_count => 1,
			c_has_almost_empty => 0,
			c_rd_data_count_width => 12,
			c_enable_rlocs => 0,
			c_wr_pntr_width => 11,
			c_overflow_low => 0,
			c_prog_empty_type => 0,
			c_optimization_mode => 0,
			c_wr_data_count_width => 12,
			c_preload_regs => 1,
			c_dout_rst_val => "0",
			c_has_data_count => 0,
			c_prog_full_thresh_negate_val => 2046,
			c_wr_depth => 2048,
			c_prog_empty_thresh_negate_val => 5,
			c_prog_empty_thresh_assert_val => 4,
			c_has_valid => 0,
			c_init_wr_pntr_val => 0,
			c_prog_full_thresh_assert_val => 2047,
			c_use_fifo16_flags => 0,
			c_has_backup => 0,
			c_valid_low => 0,
			c_prim_fifo_type => "2kx18",
			c_count_type => 0,
			c_prog_full_type => 0,
			c_memory_type => 1);
-- synthesis translate_on
BEGIN
-- synthesis translate_off
U0 : wrapped_fifo_xlnx_2Kx36_2clk
		port map (
			din => din,
			rd_clk => rd_clk,
			rd_en => rd_en,
			rst => rst,
			wr_clk => wr_clk,
			wr_en => wr_en,
			dout => dout,
			empty => empty,
			full => full,
			rd_data_count => rd_data_count,
			wr_data_count => wr_data_count);
-- synthesis translate_on

END fifo_xlnx_2Kx36_2clk_a;

