`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////

module single_u2_sim();
   // Misc, debug
   wire [7:0] leds;
   wire [31:0] debug;
   wire [1:0]  debug_clk;
   
   // Expansion
   wire        exp_pps_in;
   wire        exp_pps_out;
   
   // GMII
   //   GMII-CTRL
   wire        GMII_COL;
   wire        GMII_CRS;
   
   //   GMII-TX
   wire [7:0]  GMII_TXD;
   wire        GMII_TX_EN;
   wire        GMII_TX_ER;
   wire        GMII_GTX_CLK;
   wire        GMII_TX_CLK;  // 100mbps clk
   
   //   GMII-RX
   wire [7:0]  GMII_RXD;
   wire        GMII_RX_CLK;
   wire        GMII_RX_DV;
   wire        GMII_RX_ER;
   
   //   GMII-Management
   wire        MDIO;
   wire        MDC;
   wire        PHY_INTn;   // open drain
   wire        PHY_RESETn;
   wire        PHY_CLK;   // possibly use on-board osc
   
   // RAM
   wire [17:0] RAM_D;
   wire [18:0] RAM_A;
   wire        RAM_CE1n;
   wire        RAM_CENn;
   wire        RAM_CLK;
   wire        RAM_WEn;
   wire        RAM_OEn;
   wire        RAM_LDn;
   
   // SERDES
   wire        ser_enable;
   wire        ser_prbsen;
   wire        ser_loopen;
   wire        ser_rx_en;
   
   wire        ser_tx_clk;
   wire [15:0] ser_t;
   wire        ser_tklsb;
   wire        ser_tkmsb;
   
   wire        ser_rx_clk;
   wire [15:0] ser_r;
   wire        ser_rklsb;
   wire        ser_rkmsb;
   
   // CPLD interface
   wire        cpld_din, cpld_clk, cpld_detached, cpld_start, cpld_mode, cpld_done;
      
   // ADC
   wire [13:0] adc_a;
   wire        adc_ovf_a;
   wire        adc_on_a,  adc_oe_a;
   
   wire [13:0] adc_b;
   wire        adc_ovf_b;
   wire        adc_on_b, adc_oe_b;
   
   // DAC
   wire [15:0] dac_a;
   wire [15:0] dac_b;
   
   // I2C
   wire        SCL;
   wire        SDA;
   
   // Clock Gen Control
   wire [1:0]  clk_en;
   wire [1:0]  clk_sel;
   wire        clk_func;        // FIXME is an input to control the 9510
   wire        clk_status;
   
   // Clocks
   reg 	       clk_fpga;
   reg 	       clk_to_mac;
   wire        pps_in;
   
   // Generic SPI
   wire        sclk, mosi, miso;   
   wire        sen_clk;
   wire        sen_dac;
   wire        sen_tx_db;
   wire        sen_tx_adc;
   wire        sen_tx_dac;
   wire        sen_rx_db;
   wire        sen_rx_adc;
   wire        sen_rx_dac;
   
   // GPIO to DBoards
   wire [15:0] io_tx;
   wire [15:0] io_rx;
   
   wire        wb_clk;
   wire        start, clock_ready;
   
   reg 	       aux_clk;

   initial aux_clk= 1'b0;
   always #6 aux_clk = ~aux_clk;
   
   initial clk_fpga = 1'bx;
   initial #3007 clk_fpga = 1'b0;
   always #5 clk_fpga = ~clk_fpga;
   
   initial clk_to_mac = 0;
   always #4 clk_to_mac = ~clk_to_mac;
   
   wire        div_clk, dsp_clk;
   reg [7:0]   div_ctr = 0;
   
   assign      dsp_clk = clock_ready ? clk_fpga : aux_clk;
   assign      wb_clk = div_clk;

`define CLK_DIV_2 1
//`define CLK_DIV_3

`ifdef CLK_DIV_2
   localparam  clock_divider = 4'd2;
   always @(posedge dsp_clk)
     div_ctr <= div_ctr + 1;
   assign      div_clk = div_ctr[0];
`endif

`ifdef CLK_DIV_3
   localparam  clock_divider = 2;
   always @(posedge dsp_clk or negedge dsp_clk)
     if(div_ctr == 5)
       div_ctr <= 0;
     else
       div_ctr <= div_ctr + 1;
   assign      div_clk = ((div_ctr == 0) | (div_ctr == 1) | (div_ctr == 2));
`endif
   
   initial
     $monitor($time, ,clock_ready);

   always #1000000 $monitor("Time in ns ",$time);
   
   initial begin
      @(negedge cpld_done);
      @(posedge cpld_done);
      $dumpfile("single_u2_sim.lxt");
      $dumpvars(0,single_u2_sim);
   end

   initial #10000000 $finish;

   cpld_model 
     cpld_model (.aux_clk(aux_clk),.start(cpld_start),.mode(cpld_mode),.done(cpld_done),
		 .dout(cpld_din),.sclk(cpld_clk),.detached(cpld_detached));
     
   serdes_model serdes_model
     (.ser_tx_clk(ser_tx_clk), .ser_tkmsb(ser_tkmsb), .ser_tklsb(ser_tklsb), .ser_t(ser_t),
      .ser_rx_clk(ser_rx_clk), .ser_rkmsb(ser_rkmsb), .ser_rklsb(ser_rklsb), .ser_r(ser_r),
      .even(0),.error(0) );

   adc_model adc_model
     (.clk(dsp_clk),.rst(0),
      .adc_a(adc_a),.adc_ovf_a(adc_ovf_a),.adc_on_a(adc_on_a),.adc_oe_a(adc_oe_a),
      .adc_b(adc_b),.adc_ovf_b(adc_ovf_b),.adc_on_b(adc_on_b),.adc_oe_b(adc_oe_b) );

   wire [2:0] speed;
   Phy_sim phy_model
     (.Gtx_clk(GMII_GTX_CLK), . Rx_clk(GMII_RX_CLK), .Tx_clk(GMII_TX_CLK),
      .Tx_er(GMII_TX_ER), .Tx_en(GMII_TX_EN), .Txd(GMII_TXD),
      .Rx_er(GMII_RX_ER), .Rx_dv(GMII_RX_DV), .Rxd(GMII_RXD),
      .Crs(GMII_CRS), .Col(GMII_COL),
      .Speed(speed), .Done(0) );
   pullup p3(MDIO);
   
   miim_model miim_model
     (.mdc_i(MDC),.mdio(MDIO),.phy_resetn_i(PHY_RESETn),.phy_clk_i(PHY_CLK),
      .phy_intn_o(PHY_INTn),.speed_o(speed) );
   
   xlnx_glbl glbl (.GSR(),.GTS());

   wire       RAM_MODE = 1'b0;
   cy1356 ram_model(.d(RAM_D),.clk(RAM_CLK),.a(RAM_A),
		    .bws(2'b00),.we_b(RAM_WEn),.adv_lb(RAM_LDn),
		    .ce1b(RAM_CE1n),.ce2(1'b1),.ce3b(1'b0),
		    .oeb(RAM_OEn),.cenb(RAM_CENn),.mode(RAM_MODE) );
   
   M24LC024B eeprom_model(.A0(0),.A1(0),.A2(0),.WP(0),
			  .SDA(SDA),.SCL(SCL),.RESET(0));

   wire       scl_pad_i, scl_pad_o, scl_pad_oen_o;
   wire       sda_pad_i, sda_pad_o, sda_pad_oen_o;
   
   pullup p1(SCL);
   pullup p2(SDA);

   assign     scl_pad_i = SCL;
   assign     sda_pad_i = SDA;

   assign     SCL = scl_pad_oen_o ? 1'bz : scl_pad_o;
   assign     SDA = sda_pad_oen_o ? 1'bz : sda_pad_o;

   // printf output
   wire       uart_baud_o, uart_tx_o, uart_rx_i;
   assign     uart_rx_i = 1'b1;
   
   uart_rx uart_rx(.baudclk(uart_baud_o),.rxd(uart_tx_o));
   
   // End the simulation
   always @(posedge wb_clk)
     if((u2_core.m0_we == 1'd1)&&(u2_core.m0_adr == 16'hC2F0))
       begin
	  $display($time, "Finish called.",);
	  $finish;
       end
   
   u2_core #(.RAM_SIZE(32768))
            u2_core(.dsp_clk		(dsp_clk),
		     .wb_clk            (wb_clk),
		     .clock_ready	(clock_ready),
		     .clk_to_mac	(clk_to_mac),
		     .pps_in		(pps_in),
		     .leds		(leds),
		     .debug		(debug[31:0]),
		     .debug_clk		(debug_clk[1:0]),
		     .exp_pps_in	(exp_pps_in),
		     .exp_pps_out	(exp_pps_out),
		     .GMII_COL		(GMII_COL),
		     .GMII_CRS		(GMII_CRS),
		     .GMII_TXD		(GMII_TXD[7:0]),
		     .GMII_TX_EN	(GMII_TX_EN),
		     .GMII_TX_ER	(GMII_TX_ER),
		     .GMII_GTX_CLK	(GMII_GTX_CLK),
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
		     .ser_tx_clk	(ser_tx_clk),
		     .ser_t		(ser_t[15:0]),
		     .ser_tklsb		(ser_tklsb),
		     .ser_tkmsb		(ser_tkmsb),
		     .ser_rx_clk	(ser_rx_clk),
		     .ser_r		(ser_r[15:0]),
		     .ser_rklsb		(ser_rklsb),
		     .ser_rkmsb		(ser_rkmsb),
		     .cpld_start	(cpld_start),
		     .cpld_mode		(cpld_mode),
		     .cpld_done		(cpld_done),
		     .cpld_din		(cpld_din),
		     .cpld_clk		(cpld_clk),
		     .cpld_detached	(cpld_detached),
		     .cpld_init_b       (1),
		     .por               (0),
		     .adc_a		(adc_a[13:0]),
		     .adc_ovf_a		(adc_ovf_a),
		     .adc_on_a		(adc_on_a),
		     .adc_oe_a		(adc_oe_a),
		     .adc_b		(adc_b[13:0]),
		     .adc_ovf_b		(adc_ovf_b),
		     .adc_on_b		(adc_on_b),
		     .adc_oe_b		(adc_oe_b),
		     .dac_a		(dac_a[15:0]),
		     .dac_b		(dac_b[15:0]),
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
		     .sclk		(sclk),
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
		     .uart_baud_o       (uart_baud_o),
		     .sim_mode          (1'b1),
		     .clock_divider     (clock_divider)
		     );

endmodule // single_u2_sim
