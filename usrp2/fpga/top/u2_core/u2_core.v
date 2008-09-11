// ////////////////////////////////////////////////////////////////////////////////
// Module Name:    u2_core
// ////////////////////////////////////////////////////////////////////////////////

module u2_core
  #(parameter RAM_SIZE=16384)
  (// Clocks
   input dsp_clk,
   input wb_clk,
   output clock_ready,
   input clk_to_mac,
   input pps_in,
   
   // Misc, debug
   output [7:0] leds,
   output [31:0] debug,
   output [1:0] debug_clk,

   // Expansion
   input exp_pps_in,
   output exp_pps_out,
   
   // GMII
   //   GMII-CTRL
   input GMII_COL,
   input GMII_CRS,

   //   GMII-TX
   output [7:0] GMII_TXD,
   output GMII_TX_EN,
   output GMII_TX_ER,
   output GMII_GTX_CLK,
   input GMII_TX_CLK,  // 100mbps clk

   //   GMII-RX
   input [7:0] GMII_RXD,
   input GMII_RX_CLK,
   input GMII_RX_DV,
   input GMII_RX_ER,

   //   GMII-Management
   inout MDIO,
   output MDC,
   input PHY_INTn,   // open drain
   output PHY_RESETn,

   // SERDES
   output ser_enable,
   output ser_prbsen,
   output ser_loopen,
   output ser_rx_en,
   
   output ser_tx_clk,
   output [15:0] ser_t,
   output ser_tklsb,
   output ser_tkmsb,

   input ser_rx_clk,
   input [15:0] ser_r,
   input ser_rklsb,
   input ser_rkmsb,
   
   // CPLD interface
   output cpld_start,
   output cpld_mode,
   output cpld_done,
   input cpld_din,
   input cpld_clk,
   input cpld_detached,
   input cpld_misc,
   input cpld_init_b,
   input por,
   output config_success,
   
   // ADC
   input [13:0] adc_a,
   input adc_ovf_a,
   output adc_on_a,
   output adc_oe_a,
   
   input [13:0] adc_b,
   input adc_ovf_b,
   output adc_on_b,
   output adc_oe_b,
   
   // DAC
   output [15:0] dac_a,
   output [15:0] dac_b,

   // I2C
   input scl_pad_i,
   output scl_pad_o,
   output scl_pad_oen_o,
   input sda_pad_i,
   output sda_pad_o,
   output sda_pad_oen_o,
   
   // Clock Gen Control
   output [1:0] clk_en,
   output [1:0] clk_sel,
   input clk_func,        // FIXME is an input to control the 9510
   input clk_status,

   // Generic SPI
   output sclk,
   output mosi,
   input miso,
   output sen_clk,
   output sen_dac,
   output sen_tx_db,
   output sen_tx_adc,
   output sen_tx_dac,
   output sen_rx_db,
   output sen_rx_adc,
   output sen_rx_dac,
   
   // GPIO to DBoards
   inout [15:0] io_tx,
   inout [15:0] io_rx,

   // External RAM
   inout [17:0] RAM_D,
   output [18:0] RAM_A,
   output RAM_CE1n,
   output RAM_CENn,
   input RAM_CLK,
   output RAM_WEn,
   output RAM_OEn,
   output RAM_LDn,
   
   // Debug stuff
   output uart_tx_o, 
   input uart_rx_i,
   output uart_baud_o,
   input sim_mode,
   input [3:0] clock_divider
   );
   
   wire [7:0] 	set_addr;
   wire [31:0] 	set_data;
   wire 	set_stb;
   
   wire 	ram_loader_done;
   wire 	ram_loader_rst, wb_rst, dsp_rst;

   wire [31:0] 	status, status_b0, status_b1, status_b2, status_b3, status_b4, status_b5, status_b6, status_b7;
   wire 	bus_error, spi_int, i2c_int, pps_int, timer_int, buffer_int, proc_int, overrun, underrun, uart_tx_int, uart_rx_int;

   wire [31:0] 	debug_gpio_0, debug_gpio_1;
   wire [31:0] 	atr_lines;

   wire [31:0] 	debug_rx, debug_mac0, debug_mac1, debug_tx_dsp, debug_txc, 
		debug_serdes0, debug_serdes1, debug_serdes2, debug_rx_dsp;

   wire [15:0] 	ser_rx_occ, ser_tx_occ, dsp_rx_occ, dsp_tx_occ, eth_rx_occ, eth_tx_occ, eth_rx_occ2;
   wire 	ser_rx_full, ser_tx_full, dsp_rx_full, dsp_tx_full, eth_rx_full, eth_tx_full, eth_rx_full2;
   wire 	ser_rx_empty, ser_tx_empty, dsp_rx_empty, dsp_tx_empty, eth_rx_empty, eth_tx_empty, eth_rx_empty2;
	
   // ///////////////////////////////////////////////////////////////////////////////////////////////
   // Wishbone Single Master INTERCON
   parameter 	dw = 32;  // Data bus width
   parameter 	aw = 16;  // Address bus width, for byte addressibility, 16 = 64K byte memory space
   parameter 	sw = 4;   // Select width -- 32-bit data bus with 8-bit granularity.  
   
   wire [dw-1:0] m0_dat_o, m0_dat_i;
   wire [dw-1:0] s0_dat_o, s1_dat_o, s0_dat_i, s1_dat_i, s2_dat_o, s3_dat_o, s2_dat_i, s3_dat_i,
		 s4_dat_o, s5_dat_o, s4_dat_i, s5_dat_i, s6_dat_o, s7_dat_o, s6_dat_i, s7_dat_i,
		 s8_dat_o, s9_dat_o, s8_dat_i, s9_dat_i, s10_dat_o, s10_dat_i, s11_dat_i, s11_dat_o,
		 s12_dat_i, s12_dat_o, s13_dat_i, s13_dat_o;
   wire [aw-1:0] m0_adr,s0_adr,s1_adr,s2_adr,s3_adr,s4_adr,s5_adr,s6_adr,s7_adr,s8_adr,s9_adr,s10_adr,s11_adr,s12_adr, s13_adr;
   wire [sw-1:0] m0_sel,s0_sel,s1_sel,s2_sel,s3_sel,s4_sel,s5_sel,s6_sel,s7_sel,s8_sel,s9_sel,s10_sel,s11_sel,s12_sel, s13_sel;
   wire 	 m0_ack,s0_ack,s1_ack,s2_ack,s3_ack,s4_ack,s5_ack,s6_ack,s7_ack,s8_ack,s9_ack,s10_ack,s11_ack,s12_ack, s13_ack;
   wire 	 m0_stb,s0_stb,s1_stb,s2_stb,s3_stb,s4_stb,s5_stb,s6_stb,s7_stb,s8_stb,s9_stb,s10_stb,s11_stb,s12_stb, s13_stb;
   wire 	 m0_cyc,s0_cyc,s1_cyc,s2_cyc,s3_cyc,s4_cyc,s5_cyc,s6_cyc,s7_cyc,s8_cyc,s9_cyc,s10_cyc,s11_cyc,s12_cyc, s13_cyc;
   wire 	 m0_err,s0_err,s1_err,s2_err,s3_err,s4_err,s5_err,s6_err,s7_err,s8_err,s9_err,s10_err,s11_err,s12_err, s13_err;
   wire 	 m0_rty,s0_rty,s1_rty,s2_rty,s3_rty,s4_rty,s5_rty,s6_rty,s7_rty,s8_rty,s9_rty,s10_rty,s11_rty,s12_rty, s13_rty;
   wire 	 m0_we,s0_we,s1_we,s2_we,s3_we,s4_we,s5_we,s6_we,s7_we,s8_we,s9_we,s10_we,s11_we,s12_we,s13_we;
   
   wb_1master #(.s0_addr_w(1),.s0_addr(1'b0),.s1_addr_w(2),.s1_addr(2'b10),
		.s215_addr_w(6),.s2_addr(6'b1100_00),.s3_addr(6'b1100_01),.s4_addr(6'b1100_10),
		.s5_addr(6'b1100_11),.s6_addr(6'b1101_00),.s7_addr(6'b1101_01),.s8_addr(6'b1101_10),
		.s9_addr(6'b1101_11),.s10_addr(6'b1110_00),.s11_addr(6'b1110_01),.s12_addr(6'b1110_10),
		.s13_addr(6'b1110_11),.s14_addr(6'b1111_00),.s15_addr(6'b1111_01),
		.dw(dw),.aw(aw),.sw(sw)) wb_1master
     (.clk_i(wb_clk),.rst_i(wb_rst),       
      .m0_dat_o(m0_dat_o),.m0_ack_o(m0_ack),.m0_err_o(m0_err),.m0_rty_o(m0_rty),.m0_dat_i(m0_dat_i),
      .m0_adr_i(m0_adr),.m0_sel_i(m0_sel),.m0_we_i(m0_we),.m0_cyc_i(m0_cyc),.m0_stb_i(m0_stb),
      .s0_dat_o(s0_dat_o),.s0_adr_o(s0_adr),.s0_sel_o(s0_sel),.s0_we_o	(s0_we),.s0_cyc_o(s0_cyc),.s0_stb_o(s0_stb),
      .s0_dat_i(s0_dat_i),.s0_ack_i(s0_ack),.s0_err_i(s0_err),.s0_rty_i(s0_rty),
      .s1_dat_o(s1_dat_o),.s1_adr_o(s1_adr),.s1_sel_o(s1_sel),.s1_we_o	(s1_we),.s1_cyc_o(s1_cyc),.s1_stb_o(s1_stb),
      .s1_dat_i(s1_dat_i),.s1_ack_i(s1_ack),.s1_err_i(s1_err),.s1_rty_i(s1_rty),
      .s2_dat_o(s2_dat_o),.s2_adr_o(s2_adr),.s2_sel_o(s2_sel),.s2_we_o	(s2_we),.s2_cyc_o(s2_cyc),.s2_stb_o(s2_stb),
      .s2_dat_i(s2_dat_i),.s2_ack_i(s2_ack),.s2_err_i(s2_err),.s2_rty_i(s2_rty),
      .s3_dat_o(s3_dat_o),.s3_adr_o(s3_adr),.s3_sel_o(s3_sel),.s3_we_o	(s3_we),.s3_cyc_o(s3_cyc),.s3_stb_o(s3_stb),
      .s3_dat_i(s3_dat_i),.s3_ack_i(s3_ack),.s3_err_i(s3_err),.s3_rty_i(s3_rty),
      .s4_dat_o(s4_dat_o),.s4_adr_o(s4_adr),.s4_sel_o(s4_sel),.s4_we_o	(s4_we),.s4_cyc_o(s4_cyc),.s4_stb_o(s4_stb),
      .s4_dat_i(s4_dat_i),.s4_ack_i(s4_ack),.s4_err_i(s4_err),.s4_rty_i(s4_rty),
      .s5_dat_o(s5_dat_o),.s5_adr_o(s5_adr),.s5_sel_o(s5_sel),.s5_we_o	(s5_we),.s5_cyc_o(s5_cyc),.s5_stb_o(s5_stb),
      .s5_dat_i(s5_dat_i),.s5_ack_i(s5_ack),.s5_err_i(s5_err),.s5_rty_i(s5_rty),
      .s6_dat_o(s6_dat_o),.s6_adr_o(s6_adr),.s6_sel_o(s6_sel),.s6_we_o	(s6_we),.s6_cyc_o(s6_cyc),.s6_stb_o(s6_stb),
      .s6_dat_i(s6_dat_i),.s6_ack_i(s6_ack),.s6_err_i(s6_err),.s6_rty_i(s6_rty),
      .s7_dat_o(s7_dat_o),.s7_adr_o(s7_adr),.s7_sel_o(s7_sel),.s7_we_o	(s7_we),.s7_cyc_o(s7_cyc),.s7_stb_o(s7_stb),
      .s7_dat_i(s7_dat_i),.s7_ack_i(s7_ack),.s7_err_i(s7_err),.s7_rty_i(s7_rty),
      .s8_dat_o(s8_dat_o),.s8_adr_o(s8_adr),.s8_sel_o(s8_sel),.s8_we_o	(s8_we),.s8_cyc_o(s8_cyc),.s8_stb_o(s8_stb),
      .s8_dat_i(s8_dat_i),.s8_ack_i(s8_ack),.s8_err_i(s8_err),.s8_rty_i(s8_rty),
      .s9_dat_o(s9_dat_o),.s9_adr_o(s9_adr),.s9_sel_o(s9_sel),.s9_we_o	(s9_we),.s9_cyc_o(s9_cyc),.s9_stb_o(s9_stb),
      .s9_dat_i(s9_dat_i),.s9_ack_i(s9_ack),.s9_err_i(s9_err),.s9_rty_i(s9_rty),
      .s10_dat_o(s10_dat_o),.s10_adr_o(s10_adr),.s10_sel_o(s10_sel),.s10_we_o(s10_we),.s10_cyc_o(s10_cyc),.s10_stb_o(s10_stb),
      .s10_dat_i(s10_dat_i),.s10_ack_i(s10_ack),.s10_err_i(s10_err),.s10_rty_i(s10_rty),
      .s11_dat_o(s11_dat_o),.s11_adr_o(s11_adr),.s11_sel_o(s11_sel),.s11_we_o(s11_we),.s11_cyc_o(s11_cyc),.s11_stb_o(s11_stb),
      .s11_dat_i(s11_dat_i),.s11_ack_i(s11_ack),.s11_err_i(s11_err),.s11_rty_i(s11_rty),
      .s12_dat_o(s12_dat_o),.s12_adr_o(s12_adr),.s12_sel_o(s12_sel),.s12_we_o(s12_we),.s12_cyc_o(s12_cyc),.s12_stb_o(s12_stb),
      .s12_dat_i(s12_dat_i),.s12_ack_i(s12_ack),.s12_err_i(s12_err),.s12_rty_i(s12_rty),
      .s13_dat_o(s13_dat_o),.s13_adr_o(s13_adr),.s13_sel_o(s13_sel),.s13_we_o(s13_we),.s13_cyc_o(s13_cyc),.s13_stb_o(s13_stb),
      .s13_dat_i(s13_dat_i),.s13_ack_i(s13_ack),.s13_err_i(s13_err),.s13_rty_i(s13_rty),
      .s14_dat_i(0),.s14_ack_i(0),.s14_err_i(0),.s14_rty_i(0),
      .s15_dat_i(0),.s15_ack_i(0),.s15_err_i(0),.s15_rty_i(0)  );
   
   //////////////////////////////////////////////////////////////////////////////////////////
   // Reset Controller
   system_control sysctrl (.wb_clk_i(wb_clk), // .por_i(por),
			   .ram_loader_rst_o(ram_loader_rst),
			   .wb_rst_o(wb_rst),
			   .ram_loader_done_i(ram_loader_done));

   assign 	 config_success = ram_loader_done;
   
   // ///////////////////////////////////////////////////////////////////
   // RAM Loader

   wire [31:0] 	 ram_loader_dat, iwb_dat;
   wire [15:0] 	 ram_loader_adr, iwb_adr;
   wire [3:0] 	 ram_loader_sel;
   wire 	 ram_loader_stb, ram_loader_we, ram_loader_ack;
   wire 	 iwb_ack, iwb_stb;
   ram_loader #(.AWIDTH(16),.RAM_SIZE(RAM_SIZE))
     ram_loader (.clk_i(wb_clk),.rst_i(ram_loader_rst),
		 // CPLD Interface
		 .cfg_clk_i(cpld_clk),
		 .cfg_data_i(cpld_din),
		 .start_o(cpld_start),
		 .mode_o(cpld_mode),
		 .done_o(cpld_done),
		 .detached_i(cpld_detached),
		 // Wishbone Interface
		 .wb_dat_o(ram_loader_dat),.wb_adr_o(ram_loader_adr),
		 .wb_stb_o(ram_loader_stb),.wb_cyc_o(),.wb_sel_o(ram_loader_sel),
		 .wb_we_o(ram_loader_we),.wb_ack_i(ram_loader_ack),
		 .ram_loader_done_o(ram_loader_done));

   // Processor
   aeMB_core_BE #(.ISIZ(16),.DSIZ(16),.MUL(0),.BSF(1))
     aeMB (.sys_clk_i(wb_clk), .sys_rst_i(wb_rst),
	   // Instruction Wishbone bus to I-RAM
	   .iwb_stb_o(iwb_stb),.iwb_adr_o(iwb_adr),
	   .iwb_dat_i(iwb_dat),.iwb_ack_i(iwb_ack),
	   // Data Wishbone bus to system bus fabric
	   .dwb_we_o(m0_we),.dwb_stb_o(m0_stb),.dwb_dat_o(m0_dat_i),.dwb_adr_o(m0_adr),
	   .dwb_dat_i(m0_dat_o),.dwb_ack_i(m0_ack),.dwb_sel_o(m0_sel),.dwb_cyc_o(m0_cyc),
	   // Interrupts and exceptions
	   .sys_int_i(proc_int),.sys_exc_i(bus_error) );
   
   assign 	 bus_error = m0_err | m0_rty;
   
   // Dual Ported RAM -- D-Port is Slave #0 on main Wishbone
   // I-port connects directly to processor and ram loader

   ram_harv_cache #(.AWIDTH(15),.RAM_SIZE(RAM_SIZE),.ICWIDTH(7),.DCWIDTH(6))
     sys_ram(.wb_clk_i(wb_clk),.wb_rst_i(wb_rst),
	     
	     .ram_loader_adr_i(ram_loader_adr[14:0]), .ram_loader_dat_i(ram_loader_dat),
	     .ram_loader_stb_i(ram_loader_stb), .ram_loader_sel_i(ram_loader_sel),
	     .ram_loader_we_i(ram_loader_we), .ram_loader_ack_o(ram_loader_ack),
	     .ram_loader_done_i(ram_loader_done),
	     
	     .iwb_adr_i(iwb_adr[14:0]), .iwb_stb_i(iwb_stb),
	     .iwb_dat_o(iwb_dat), .iwb_ack_o(iwb_ack),
	     
	     .dwb_adr_i(s0_adr[14:0]), .dwb_dat_i(s0_dat_o), .dwb_dat_o(s0_dat_i),
	     .dwb_we_i(s0_we), .dwb_ack_o(s0_ack), .dwb_stb_i(s0_stb), .dwb_sel_i(s0_sel));
   
   assign 	 s0_err = 1'b0;
   assign 	 s0_rty = 1'b0;

   // Buffer Pool, slave #1
   wire 	 rd0_read, rd0_sop, rd0_error, rd0_done, rd0_eop;
   wire 	 rd1_read, rd1_sop, rd1_error, rd1_done, rd1_eop;
   wire 	 rd2_read, rd2_sop, rd2_error, rd2_done, rd2_eop;
   wire 	 rd3_read, rd3_sop, rd3_error, rd3_done, rd3_eop;
   wire [31:0] 	 rd0_dat, rd1_dat, rd2_dat, rd3_dat;

   wire 	 wr0_write, wr0_done, wr0_error, wr0_ready, wr0_full;
   wire 	 wr1_write, wr1_done, wr1_error, wr1_ready, wr1_full;
   wire 	 wr2_write, wr2_done, wr2_error, wr2_ready, wr2_full;
   wire 	 wr3_write, wr3_done, wr3_error, wr3_ready, wr3_full;
   wire [31:0] 	 wr0_dat, wr1_dat, wr2_dat, wr3_dat;
   
   buffer_pool buffer_pool
     (.wb_clk_i(wb_clk),.wb_rst_i(wb_rst),
      .wb_we_i(s1_we),.wb_stb_i(s1_stb),.wb_adr_i(s1_adr),.wb_dat_i(s1_dat_o),   
      .wb_dat_o(s1_dat_i),.wb_ack_o(s1_ack),.wb_err_o(s1_err),.wb_rty_o(s1_rty),
   
      .stream_clk(dsp_clk), .stream_rst(dsp_rst),
      .set_stb(set_stb), .set_addr(set_addr), .set_data(set_data),
      .status(status),.sys_int_o(buffer_int),

      .s0(status_b0),.s1(status_b1),.s2(status_b2),.s3(status_b3),
      .s4(status_b4),.s5(status_b5),.s6(status_b6),.s7(status_b7),
      
      // Write Interfaces
      .wr0_dat_i(wr0_dat), .wr0_write_i(wr0_write), .wr0_done_i(wr0_done),
      .wr0_error_i(wr0_error), .wr0_ready_o(wr0_ready), .wr0_full_o(wr0_full),
      .wr1_dat_i(wr1_dat), .wr1_write_i(wr1_write), .wr1_done_i(wr1_done),
      .wr1_error_i(wr1_error), .wr1_ready_o(wr1_ready), .wr1_full_o(wr1_full),
      .wr2_dat_i(wr2_dat), .wr2_write_i(wr2_write), .wr2_done_i(wr2_done),
      .wr2_error_i(wr2_error), .wr2_ready_o(wr2_ready), .wr2_full_o(wr2_full),
      .wr3_dat_i(wr3_dat), .wr3_write_i(wr3_write), .wr3_done_i(wr3_done),
      .wr3_error_i(wr3_error), .wr3_ready_o(wr3_ready), .wr3_full_o(wr3_full),
      // Read Interfaces
      .rd0_dat_o(rd0_dat), .rd0_read_i(rd0_read), .rd0_done_i(rd0_done),
      .rd0_error_i(rd0_error), .rd0_sop_o(rd0_sop), .rd0_eop_o(rd0_eop),
      .rd1_dat_o(rd1_dat), .rd1_read_i(rd1_read), .rd1_done_i(rd1_done),
      .rd1_error_i(rd1_error), .rd1_sop_o(rd1_sop), .rd1_eop_o(rd1_eop),
      .rd2_dat_o(rd2_dat), .rd2_read_i(rd2_read), .rd2_done_i(rd2_done),
      .rd2_error_i(rd2_error), .rd2_sop_o(rd2_sop), .rd2_eop_o(rd2_eop),
      .rd3_dat_o(rd3_dat), .rd3_read_i(rd3_read), .rd3_done_i(rd3_done),
      .rd3_error_i(rd3_error), .rd3_sop_o(rd3_sop), .rd3_eop_o(rd3_eop)
      );

   // SPI -- Slave #2
   spi_top shared_spi
     (.wb_clk_i(wb_clk),.wb_rst_i(wb_rst),.wb_adr_i(s2_adr[4:0]),.wb_dat_i(s2_dat_o),
      .wb_dat_o(s2_dat_i),.wb_sel_i(s2_sel),.wb_we_i(s2_we),.wb_stb_i(s2_stb),
      .wb_cyc_i(s2_cyc),.wb_ack_o(s2_ack),.wb_err_o(s2_err),.wb_int_o(spi_int),
      .ss_pad_o({sen_tx_db,sen_tx_adc,sen_tx_dac,sen_rx_db,sen_rx_adc,sen_rx_dac,sen_dac,sen_clk}),
      .sclk_pad_o(sclk),.mosi_pad_o(mosi),.miso_pad_i(miso) );

   assign 	 s2_rty = 1'b0;
   
   // I2C -- Slave #3
   i2c_master_top #(.ARST_LVL(1)) 
     i2c (.wb_clk_i(wb_clk),.wb_rst_i(wb_rst),.arst_i(1'b0), 
	  .wb_adr_i(s3_adr[4:2]),.wb_dat_i(s3_dat_o[7:0]),.wb_dat_o(s3_dat_i[7:0]),
	  .wb_we_i(s3_we),.wb_stb_i(s3_stb),.wb_cyc_i(s3_cyc),
	  .wb_ack_o(s3_ack),.wb_inta_o(i2c_int),
	  .scl_pad_i(scl_pad_i),.scl_pad_o(scl_pad_o),.scl_padoen_o(scl_pad_oen_o),
	  .sda_pad_i(sda_pad_i),.sda_pad_o(sda_pad_o),.sda_padoen_o(sda_pad_oen_o) );

   assign 	 s3_dat_i[31:8] = 24'd0;
   assign 	 s3_err = 1'b0;
   assign 	 s3_rty = 1'b0;
   
   // GPIOs -- Slave #4
   nsgpio nsgpio(.clk_i(wb_clk),.rst_i(wb_rst),
		 .cyc_i(s4_cyc),.stb_i(s4_stb),.adr_i(s4_adr[3:0]),.we_i(s4_we),
		 .dat_i(s4_dat_o),.dat_o(s4_dat_i),.ack_o(s4_ack),
		 .atr(atr_lines),.debug_0(debug_gpio_0),.debug_1(debug_gpio_1),
		 .gpio( {io_tx,io_rx} ) );
   assign 	 s4_err = 1'b0;
   assign 	 s4_rty = 1'b0;

   // Buffer Pool Status -- Slave #5
   wb_readback_mux buff_pool_status
     (.wb_clk_i(wb_clk),
      .wb_rst_i(wb_rst),
      .wb_stb_i(s5_stb),
      .wb_adr_i(s5_adr),
      .wb_dat_o(s5_dat_i),
      .wb_ack_o(s5_ack),
      
      .word00(status_b0),.word01(status_b1),.word02(status_b2),.word03(status_b3),
      .word04(status_b4),.word05(status_b5),.word06(status_b6),.word07(status_b7),
      .word08(status),.word09({sim_mode,27'b0,clock_divider[3:0]}),.word10({30'b0,clk_func,clk_status}),
      .word11(32'b0),.word12(32'b0),.word13(32'b0),.word14(32'b0),.word15(32'b0)
      );

   assign 	 s5_err = 1'b0;
   assign 	 s5_rty = 1'b0;

   // Slave, #6 Ethernet MAC, see below
   
   // Settings Bus -- Slave #7
   settings_bus settings_bus
     (.wb_clk(wb_clk),.wb_rst(wb_rst),.wb_adr_i(s7_adr),.wb_dat_i(s7_dat_o),
      .wb_stb_i(s7_stb),.wb_we_i(s7_we),.wb_ack_o(s7_ack),
      .sys_clk(dsp_clk),.strobe(set_stb),.addr(set_addr),.data(set_data));
   
   assign 	 s7_err = 1'b0;
   assign 	 s7_rty = 1'b0;
   assign 	 s7_dat_i = 32'd0;

   // Output control lines
   wire [7:0] 	 clock_outs, serdes_outs, adc_outs;
   assign 	 {clock_ready, clk_en[1:0], clk_sel[1:0]} = clock_outs[4:0];
   assign 	 {ser_enable, ser_prbsen, ser_loopen, ser_rx_en} = serdes_outs[3:0];
   assign 	 {adc_oe_a, adc_on_a, adc_oe_b, adc_on_b } = adc_outs[3:0];

   wire 	 phy_reset;
   assign 	 PHY_RESETn = ~phy_reset;
   
   setting_reg #(.my_addr(0)) sr_clk (.clk(wb_clk),.rst(wb_rst),.strobe(s7_ack),.addr(set_addr),
				      .in(set_data),.out(clock_outs),.changed());
   setting_reg #(.my_addr(1)) sr_ser (.clk(wb_clk),.rst(wb_rst),.strobe(set_stb),.addr(set_addr),
				      .in(set_data),.out(serdes_outs),.changed());
   setting_reg #(.my_addr(2)) sr_adc (.clk(wb_clk),.rst(wb_rst),.strobe(set_stb),.addr(set_addr),
				      .in(set_data),.out(adc_outs),.changed());
   setting_reg #(.my_addr(3)) sr_led (.clk(wb_clk),.rst(wb_rst),.strobe(set_stb),.addr(set_addr),
				      .in(set_data),.out(leds),.changed());
   setting_reg #(.my_addr(4)) sr_phy (.clk(wb_clk),.rst(wb_rst),.strobe(set_stb),.addr(set_addr),
				      .in(set_data),.out(phy_reset),.changed());

   // /////////////////////////////////////////////////////////////////////////
   // Ethernet MAC  Slave #6
   
   wire 	 Tx_mac_wa, Tx_mac_wr, Tx_mac_sop, Tx_mac_eop;
   wire 	 Rx_mac_empty, Rx_mac_rd, Rx_mac_sop, Rx_mac_eop, Rx_mac_err;
   wire [31:0] 	 Tx_mac_data, Rx_mac_data;
   wire [1:0] 	 Tx_mac_BE, Rx_mac_BE;
   wire 	 rst_mac;
  
   oneshot_2clk mac_rst_1shot (.clk_in(wb_clk),.in(wb_rst),.clk_out(clk_to_mac),.out(rst_mac));
   
   MAC_top #(.TX_FF_DEPTH(9), .RX_FF_DEPTH(11))
     MAC_top
       (.Clk_125M(clk_to_mac),.Clk_user(dsp_clk),
	.rst_mac(rst_mac),.rst_user(dsp_rst),
	.RST_I(wb_rst),.CLK_I(wb_clk),.STB_I(s6_stb),.CYC_I(s6_cyc),.ADR_I(s6_adr[8:2]),
	.WE_I(s6_we),.DAT_I(s6_dat_o),.DAT_O(s6_dat_i),.ACK_O(s6_ack),
	.Rx_mac_empty(Rx_mac_empty),.Rx_mac_rd(Rx_mac_rd),.Rx_mac_data(Rx_mac_data),.Rx_mac_BE(Rx_mac_BE),
	.Rx_mac_sop(Rx_mac_sop),.Rx_mac_eop(Rx_mac_eop),.Rx_mac_err(Rx_mac_err),
	.Tx_mac_wa(Tx_mac_wa),.Tx_mac_wr(Tx_mac_wr),.Tx_mac_data(Tx_mac_data),
	.Tx_mac_BE(Tx_mac_BE),.Tx_mac_sop(Tx_mac_sop),.Tx_mac_eop(Tx_mac_eop),
	.Gtx_clk(GMII_GTX_CLK),.Tx_clk(GMII_TX_CLK),.Tx_er(GMII_TX_ER),.Tx_en(GMII_TX_EN),.Txd(GMII_TXD),
	.Rx_clk(GMII_RX_CLK),.Rx_er(GMII_RX_ER),.Rx_dv(GMII_RX_DV),.Rxd(GMII_RXD),
	.Crs(GMII_CRS),.Col(GMII_COL),
	.Mdio(MDIO),.Mdc(MDC),
	.rx_fifo_occupied(eth_rx_occ2),.rx_fifo_full(eth_rx_full2),.rx_fifo_empty(eth_rx_empty2),
	.tx_fifo_occupied(),.tx_fifo_full(),.tx_fifo_empty(),
	.debug0(debug_mac0),.debug1(debug_mac1) );

   assign 	 s6_err = 1'b0;
   assign 	 s6_rty = 1'b0;

   mac_rxfifo_int mac_rxfifo_int
     (.clk(dsp_clk),.rst(dsp_rst),
      .Rx_mac_empty(Rx_mac_empty),.Rx_mac_rd(Rx_mac_rd),.Rx_mac_data(Rx_mac_data),
      .Rx_mac_BE(Rx_mac_BE),.Rx_mac_sop(Rx_mac_sop),
      .Rx_mac_eop(Rx_mac_eop),.Rx_mac_err(Rx_mac_err),
      .wr_dat_o(wr2_dat),.wr_write_o(wr2_write),.wr_done_o(wr2_done),
      .wr_error_o(wr2_error),.wr_ready_i(wr2_ready),.wr_full_i(wr2_full),
      .fifo_occupied(eth_rx_occ),.fifo_full(eth_rx_full),.fifo_empty(eth_rx_empty) );

   mac_txfifo_int mac_txfifo_int
     (.clk(dsp_clk),.rst(dsp_rst),.mac_clk(clk_to_mac),
      .Tx_mac_wa(Tx_mac_wa),.Tx_mac_wr(Tx_mac_wr),.Tx_mac_data(Tx_mac_data),
      .Tx_mac_BE(Tx_mac_BE),.Tx_mac_sop(Tx_mac_sop),.Tx_mac_eop(Tx_mac_eop),
      .rd_dat_i(rd2_dat),.rd_read_o(rd2_read),.rd_done_o(rd2_done),
      .rd_error_o(rd2_error),.rd_sop_i(rd2_sop),.rd_eop_i(rd2_eop),
      .fifo_occupied(eth_tx_occ),.fifo_full(eth_tx_full),.fifo_empty(eth_tx_empty) );
   
   // /////////////////////////////////////////////////////////////////////////
   // Interrupt Controller, Slave #8

   wire [8:0] 	 irq={{6'b0,uart_tx_int, uart_rx_int},
		      {pps_int,overrun,underrun,PHY_INTn,i2c_int,spi_int,timer_int,buffer_int}};
   
   simple_pic #(.is(9),.dwidth(32)) simple_pic
     (.clk_i(wb_clk),.rst_i(wb_rst),.cyc_i(s8_cyc),.stb_i(s8_stb),.adr_i(s8_adr[3:2]),
      .we_i(s8_we),.dat_i(s8_dat_o),.dat_o(s8_dat_i),.ack_o(s8_ack),.int_o(proc_int),
      .irq(irq) );
   assign 	 s8_err = 0;
   assign 	 s8_rty = 0;
 	 
   // /////////////////////////////////////////////////////////////////////////
   // Master Timer, Slave #9

   wire [31:0] 	 master_time;
   timer timer
     (.wb_clk_i(wb_clk),.rst_i(wb_rst),
      .cyc_i(s9_cyc),.stb_i(s9_stb),.adr_i(s9_adr[4:2]),
      .we_i(s9_we),.dat_i(s9_dat_o),.dat_o(s9_dat_i),.ack_o(s9_ack),
      .sys_clk_i(dsp_clk),.master_time_i(master_time),.int_o(timer_int) );
   assign 	 s9_err = 0;
   assign 	 s9_rty = 0;

   // /////////////////////////////////////////////////////////////////////////
   // UART, Slave #10

   simple_uart #(.TXDEPTH(3),.RXDEPTH(3)) uart  // depth of 3 is 128 entries
     (.clk_i(wb_clk),.rst_i(wb_rst),
      .we_i(s10_we),.stb_i(s10_stb),.cyc_i(s10_cyc),.ack_o(s10_ack),
      .adr_i(s10_adr[4:2]),.dat_i(s10_dat_o),.dat_o(s10_dat_i),
      .rx_int_o(uart_rx_int),.tx_int_o(uart_tx_int),
      .tx_o(uart_tx_o),.rx_i(uart_rx_i),.baud_o(uart_baud_o));
   
   assign 	 s10_err = 0;
   assign 	 s10_rty = 0;
   
   // /////////////////////////////////////////////////////////////////////////
   // ATR Controller, Slave #11

   wire 	 run_rx, run_tx;
   reg 		 run_rx_d1;
   always @(posedge dsp_clk)
     run_rx_d1 <= run_rx;
   
   atr_controller atr_controller
     (.clk_i(wb_clk),.rst_i(wb_rst),
      .adr_i(s11_adr[5:0]),.sel_i(s11_sel),.dat_i(s11_dat_o),.dat_o(s11_dat_i),
      .we_i(s11_we),.stb_i(s11_stb),.cyc_i(s11_cyc),.ack_o(s11_ack),
      .run_rx(run_rx_d1),.run_tx(run_tx),.ctrl_lines(atr_lines) );
   assign 	 s11_err = 0;
   assign 	 s11_rty = 0;
   
   // //////////////////////////////////////////////////////////////////////////
   // Time Sync, Slave #12 

   time_sync time_sync
     (.wb_clk_i(wb_clk),.rst_i(wb_rst),
      .cyc_i(s12_cyc),.stb_i(s12_stb),.adr_i(s12_adr[4:2]),
      .we_i(s12_we),.dat_i(s12_dat_o),.dat_o(s12_dat_i),.ack_o(s12_ack),
      .sys_clk_i(dsp_clk),.master_time_o(master_time),
      .pps_in(pps_in),.exp_pps_in(exp_pps_in),.exp_pps_out(exp_pps_out),
      .int_o(pps_int) );
   assign 	 s12_err = 0;
   assign 	 s12_rty = 0;

   // /////////////////////////////////////////////////////////////////////////
   // SD Card Reader / Writer, Slave #13

   sd_spi_wb sd_spi_wb
     (.clk(wb_clk),.rst(wb_rst),
      .sd_clk(sd_clk),.sd_csn(sd_csn),.sd_mosi(sd_mosi),.sd_miso(sd_miso),
      .wb_cyc_i(s13_cyc),.wb_stb_i(s13_stb),.wb_we_i(s13_we),
      .wb_adr_i(s13_adr[3:2]),.wb_dat_i(s13_dat_o),.wb_dat_o(s13_dat_i),
      .wb_ack_o(s13_ack) );
   assign 	 s13_err = 0;
   assign 	 s13_rty = 0;
   // /////////////////////////////////////////////////////////////////////////
   // DSP
   wire [31:0] 	 sample_rx, sample_tx;
   wire 	 strobe_rx, strobe_tx;

   rx_control #(.FIFOSIZE(10)) rx_control
     (.clk(dsp_clk), .rst(dsp_rst),
      .set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .master_time(master_time),.overrun(overrun),
      .wr_dat_o(wr1_dat), .wr_write_o(wr1_write), .wr_done_o(wr1_done), .wr_error_o(wr1_error),
      .wr_ready_i(wr1_ready), .wr_full_i(wr1_full),
      .sample(sample_rx), .run(run_rx), .strobe(strobe_rx),
      .fifo_occupied(dsp_rx_occ),.fifo_full(dsp_rx_full),.fifo_empty(dsp_rx_empty),
      .debug_rx(debug_rx) );
   
   // dummy_rx dsp_core_rx
   dsp_core_rx dsp_core_rx
     (.clk(dsp_clk),.rst(dsp_rst),
      .set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .adc_a(adc_a),.adc_ovf_a(adc_ovf_a),.adc_b(adc_b),.adc_ovf_b(adc_ovf_b),
      .sample(sample_rx), .run(run_rx_d1), .strobe(strobe_rx),
      .debug(debug_rx_dsp) );

   tx_control #(.FIFOSIZE(10)) tx_control
     (.clk(dsp_clk), .rst(dsp_rst),
      .set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .master_time(master_time),.underrun(underrun),
      .rd_dat_i(rd1_dat), .rd_sop_i(rd1_sop), .rd_eop_i(rd1_eop),
      .rd_read_o(rd1_read), .rd_done_o(rd1_done), .rd_error_o(rd1_error),
      .sample(sample_tx), .run(run_tx), .strobe(strobe_tx),
      .fifo_occupied(dsp_tx_occ),.fifo_full(dsp_tx_full),.fifo_empty(dsp_tx_empty),
      .debug(debug_txc) );
   
   dsp_core_tx dsp_core_tx
     (.clk(dsp_clk),.rst(dsp_rst),
      .set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .dac_a(dac_a),.dac_b(dac_b),
      .sample(sample_tx), .run(run_tx), .strobe(strobe_tx), .debug(debug_tx_dsp) );

   assign dsp_rst = wb_rst;

   // ///////////////////////////////////////////////////////////////////////////////////
   // SERDES

   serdes #(.TXFIFOSIZE(9),.RXFIFOSIZE(9)) serdes
     (.clk(dsp_clk),.rst(dsp_rst),
      .ser_tx_clk(ser_tx_clk),.ser_t(ser_t),.ser_tklsb(ser_tklsb),.ser_tkmsb(ser_tkmsb),
      .rd_dat_i(rd0_dat),.rd_read_o(rd0_read),.rd_done_o(rd0_done),.rd_error_o(rd0_error),
      .rd_sop_i(rd0_sop),.rd_eop_i(rd0_eop),
      .ser_rx_clk(ser_rx_clk),.ser_r(ser_r),.ser_rklsb(ser_rklsb),.ser_rkmsb(ser_rkmsb),
      .wr_dat_o(wr0_dat),.wr_write_o(wr0_write),.wr_done_o(wr0_done),.wr_error_o(wr0_error),
      .wr_ready_i(wr0_ready),.wr_full_i(wr0_full),
      .tx_occupied(ser_tx_occ),.tx_full(ser_tx_full),.tx_empty(ser_tx_empty),
      .rx_occupied(ser_rx_occ),.rx_full(ser_rx_full),.rx_empty(ser_rx_empty),
      .debug0(debug_serdes0), .debug1(debug_serdes1) );

   // ///////////////////////////////////////////////////////////////////////////////////
   // External RAM Interface

   extram_interface extram_interface
     (.clk(dsp_clk),.rst(dsp_rst),
      .set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .rd_dat_i(rd3_dat),.rd_read_o(rd3_read),.rd_done_o(rd3_done),.rd_error_o(rd3_error),
      .rd_sop_i(rd3_sop),.rd_eop_i(rd3_eop),
      .wr_dat_o(wr3_dat),.wr_write_o(wr3_write),.wr_done_o(wr3_done),.wr_error_o(wr3_error),
      .wr_ready_i(wr3_ready),.wr_full_i(wr3_full),
      .RAM_D(RAM_D),.RAM_A(RAM_A),.RAM_CE1n(RAM_CE1n),.RAM_CENn(RAM_CENn),
      .RAM_CLK(RAM_CLK),.RAM_WEn(RAM_WEn),.RAM_OEn(RAM_OEn),.RAM_LDn(RAM_LDn) );

  
   // /////////////////////////////////////////////////////////////////////////////////////////
   // Debug Pins

   // FIFO Level Debugging
   reg [31:0] host_to_dsp_fifo, dsp_to_host_fifo, eth_mac_debug, serdes_to_dsp_fifo, dsp_to_serdes_fifo;

   always @(posedge dsp_clk)
     serdes_to_dsp_fifo <= { {ser_rx_full,ser_rx_empty,ser_rx_occ[13:0]},
			   {dsp_tx_full,dsp_tx_empty,dsp_tx_occ[13:0]} };

   always @(posedge dsp_clk)
     dsp_to_serdes_fifo <= { {ser_tx_full,ser_tx_empty,ser_tx_occ[13:0]},
			  {dsp_rx_full,dsp_rx_empty,dsp_rx_occ[13:0]} };

   always @(posedge dsp_clk)
     host_to_dsp_fifo <= { {eth_rx_full,eth_rx_empty,eth_rx_occ[13:0]},
			   {dsp_tx_full,dsp_tx_empty,dsp_tx_occ[13:0]} };

   always @(posedge dsp_clk)
     dsp_to_host_fifo <= { {eth_tx_full,eth_tx_empty,eth_tx_occ[13:0]},
			  {dsp_rx_full,dsp_rx_empty,dsp_rx_occ[13:0]} };

   always @(posedge dsp_clk)
     eth_mac_debug <= {// {eth_tx_full2, eth_tx_empty2, eth_tx_occ2[13:0]},
		      // {underrun, overrun, debug_mac0[13:0] },
		       {debug_txc[15:0]},
		       {eth_rx_full2, eth_rx_empty2, eth_rx_occ2[13:0]} };
   
   wire       debug_mux;
   setting_reg #(.my_addr(5)) sr_debug (.clk(wb_clk),.rst(wb_rst),.strobe(set_stb),.addr(set_addr),
					.in(set_data),.out(debug_mux),.changed());

   //assign     debug = debug_mux ? host_to_dsp_fifo : dsp_to_host_fifo;
   //assign     debug = debug_mux ? serdes_to_dsp_fifo : dsp_to_serdes_fifo;
   
   // Assign various commonly used debug buses.
   /*
   wire [31:0] debug_rx_1 = {uart_tx_o,GMII_TX_EN,strobe_rx,overrun,proc_int,buffer_int,timer_int,GMII_RX_DV,
			     irq[7:0],
			     GMII_RXD,
			     GMII_TXD};
   
   wire [31:0] debug_rx_2 = { 5'd0, s8_we, s8_stb, s8_ack, debug_rx[23:0] };
   
   wire [31:0] debug_time =  {uart_tx_o, 7'b0,
			      irq[7:0],
			      6'b0, GMII_RX_DV, GMII_TX_EN,
			      4'b0, exp_pps_in, exp_pps_out, pps_in, pps_int};

   wire [31:0] debug_irq =  {uart_tx_o, iwb_adr, iwb_ack,
			     irq[7:0],
			     proc_int,  7'b0 };

   wire [31:0] debug_eth = 
	       {{uart_tx_o,proc_int,underrun,buffer_int,wr2_ready,wr2_error,wr2_done,wr2_write},
		{8'd0},
		{8'd0},
		{GMII_TX_EN,GMII_RX_DV,Rx_mac_empty,Rx_mac_rd,Rx_mac_err,Rx_mac_sop,Rx_mac_eop,wr2_full} };

   assign      debug_serdes0 = { { rd0_dat[7:0] },
				 { ser_tx_clk, ser_tkmsb, ser_tklsb, rd0_sop, rd0_eop, rd0_read, rd0_error, rd0_done },
				 { ser_t[15:8] },
				 { ser_t[7:0] } };

   assign      debug_serdes1 = { {1'b0,proc_int,underrun,buffer_int,wr0_ready,wr0_error,wr0_done,wr0_write},
				 { 1'b0, ser_rx_clk, ser_rkmsb, ser_rklsb, ser_enable, ser_prbsen, ser_loopen, ser_rx_en },
				 { ser_r[15:8] },
				 { ser_r[7:0] } };
       
   assign      debug_gpio_1 = {uart_tx_o,7'd0,
			       3'd0,rd1_sop,rd1_eop,rd1_read,rd1_done,rd1_error,
			       debug_txc[15:0]};
   assign      debug_gpio_1 = debug_rx;
   assign      debug_gpio_1 = debug_serdes1;
   assign      debug_gpio_1 = debug_eth;
      
    */
      
   // Choose actual debug buses
   assign      debug_clk[0] = wb_clk;
   assign      debug_clk[1] = dsp_clk;	
   
   //assign      debug = {{strobe_rx,/*adc_ovf_a*/ 1'b0,adc_a},
   	//		{run_rx,/*adc_ovf_b*/ 1'b0,adc_b}};

   //assign      debug = debug_tx_dsp;
   //assign      debug = debug_serdes0;
   
   assign      debug_gpio_0 = 0; //debug_serdes0;
   assign      debug_gpio_1 = 0; //debug_serdes1;

   assign      debug={{3'b0, wb_clk, wb_rst, dsp_rst, por, config_success},
		      {8'b0},
		      {3'b0,ram_loader_ack, ram_loader_stb, ram_loader_we,ram_loader_rst,ram_loader_done },
		      {cpld_start,cpld_mode,cpld_done,cpld_din,cpld_clk,cpld_detached,cpld_misc,cpld_init_b} };

   //assign      debug = {dac_a,dac_b};
endmodule // u2_core
