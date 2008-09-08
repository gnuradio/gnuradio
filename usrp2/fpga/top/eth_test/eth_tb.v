`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////

// Nearly everything is an input

module eth_tb();
   // Misc, debug
   wire led1;
   wire led2;
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
   wire        adc_oen_a;
   wire        adc_pdn_a;
   
   wire [13:0] adc_b;
   wire        adc_ovf_b;
   wire        adc_oen_b;
   wire        adc_pdn_b;
   
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
   reg        clk_fpga;
   wire        clk_to_mac;
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
   
   wire        wb_clk, wb_rst;
   wire        start, clock_ready;
   
   reg 	       aux_clk;

   initial aux_clk= 1'b0;
   always #25 aux_clk = ~aux_clk;
   
   initial clk_fpga = 1'bx;
   initial #3007 clk_fpga = 1'b0;
   always #7 clk_fpga = ~clk_fpga;


   wire        div_clk;
   reg [2:0]   div_ctr = 0;
   
   always @(posedge clk_fpga or negedge clk_fpga)
     if(div_ctr==5)
       div_ctr = 0;
     else
       div_ctr = div_ctr + 1;
   assign      div_clk = (div_ctr == 0) | (div_ctr == 1) | (div_ctr == 2);
   
   assign      dsp_clk = clk_fpga;
   assign      wb_clk = clock_ready ? div_clk : aux_clk;

   initial
     $monitor($time, ,clock_ready);
   
   initial begin
      $dumpfile("eth_tb.vcd");
      $dumpvars(0,eth_tb);
   end

   initial #10000000 $finish;

   cpld_model 
     cpld_model (.aux_clk(aux_clk),.start(cpld_start),.mode(cpld_mode),.done(cpld_done),
		 .dout(cpld_din),.sclk(cpld_clk),.detached(cpld_detached));
   
   eth_sim_top eth_sim_top(.dsp_clk		(dsp_clk),
			   .wb_clk            (wb_clk),
			   .clock_ready	(clock_ready),
			   .clk_to_mac	(clk_to_mac),
			   .pps_in		(pps_in),
			   .led1		(led1),
			   .led2		(led2),
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
			   .PHY_CLK		(PHY_CLK),
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
			   .adc_a		(adc_a[13:0]),
			   .adc_ovf_a		(adc_ovf_a),
			   .adc_oen_a		(adc_oen_a),
			   .adc_pdn_a		(adc_pdn_a),
			   .adc_b		(adc_b[13:0]),
			   .adc_ovf_b		(adc_ovf_b),
			   .adc_oen_b		(adc_oen_b),
			   .adc_pdn_b		(adc_pdn_b),
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
			   .io_rx		(io_rx[15:0]));
   
   // Experimental printf-like function
   always @(posedge wb_clk)
     begin
	if((eth_sim_top.m0_we == 1'd1)&&(eth_sim_top.m0_adr == 16'hC000))
	  $write("%x",eth_sim_top.m0_dat_i);
	if((eth_sim_top.m0_we == 1'd1)&&(eth_sim_top.m0_adr == 16'hC100))
	  $display("%x",eth_sim_top.m0_dat_i);
	if((eth_sim_top.m0_we == 1'd1)&&(eth_sim_top.m0_adr == 16'hC004))
	  $write("%c",eth_sim_top.m0_dat_i);
	if((eth_sim_top.m0_we == 1'd1)&&(eth_sim_top.m0_adr == 16'hC104))
	  $display("%c",eth_sim_top.m0_dat_i);
	if((eth_sim_top.m0_we == 1'd1)&&(eth_sim_top.m0_adr == 16'hC008))
	  $display("");
     end
	

endmodule // u2_sim_top

// Local Variables:
// verilog-library-directories:("." "subdir" "subdir2")
// verilog-library-files:("/home/matt/u2f/top/u2_basic/u2_basic.v")
// verilog-library-extensions:(".v" ".h")
// End:
