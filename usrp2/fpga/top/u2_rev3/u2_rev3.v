`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////

module u2_rev3
  (
   // Misc, debug
   output [5:0] leds,
   output [31:0] debug,
   output [1:0] debug_clk,
   output uart_tx_o,
   input uart_rx_i,
   
   // Expansion
   input exp_pps_in_p, // Diff
   input exp_pps_in_n, // Diff
   output exp_pps_out_p, // Diff 
   output exp_pps_out_n, // Diff 
   
   // GMII
   //   GMII-CTRL
   input GMII_COL,
   input GMII_CRS,

   //   GMII-TX
   output reg [7:0] GMII_TXD,
   output reg GMII_TX_EN,
   output reg GMII_TX_ER,
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
   input PHY_CLK,   // possibly use on-board osc

   // RAM
   inout [17:0] RAM_D,
   output [18:0] RAM_A,
   output RAM_CE1n,
   output RAM_CENn,
   output RAM_CLK,
   output RAM_WEn,
   output RAM_OEn,
   output RAM_LDn,
   
   // SERDES
   output ser_enable,
   output ser_prbsen,
   output ser_loopen,
   output ser_rx_en,
   
   output ser_tx_clk,
   output reg [15:0] ser_t,
   output reg ser_tklsb,
   output reg ser_tkmsb,

   input ser_rx_clk,
   input [15:0] ser_r,
   input ser_rklsb,
   input ser_rkmsb,
   
   // CPLD interface
   output cpld_start,  // AA9
   output cpld_mode,   // U12
   output cpld_done,   // V12
   input cpld_din,     // AA14 Now shared with CFG_Din
   input cpld_clk,     // AB14 serial clock
   input cpld_detached,// V11 unused
   output cpld_init_b,  // W12 unused dual purpose
   output cpld_misc,  // Y12 

   // Watchdog interface
   input POR,
   output WDI,
   
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
   output reg [15:0] dac_a,
   output reg [15:0] dac_b,
   input dac_lock,     // unused for now
   
   // I2C
   inout SCL,
   inout SDA,

   // Clock Gen Control
   output [1:0] clk_en,
   output [1:0] clk_sel,
   input clk_func,        // FIXME is an input to control the 9510
   input clk_status,

   // Clocks
   input clk_fpga_p,  // Diff
   input clk_fpga_n,  // Diff
   input clk_to_mac,
   input pps_in,
   
   // Generic SPI
   output sclk,
   output sen_clk,
   output sen_dac,
   output sdi,
   input sdo,
   
   // TX DBoard
   output sen_tx_db,
   output sclk_tx_db,
   input sdo_tx_db,
   output sdi_tx_db,

   output sen_tx_adc,
   output sclk_tx_adc,
   input sdo_tx_adc,
   output sdi_tx_adc,

   output sen_tx_dac,
   output sclk_tx_dac,
   output sdi_tx_dac,

   inout [15:0] io_tx,

   // RX DBoard
   output sen_rx_db,
   output sclk_rx_db,
   input sdo_rx_db,
   output sdi_rx_db,

   output sen_rx_adc,
   output sclk_rx_adc,
   input sdo_rx_adc,
   output sdi_rx_adc,

   output sen_rx_dac,
   output sclk_rx_dac,
   output sdi_rx_dac,
   
   inout [15:0] io_rx   
   );

   assign 	cpld_init_b = 0;
   // FPGA-specific pins connections
   wire 	clk_fpga, dsp_clk, clk_div, dcm_out, wb_clk, clock_ready;
   wire 	clk90, clk180, clk270;

   // reset the watchdog continuously
   reg [15:0] 	wd;
   wire 	config_success;
   
   always @(posedge wb_clk)
     if(~config_success)
       wd <= 0;
     else
       wd <= wd + 1;
   assign 	WDI = wd[15];
   
   IBUFGDS clk_fpga_pin (.O(clk_fpga),.I(clk_fpga_p),.IB(clk_fpga_n));
   defparam 	clk_fpga_pin.IOSTANDARD = "LVPECL_25";
   
   wire 	exp_pps_in;
   IBUFDS exp_pps_in_pin (.O(exp_pps_in),.I(exp_pps_in_p),.IB(exp_pps_in_n));
   defparam 	exp_pps_in_pin.IOSTANDARD = "LVDS_25";
   
   wire 	exp_pps_out;
   OBUFDS exp_pps_out_pin (.O(exp_pps_out_p),.OB(exp_pps_out_n),.I(exp_pps_out));
   defparam 	exp_pps_out_pin.IOSTANDARD = "LVDS_25";

   reg [5:0] 	clock_ready_d;
   always @(posedge clk_fpga)
     clock_ready_d[5:0] <= {clock_ready_d[4:0],clock_ready};
   wire 	dcm_rst = ~&clock_ready_d & |clock_ready_d;
   
   wire 	adc_on_a, adc_on_b, adc_oe_a, adc_oe_b;
   assign 	adc_oen_a = ~adc_oe_a;
   assign 	adc_oen_b = ~adc_oe_b;
   assign 	adc_pdn_a = ~adc_on_a; 	
   assign 	adc_pdn_b = ~adc_on_b; 	

   reg [13:0] 	 adc_a_reg1, adc_b_reg1, adc_a_reg2, adc_b_reg2;
   reg 		 adc_ovf_a_reg1, adc_ovf_a_reg2, adc_ovf_b_reg1, adc_ovf_b_reg2;

   always @(posedge dsp_clk)
     begin
	adc_a_reg1 <= adc_a;
	adc_b_reg1 <= adc_b;
	adc_ovf_a_reg1 <= adc_ovf_a;
	adc_ovf_b_reg1 <= adc_ovf_b;
     end
   
   always @(posedge dsp_clk)
     begin
	adc_a_reg2 <= adc_a_reg1;
	adc_b_reg2 <= adc_b_reg1;
	adc_ovf_a_reg2 <= adc_ovf_a_reg1;
	adc_ovf_b_reg2 <= adc_ovf_b_reg1;
     end // always @ (posedge dsp_clk)

   // Handle Clocks
   DCM DCM_INST (.CLKFB(dsp_clk), 
                 .CLKIN(clk_fpga), 
                 .DSSEN(0), 
                 .PSCLK(0), 
                 .PSEN(0), 
                 .PSINCDEC(0), 
                 .RST(dcm_rst), 
                 .CLKDV(clk_div), 
                 .CLKFX(), 
                 .CLKFX180(), 
                 .CLK0(dcm_out), 
                 .CLK2X(), 
                 .CLK2X180(), 
                 .CLK90(clk90), 
                 .CLK180(clk180), 
                 .CLK270(clk270), 
                 .LOCKED(LOCKED_OUT), 
                 .PSDONE(), 
                 .STATUS());
   defparam DCM_INST.CLK_FEEDBACK = "1X";
   defparam DCM_INST.CLKDV_DIVIDE = 2.0;
   defparam DCM_INST.CLKFX_DIVIDE = 1;
   defparam DCM_INST.CLKFX_MULTIPLY = 4;
   defparam DCM_INST.CLKIN_DIVIDE_BY_2 = "FALSE";
   defparam DCM_INST.CLKIN_PERIOD = 10.000;
   defparam DCM_INST.CLKOUT_PHASE_SHIFT = "NONE";
   defparam DCM_INST.DESKEW_ADJUST = "SYSTEM_SYNCHRONOUS";
   defparam DCM_INST.DFS_FREQUENCY_MODE = "LOW";
   defparam DCM_INST.DLL_FREQUENCY_MODE = "LOW";
   defparam DCM_INST.DUTY_CYCLE_CORRECTION = "TRUE";
   defparam DCM_INST.FACTORY_JF = 16'h8080;
   defparam DCM_INST.PHASE_SHIFT = 0;
   defparam DCM_INST.STARTUP_WAIT = "FALSE";

   BUFG dspclk_BUFG (.I(dcm_out), .O(dsp_clk));
   BUFG wbclk_BUFG (.I(clk_div), .O(wb_clk));

   // I2C -- Don't use external transistors for open drain, the FPGA implements this
   IOBUF scl_pin(.O(scl_pad_i), .IO(SCL), .I(scl_pad_o), .T(scl_pad_oen_o));
   IOBUF sda_pin(.O(sda_pad_i), .IO(SDA), .I(sda_pad_o), .T(sda_pad_oen_o));

   // LEDs are active low outputs
   wire [5:0] leds_int;
   assign     leds = 6'b011111 ^ leds_int;  // all except eth are active-low
   
   // SPI
   wire 	miso, mosi, sclk_int;
   assign 	{sclk,sdi} = (~sen_clk | ~sen_dac) ? {sclk_int,mosi} : 2'b0;
   assign 	{sclk_tx_db,sdi_tx_db} = ~sen_tx_db ? {sclk_int,mosi} : 2'b0;
   assign 	{sclk_tx_dac,sdi_tx_dac} = ~sen_tx_dac ? {sclk_int,mosi} : 2'b0;
   assign 	{sclk_tx_adc,sdi_tx_adc} = ~sen_tx_adc ? {sclk_int,mosi} : 2'b0;
   assign 	{sclk_rx_db,sdi_rx_db} = ~sen_rx_db ? {sclk_int,mosi} : 2'b0;
   assign 	{sclk_rx_dac,sdi_rx_dac} = ~sen_rx_dac ? {sclk_int,mosi} : 2'b0;
   assign 	{sclk_rx_adc,sdi_rx_adc} = ~sen_rx_adc ? {sclk_int,mosi} : 2'b0;
   
   assign 	miso = (~sen_clk & sdo) | (~sen_dac & sdo) | 
		(~sen_tx_db & sdo_tx_db) | (~sen_tx_adc & sdo_tx_adc) |
		(~sen_rx_db & sdo_rx_db) | (~sen_rx_adc & sdo_rx_adc);

   wire 	GMII_TX_EN_unreg, GMII_TX_ER_unreg;
   wire [7:0] 	GMII_TXD_unreg;
   wire 	GMII_GTX_CLK_int;
   
   always @(posedge GMII_GTX_CLK_int)
     begin
	GMII_TX_EN <= GMII_TX_EN_unreg;
	GMII_TX_ER <= GMII_TX_ER_unreg;
	GMII_TXD <= GMII_TXD_unreg;
     end

   OFDDRRSE OFDDRRSE_gmii_inst 
     (.Q(GMII_GTX_CLK),      // Data output (connect directly to top-level port)
      .C0(GMII_GTX_CLK_int),    // 0 degree clock input
      .C1(~GMII_GTX_CLK_int),    // 180 degree clock input
      .CE(1),    // Clock enable input
      .D0(0),    // Posedge data input
      .D1(1),    // Negedge data input
      .R(0),      // Synchronous reset input
      .S(0)       // Synchronous preset input
      );
   
   wire ser_tklsb_unreg, ser_tkmsb_unreg;
   wire [15:0] ser_t_unreg;
   wire        ser_tx_clk_int;
   
   always @(posedge ser_tx_clk_int)
     begin
	ser_tklsb <= ser_tklsb_unreg;
	ser_tkmsb <= ser_tkmsb_unreg;
	ser_t <= ser_t_unreg;
     end

   assign ser_tx_clk = clk_fpga;

   reg [15:0] ser_r_int;
   reg 	      ser_rklsb_int, ser_rkmsb_int;

   always @(posedge ser_rx_clk)
     begin
	ser_r_int <= ser_r;
	ser_rklsb_int <= ser_rklsb;
	ser_rkmsb_int <= ser_rkmsb;
     end

   wire [15:0] dac_a_int, dac_b_int;
   always @(negedge dsp_clk) dac_a <= dac_a_int;
   always @(negedge dsp_clk) dac_b <= dac_b_int;

   /*
   OFDDRRSE OFDDRRSE_serdes_inst 
     (.Q(ser_tx_clk),      // Data output (connect directly to top-level port)
      .C0(ser_tx_clk_int),    // 0 degree clock input
      .C1(~ser_tx_clk_int),    // 180 degree clock input
      .CE(1),    // Clock enable input
      .D0(0),    // Posedge data input
      .D1(1),    // Negedge data input
      .R(0),      // Synchronous reset input
      .S(0)       // Synchronous preset input
      );
   */
   u2_core #(.RAM_SIZE(32768))
	u2_core(.dsp_clk           (dsp_clk),
		     .wb_clk            (wb_clk),
		     .clock_ready       (clock_ready),
		     .clk_to_mac	(clk_to_mac),
		     .pps_in		(pps_in),
		     .leds		(leds_int),
		     .debug		(debug[31:0]),
		     .debug_clk		(debug_clk[1:0]),
		     .exp_pps_in	(exp_pps_in),
		     .exp_pps_out	(exp_pps_out),
		     .GMII_COL		(GMII_COL),
		     .GMII_CRS		(GMII_CRS),
		     .GMII_TXD		(GMII_TXD_unreg[7:0]),
		     .GMII_TX_EN	(GMII_TX_EN_unreg),
		     .GMII_TX_ER	(GMII_TX_ER_unreg),
		     .GMII_GTX_CLK	(GMII_GTX_CLK_int),
		     .GMII_TX_CLK	(GMII_TX_CLK),
		     .GMII_RXD		(GMII_RXD[7:0]),
		     .GMII_RX_CLK	(GMII_RX_CLK),
		     .GMII_RX_DV	(GMII_RX_DV),
		     .GMII_RX_ER	(GMII_RX_ER),
		     .MDIO		(MDIO),
		     .MDC		(MDC),
		     .PHY_INTn		(PHY_INTn),
		     .PHY_RESETn	(PHY_RESETn),
		     .ser_enable	(ser_enable),
		     .ser_prbsen	(ser_prbsen),
		     .ser_loopen	(ser_loopen),
		     .ser_rx_en		(ser_rx_en),
		     .ser_tx_clk	(ser_tx_clk_int),
		     .ser_t		(ser_t_unreg[15:0]),
		     .ser_tklsb		(ser_tklsb_unreg),
		     .ser_tkmsb		(ser_tkmsb_unreg),
		     .ser_rx_clk	(ser_rx_clk),
		     .ser_r		(ser_r_int[15:0]),
		     .ser_rklsb		(ser_rklsb_int),
		     .ser_rkmsb		(ser_rkmsb_int),
		     .cpld_start        (cpld_start),
		     .cpld_mode         (cpld_mode),
		     .cpld_done         (cpld_done),
		     .cpld_din          (cpld_din),
		     .cpld_clk          (cpld_clk),
		     .cpld_detached     (cpld_detached),
		     .cpld_misc         (cpld_misc),
		     .cpld_init_b       (cpld_init_b),
		     .por               (~POR),
		     .config_success    (config_success),
		     .adc_a		(adc_a_reg2),
		     .adc_ovf_a		(adc_ovf_a_reg2),
		     .adc_on_a		(adc_on_a),
		     .adc_oe_a		(adc_oe_a),
		     .adc_b		(adc_b_reg2),
		     .adc_ovf_b		(adc_ovf_b_reg2),
		     .adc_on_b		(adc_on_b),
		     .adc_oe_b		(adc_oe_b),
		     .dac_a		(dac_a_int),
		     .dac_b		(dac_b_int),
		     .scl_pad_i		(scl_pad_i),
		     .scl_pad_o		(scl_pad_o),
		     .scl_pad_oen_o	(scl_pad_oen_o),
		     .sda_pad_i		(sda_pad_i),
		     .sda_pad_o		(sda_pad_o),
		     .sda_pad_oen_o	(sda_pad_oen_o),
		     .clk_en		(clk_en[1:0]),
		     .clk_sel		(clk_sel[1:0]),
		     .clk_func		(clk_func),
		     .clk_status	(clk_status),
		     .sclk		(sclk_int),
		     .mosi		(mosi),
		     .miso		(miso),
		     .sen_clk		(sen_clk),
		     .sen_dac		(sen_dac),
		     .sen_tx_db		(sen_tx_db),
		     .sen_tx_adc	(sen_tx_adc),
		     .sen_tx_dac	(sen_tx_dac),
		     .sen_rx_db		(sen_rx_db),
		     .sen_rx_adc	(sen_rx_adc),
		     .sen_rx_dac	(sen_rx_dac),
		     .io_tx		(io_tx[15:0]),
		     .io_rx		(io_rx[15:0]),
		     .RAM_D             (RAM_D),
		     .RAM_A             (RAM_A),
		     .RAM_CE1n          (RAM_CE1n),
		     .RAM_CENn          (RAM_CENn),
		     .RAM_CLK           (RAM_CLK),
		     .RAM_WEn           (RAM_WEn),
		     .RAM_OEn           (RAM_OEn),
		     .RAM_LDn           (RAM_LDn), 
		     .uart_tx_o         (uart_tx_o),
		     .uart_rx_i         (uart_rx_i),
		     .uart_baud_o       (),
		     .sim_mode          (1'b0),
		     .clock_divider     (2)
		     );
   
endmodule // u2_rev2
