//////////////////////////////////////////////////////////////////////////////////
// Module Name:    u2_basic
//////////////////////////////////////////////////////////////////////////////////

module eth_sim_top
  (// Clocks
   input dsp_clk,
   input wb_clk,
   output clock_ready,
   input clk_to_mac,
   input pps_in,
   
   // Misc, debug
   output led1,
   output led2,
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
   input PHY_RESETn,
   input PHY_CLK,   // possibly use on-board osc

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
   
   // ADC
   input [13:0] adc_a,
   input adc_ovf_a,
   output adc_oen_a,
   output adc_pdn_a,
   
   input [13:0] adc_b,
   input adc_ovf_b,
   output adc_oen_b,
   output adc_pdn_b,
   
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
   inout [15:0] io_rx
   );
   
   wire [7:0] 	set_addr;
   wire [31:0] 	set_data;
   wire 	set_stb;
   
   wire 	ram_loader_done;
   wire 	ram_loader_rst, wb_rst, dsp_rst;

   wire [31:0] 	ser_debug;
   
   //////////////////////////////////////////////////////////////////////////////////////////////////
   // Wishbone Single Master INTERCON
   parameter 	dw = 32;  // Data bus width
   parameter 	aw = 16;  // Address bus width, for byte addressibility, 16 = 64K byte memory space
   parameter 	sw = 4;   // Select width -- 32-bit data bus with 8-bit granularity.  
      
   wire [dw-1:0] m0_dat_o, m1_dat_o, m0_dat_i, m1_dat_i;
   wire [dw-1:0] s0_dat_o, s1_dat_o, s0_dat_i, s1_dat_i, s2_dat_o, s3_dat_o, s2_dat_i, s3_dat_i,
		 s4_dat_o, s5_dat_o, s4_dat_i, s5_dat_i, s6_dat_o, s7_dat_o, s6_dat_i, s7_dat_i;
   wire [aw-1:0] m0_adr, m1_adr, s0_adr,s1_adr,s2_adr,s3_adr,s4_adr,s5_adr,s6_adr,s7_adr;
   wire [sw-1:0] m0_sel, m1_sel, s0_sel, s1_sel, s2_sel, s3_sel, s4_sel, s5_sel, s6_sel, s7_sel;
   wire 	 m0_ack, m1_ack, s0_ack, s1_ack, s2_ack, s3_ack, s4_ack, s5_ack, s6_ack, s7_ack;
   wire 	 m0_stb, m1_stb, s0_stb, s1_stb, s2_stb, s3_stb, s4_stb, s5_stb, s6_stb, s7_stb;
   wire 	 m0_cyc, m1_cyc, s0_cyc, s1_cyc, s2_cyc, s3_cyc, s4_cyc, s5_cyc, s6_cyc, s7_cyc;
   wire 	 m0_err, m1_err, s0_err, s1_err, s2_err, s3_err, s4_err, s5_err, s6_err, s7_err;
   wire 	 m0_rty, m1_rty, s0_rty, s1_rty, s2_rty, s3_rty, s4_rty, s5_rty, s6_rty, s7_rty;
   wire 	 m0_we, m1_we, s0_we, s1_we, s2_we, s3_we, s4_we, s5_we, s6_we, s7_we;
   
   wb_1master #(.s0_addr_w(2),.s0_addr(2'b00),.s1_addr_w(2),.s1_addr(2'b01),
		.s27_addr_w(4),.s2_addr(4'b1000),.s3_addr(4'b1001),.s4_addr(4'b1010),
		.s5_addr(4'b1011),.s6_addr(4'b1100),.s7_addr(4'b1101),
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
      .s7_dat_i(s7_dat_i),.s7_ack_i(s7_ack),.s7_err_i(s7_err),.s7_rty_i(s7_rty)
      );
   
   //////////////////////////////////////////////////////////////////////////////////////////
   // Reset Controller
   system_control sysctrl (.wb_clk_i(wb_clk),
			   .ram_loader_rst_o(ram_loader_rst),
			   .wb_rst_o(wb_rst),
			   .ram_loader_done_i(ram_loader_done));
   
   // ///////////////////////////////////////////////////////////////////
   // RAM Loader
   wire 	 iram_wr_stb, iram_rd_stb, iram_wr_ack, iram_rd_ack, iram_ack, iram_wr_we;
   wire [3:0] 	 iram_wr_sel;
   wire [aw-1:0] iram_wr_adr, iram_rd_adr;
   wire [dw-1:0] iram_wr_dat, iram_rd_dat;

   wire 	 bus_error, proc_int;

   assign 	 iram_rd_ack = ram_loader_done ? iram_ack : 1'b0;
   assign 	 iram_wr_ack = ram_loader_done ? 1'b0 : iram_ack;
   
   ram_loader #(.AWIDTH(16))
     ram_loader (.clk_i(wb_clk),.rst_i(ram_loader_rst),
		 // CPLD Interface
		 .cfg_clk_i(cpld_clk),
		 .cfg_data_i(cpld_din),
		 .start_o(cpld_start),
		 .mode_o(cpld_mode),
		 .done_o(cpld_done),
		 .detached_i(cpld_detached),
		 // Wishbone Interface
		 .wb_dat_o(iram_wr_dat),.wb_adr_o(iram_wr_adr),
		 .wb_stb_o(iram_wr_stb),.wb_cyc_o(),.wb_sel_o(iram_wr_sel),
		 .wb_we_o(iram_wr_we),.wb_ack_i(iram_wr_ack),
		 .ram_loader_done_o(ram_loader_done));

   // Processor
   aeMB_core_BE #(.ISIZ(16),.DSIZ(16))
     aeMB (.sys_clk_i(wb_clk), .sys_rst_i(wb_rst),
	   // Instruction Wishbone bus to I-RAM
	   .iwb_stb_o(iram_rd_stb),.iwb_adr_o(iram_rd_adr),
	   .iwb_dat_i(iram_rd_dat),.iwb_ack_i(iram_rd_ack),
	   // Data Wishbone bus to system bus fabric
	   .dwb_we_o(m0_we),.dwb_stb_o(m0_stb),.dwb_dat_o(m0_dat_i),.dwb_adr_o(m0_adr),
	   .dwb_dat_i(m0_dat_o),.dwb_ack_i(m0_ack),.dwb_sel_o(m0_sel),.dwb_cyc_o(m0_cyc),
	   // Interrupts and exceptions
	   .sys_int_i(proc_int),.sys_exc_i(bus_error) );

   assign 	 bus_error = m0_err | m0_rty;
   assign	 proc_int = 1'b0;
   
   // Dual Ported RAM -- D-Port is Slave #0 on main Wishbone
   // I-port connects directly to processor and ram loader
   
   ram_wb_harvard #(.AWIDTH(14))
     ID_ram (.wb_clk_i(wb_clk),.wb_rst_i(wb_rst),
	     
	     .iwb_adr_i(ram_loader_done ? iram_rd_adr : iram_wr_adr),.iwb_dat_i(iram_wr_dat),.iwb_dat_o(iram_rd_dat),
	     .iwb_we_i(iram_wr_we),.iwb_ack_o(iram_ack),.iwb_stb_i(ram_loader_done ? iram_rd_stb : iram_wr_stb),
	     .iwb_sel_i(ram_loader_done ? 4'b1111 : iram_wr_sel),
	     
	     .dwb_adr_i(s0_adr),.dwb_dat_i(s0_dat_o),.dwb_dat_o(s0_dat_i),
	     .dwb_we_i(s0_we),.dwb_ack_o(s0_ack),.dwb_stb_i(s0_stb),.dwb_sel_i(s0_sel));

   assign 	 s0_err = 1'b0;
   assign 	 s0_rty = 1'b0;

   // Buffer Pool, slave #1
   wire 	 rd0_read, rd0_ready, rd0_done, rd0_empty;
   wire 	 rd1_read, rd1_ready, rd1_done, rd1_empty;
   wire 	 rd2_read, rd2_ready, rd2_done, rd2_empty;
   wire 	 rd3_read, rd3_ready, rd3_done, rd3_empty;
   wire [31:0] 	 rd0_dat, rd1_dat, rd2_dat, rd3_dat;

   wire 	 wr0_write, wr0_done, wr0_ready, wr0_full;
   wire 	 wr1_write, wr1_done, wr1_ready, wr1_full;
   wire 	 wr2_write, wr2_done, wr2_ready, wr2_full;
   wire 	 wr3_write, wr3_done, wr3_ready, wr3_full;
   wire [31:0] 	 wr0_dat, wr1_dat, wr2_dat, wr3_dat;

/*   
   buffer_pool buffer_pool
     (.wb_clk_i(wb_clk),.wb_rst_i(wb_rst),
      .wb_we_i(s1_we),.wb_stb_i(s1_stb),.wb_adr_i(s1_adr),.wb_dat_i(s1_dat_o),   
      .wb_dat_o(s1_dat_i),.wb_ack_o(s1_ack),.wb_err_o(s1_err),.wb_rty_o(s1_rty),
   
      .stream_clk(dsp_clk),.stream_rst(dsp_rst),
      // Write Interfaces
      .wr0_dat_i(),.wr0_write_i(),.wr0_done_i(),.wr0_ready_o(),.wr0_full_o(),
      .wr1_dat_i(),.wr1_write_i(),.wr1_done_i(),.wr1_ready_o(),.wr1_full_o(),
      .wr2_dat_i(),.wr2_write_i(),.wr2_done_i(),.wr2_ready_o(),.wr2_full_o(),
      .wr3_dat_i(),.wr3_write_i(),.wr3_done_i(),.wr3_ready_o(),.wr3_full_o(),
      // Read Interfaces
      .rd0_dat_o(rd0_dat),.rd0_read_i(rd0_read),.rd0_done_i(),.rd0_ready_o(rd0_ready),.rd0_empty_o(rd0_empty),
      .rd1_dat_o(rd1_dat),.rd1_read_i(rd1_read),.rd1_done_i(),.rd1_ready_o(rd1_ready),.rd1_empty_o(rd1_empty),
      .rd2_dat_o(rd2_dat),.rd2_read_i(rd2_read),.rd2_done_i(),.rd2_ready_o(rd2_ready),.rd2_empty_o(rd2_empty),
      .rd3_dat_o(rd3_dat),.rd3_read_i(rd3_read),.rd3_done_i(),.rd3_ready_o(rd3_ready),.rd3_empty_o(rd3_empty)
      );
*/
   // SPI -- Slave #2
   spi_top shared_spi
     (.wb_clk_i(wb_clk),.wb_rst_i(wb_rst),.wb_adr_i(s2_adr),.wb_dat_i(s2_dat_o),.wb_dat_o(s2_dat_i),
      .wb_sel_i(s2_sel),.wb_we_i(s2_we),.wb_stb_i(s2_stb),.wb_cyc_i(s2_cyc),.wb_ack_o(s2_ack),

      .wb_err_o(s2_err),.wb_int_o(s2_int),
      .ss_pad_o({sen_tx_db,sen_tx_adc,sen_tx_dac,sen_rx_db,sen_rx_adc,sen_rx_dac,sen_dac,sen_clk}),
      .sclk_pad_o(sclk),.mosi_pad_o(mosi),.miso_pad_i(miso) );

   assign 	 s2_rty = 1'b0;
   
   // I2C -- Slave #3
   i2c_master_top #(.ARST_LVL(1)) 
     i2c (.wb_clk_i(wb_clk),.wb_rst_i(wb_rst),.arst_i(1'b0), 
	  .wb_adr_i(s3_adr),.wb_dat_i(s3_dat_o),.wb_dat_o(s3_dat_i),
	  .wb_we_i(s3_we),.wb_stb_i(s3_stb),.wb_cyc_i(s3_cyc),
	  .wb_ack_o(s3_ack),.wb_inta_o(st_int),
	  .scl_pad_i(scl_pad_i),.scl_pad_o(scl_pad_o),.scl_padoen_o(scl_pad_oen_o),
	  .sda_pad_i(sda_pad_i),.sda_pad_o(sda_pad_o),.sda_padoen_o(sda_pad_oen_o) );
   
   assign 	 s3_err = 1'b0;
   assign 	 s3_rty = 1'b0;

   // GPIOs -- Slave #4
   wire 	 s4_ack_a, s4_ack_b, s4_ack_c, s4_ack_d;
   assign 	 s4_ack = s4_ack_a | s4_ack_b | s4_ack_c | s4_ack_d;

   simple_gpio gpio_a(.clk_i(wb_clk),.rst_i(~wb_rst),
		      .cyc_i(s4_cyc),.stb_i(s4_stb&s4_sel[0]),.adr_i(s4_adr[2]),.we_i(s4_we),
		      .dat_i(s4_dat_o[7:0]),.dat_o(s4_dat_i[7:0]),.ack_o(s4_ack_a),
		      .gpio(/* io_tx[7:0]*/) );
   
   simple_gpio gpio_b(.clk_i(wb_clk),.rst_i(~wb_rst),
		      .cyc_i(s4_cyc),.stb_i(s4_stb&s4_sel[1]),.adr_i(s4_adr[2]),.we_i(s4_we),
		      .dat_i(s4_dat_o[15:8]),.dat_o(s4_dat_i[15:8]),.ack_o(s4_ack_b),
		      .gpio(/* io_tx[15:8] */) );
   
   simple_gpio gpio_c(.clk_i(wb_clk),.rst_i(~wb_rst),
		      .cyc_i(s4_cyc),.stb_i(s4_stb&s4_sel[2]),.adr_i(s4_adr[2]),.we_i(s4_we),
		      .dat_i(s4_dat_o[23:16]),.dat_o(s4_dat_i[23:16]),.ack_o(s4_ack_c),
		      .gpio(/* io_rx[7:0] */) );
   
   simple_gpio gpio_d(.clk_i(wb_clk),.rst_i(~wb_rst),
		      .cyc_i(s4_cyc),.stb_i(s4_stb&s4_sel[3]),.adr_i(s4_adr[2]),.we_i(s4_we),
		      .dat_i(s4_dat_o[31:24]),.dat_o(s4_dat_i[31:24]),.ack_o(s4_ack_d),
		      .gpio(/* io_rx[15:8]*/) );

   assign 	 s4_err = 1'b0;
   assign 	 s4_rty = 1'b0;
   
   // Output control lines, SLAVE #5
   wire [7:0] 	 clock_outs, serdes_outs, adc_outs, misc_outs;
   assign 	 {clock_ready, clk_en[1:0], clk_sel[1:0]} = clock_outs[4:0];
   assign 	 {ser_enable, ser_prbsen, ser_loopen, ser_rx_en} = serdes_outs[3:0];
   assign 	 { adc_oen_a, adc_pdn_a, adc_oen_b, adc_pdn_b } = adc_outs[3:0];
   assign 	 {led2, led1} = misc_outs[1:0];
   
   wb_output_pins32 control_lines
     (.wb_rst_i(wb_rst),.wb_clk_i(wb_clk),.wb_dat_i(s5_dat_o),.wb_dat_o(s5_dat_i),
      .wb_we_i(s5_we),.wb_sel_i(s5_sel),.wb_stb_i(s5_stb),.wb_ack_o(s5_ack),.wb_cyc_i(s5_cyc),
      .port_output( {clock_outs,serdes_outs,adc_outs,misc_outs} )  );

   assign 	 s5_err = 1'b0;
   assign 	 s5_rty = 1'b0;

   // Ethernet slave, #6
   eth_wrapper eth_wrapper
     (.Reset(wb_rst),.Clk_125M(),.Clk_user(stream_clk),.Clk_reg(wb_clk),.Speed(),
      .Gtx_clk(GMII_GTX_CLK),.Rx_clk(GMII_RX_CLK),.Tx_clk(GMII_TX_CLK),//used only in MII mode
      .Tx_er(GMII_TX_ER),.Tx_en(GMII_TX_EN),.Txd(GMII_TXD),.Rx_er(GMII_RX_ER),
      .Rx_dv(GMII_RX_DV),.Rxd(GMII_RXD),.Crs(GMII_CRS),.Col(GMII_COL),
      .Mdio(MDIO),.Mdc(MDC),
      // FIFO Interfaces
      .wr_dat_o(),.wr_write_o(),.wr_done_o(),.wr_ready_i(),.wr_full_i(),
      .rd_dat_i(),.rd_read_o(),.rd_done_o(),.rd_ready_i(),.rd_empty_i(),
      // Wishbone
      .wb_dat_i(s6_dat_o),.wb_dat_o(s6_dat_i),.wb_adr_i(s6_adr),.wb_stb_i(s6_stb),.wb_we_i(s6_we),.wb_ack_o(s6_ack)
      );
   
   assign 	 s6_err = 1'b0;
   assign 	 s6_rty = 1'b0;
   
   // Settings Bus -- Slave #7
   settings_bus settings_bus
     (.wb_clk(wb_clk),.wb_rst(wb_rst),.wb_adr_i(s7_adr),.wb_dat_i(s7_dat_o),
      .wb_stb_i(s7_stb),.wb_we_i(s7_we),.wb_ack_o(s7_ack),
      .sys_clk(dsp_clk),.strobe(set_stb),.addr(set_addr),.data(set_data));
   
   assign 	 s7_err = 1'b0;
   assign 	 s7_rty = 1'b0;
   assign 	 s7_dat_i = 32'd0;

   ///////////////////////////////////////////////////////////////////////////
   // DSP
   reg [13:0] 	 adc_a_reg1, adc_b_reg1, adc_a_reg2, adc_b_reg2;
   reg 		 adc_ovf_a_reg1, adc_ovf_a_reg2, adc_ovf_b_reg1, adc_ovf_b_reg2;
   
   always @(posedge dsp_clk)
     begin
	adc_a_reg1 <= adc_a;
	adc_a_reg2 <= adc_a_reg1;
	adc_b_reg1 <= adc_b;
	adc_b_reg2 <= adc_b_reg1;
	adc_ovf_a_reg1 <= adc_ovf_a;
	adc_ovf_a_reg2 <= adc_ovf_a_reg1;
	adc_ovf_b_reg1 <= adc_ovf_b;
	adc_ovf_b_reg2 <= adc_ovf_b_reg1;
     end // always @ (posedge dsp_clk)
   
   dsp_core_rx dsp_core_rx
     (.clk(dsp_clk),.rst(dsp_rst),
      .set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .adc_a(adc_a),.adc_ovf_a(adc_ovf_a),.adc_b(adc_b),.adc_ovf_b(adc_ovf_b),
      .rx_dat_o(wr1_dat),.rx_write_o(wr1_write),.rx_done_o(wr1_done),
      .rx_ready_i(wr1_ready),.rx_full_i(wr1_full),
      .overrun() );
   
   dsp_core_tx dsp_core_tx
     (.clk(dsp_clk),.rst(dsp_rst),
      .set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .dac_a(dac_a),.dac_b(dac_b),
      .tx_dat_i(rd1_dat),.tx_read_o(rd1_read),.tx_done_o(rd1_done),
      .tx_ready_i(rd1_ready),.tx_empty_i(rd1_empty),
      .underrun() );
   
   assign dsp_rst = wb_rst;

   /////////////////////////////////////////////////////////////////////////////////////
   // SERDES
   serdes_tx serdes_tx
     (.clk(dsp_clk),.rst(dsp_rst),
      .ser_tx_clk(ser_tx_clk),.ser_t(ser_t),.ser_tklsb(ser_tklsb),.ser_tkmsb(ser_tkmsb),
      .fifo_data_i(rd0_dat),.fifo_read_o(rd0_read),.fifo_done_o(rd0_done),
      .fifo_ready_i(rd0_ready),.fifo_empty_i(rd0_empty)
      );

   serdes_rx serdes_rx
     (.clk(dsp_clk),.rst(dsp_rst),
      .ser_rx_clk(ser_rx_clk),.ser_r(ser_r),.ser_rklsb(ser_rklsb),.ser_rkmsb(ser_rkmsb),
      .fifo_data_o(wr0_dat),.fifo_wr_o(wr0_write),.fifo_ready_i(wr0_ready),.fifo_done_i(wr0_done)
      );
   
   // Debug Pins
   wire [31:0]	debug1={{1'b0,ram_loader_done,clock_ready,dsp_clk,wb_clk,ram_loader_rst,wb_rst,dsp_rst},
   			{1'b0,cpld_start,cpld_mode,cpld_done,1'b0,cpld_din,cpld_clk,cpld_detached},
   			{8'hAF},
   			{2'b0, clk_status, sen_dac, sen_clk, sclk, mosi, miso}};

   wire [31:0]	debug_wb={{iram_wr_we,ram_loader_done,clock_ready,iram_wr_ack,iram_wr_stb,ram_loader_rst,wb_rst,dsp_rst},
   			  {iram_rd_adr[15:8]},
   			  {iram_rd_adr[7:0]},
   			  {serdes_outs}};

   assign 	io_rx = ser_debug[31:16];
   assign 	io_tx = ser_debug[15:0];
 
   assign 	debug = debug_wb;
   
   assign 	debug_clk[0] = wb_clk;
   assign 	debug_clk[1] = dsp_clk;	
   
endmodule // eth_test


// Local Variables:
// verilog-library-directories:("." "subdir" "subdir2")
// verilog-library-files:("/home/matt/u2f/opencores/wb_conbus/rtl/verilog/wb_conbus_top.v")
// verilog-library-extensions:(".v" ".h")
// End:
