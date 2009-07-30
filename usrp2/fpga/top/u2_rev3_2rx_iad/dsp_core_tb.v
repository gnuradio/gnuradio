`timescale 1ns / 100ps

module dsp_core_tb;

///////////////////////////////////////////////////////////////////////////////////
// Sim-wide wires/busses                                                         //
///////////////////////////////////////////////////////////////////////////////////
   
   // System control bus
   reg                clk = 0;
   reg                rst = 1;
   
   // Configuration bus
   reg                set_stb = 0;  
   reg          [7:0] set_addr = 0; 
   reg         [31:0] set_data = 0; 

   // ADC input bus
   wire signed [13:0] adc_a;
   wire signed [13:0] adc_b;
   wire               adc_ovf_a;
   wire               adc_ovf_b;
   
   // RX sample bus
   reg                run = 1;
   wire        [31:0] sample;
   wire               stb;
   
///////////////////////////////////////////////////////////////////////////////////
// Simulation control                                                            //
///////////////////////////////////////////////////////////////////////////////////
   
   // Set up output files
   initial begin
      $dumpfile("dsp_core_tb.vcd");
      $dumpvars(0,dsp_core_tb);
   end

   // Update display every 10 us
   always #1000 $monitor("Time in us ",$time/1000);

   // Generate master clock 50% @ 100 MHz
   always
     #5 clk = ~clk;

///////////////////////////////////////////////////////////////////////////////////
// Unit(s) under test                                                            //
///////////////////////////////////////////////////////////////////////////////////
   
   reg [13:0] amplitude = 13'h1fff;
   reg [15:0] impulse_len = 0;
   reg [15:0] zero_len = 0;
   reg 	      adc_ena = 0;

   initial #500 @(posedge clk) adc_ena = 1;

   impulse adc
     (.clk(clk),.rst(rst),.ena(adc_ena),
      .dc_offset_a(0),.dc_offset_b(0),
      .amplitude(amplitude),
      .impulse_len(impulse_len),.zero_len(zero_len),
      .adc_a(adc_a),.adc_b(adc_b),
      .adc_ovf_a(adc_ovf_a),.adc_ovf_b(adc_ovf_b) );

   initial rx_path.rx_dcoffset_a.integrator = 0; // so sim doesn't propagate X's
   initial rx_path.rx_dcoffset_b.integrator = 0; // generated before reset
   dsp_core_rx rx_path
     (.clk(clk),.rst(rst),
      .set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .adc_a(adc_a),.adc_ovf_a(adc_ovf_a),
      .adc_b(adc_b),.adc_ovf_b(adc_ovf_b),
      .io_rx(16'b0),
      .run(adc_ena),.sample(sample),.strobe(stb),
      .debug() );

   reg  [31:0] master_time = 0;
   always @(posedge clk)
     master_time <= master_time + 1;

   reg         wr_ready    = 1;
   reg         wr_full     = 0;

   wire [31:0] wr_dat;
   wire        wr_write;
   wire        wr_done;
   wire        wr_error;
   wire [15:0] fifo_occupied;
   wire        fifo_full;
   wire        fifo_empty;
   
   rx_control rx_buffer
    (.clk(clk),.rst(rst),
     .set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
     .master_time(master_time),
     .overrun(), // unconnected output
     .wr_dat_o(wr_dat),
     .wr_write_o(wr_write),
     .wr_done_o(wr_done),
     .wr_error_o(wr_error),
     .wr_ready_i(wr_ready),
     .wr_full_i(wr_full),
     .sample(sample),
     .run(), // unconnected output, supposed to drive 'run'
     .strobe(stb),
     .fifo_occupied(fifo_occupied),
     .fifo_full(fifo_full),
     .fifo_empty(fifo_empty),
     .debug_rx() // unconnected output
     );


   
///////////////////////////////////////////////////////////////////////////////////
// Simulation output/checking                                                    //
///////////////////////////////////////////////////////////////////////////////////

   integer rx_file;
   
   initial
     rx_file = $fopen("rx.dat", "wb");
   
   always @(posedge clk)
     begin
	// Write RX sample I&Q in format Octave can load
	if (stb)
	  begin
	     $fwrite(rx_file, sample[31:16]);
	     $fputc(32, rx_file);
	     $fwrite(rx_file, sample[15:0]);
	     $fputc(13, rx_file);
	  end
     end
   
///////////////////////////////////////////////////////////////////////////////////
// Tasks                                                                         //
///////////////////////////////////////////////////////////////////////////////////

   task power_on;
     begin
	@(posedge clk)
	  rst = #1 1'b1;
	@(posedge clk)
	  rst = #1 1'b0;
     end
   endtask // power_on
   
   task set_impulse_len;
      input [15:0] len;
      @(posedge clk) impulse_len = len-1;
   endtask
 
   task set_zero_len;
      input [15:0] len;
      @(posedge clk) zero_len = len-1;
   endtask

   // Strobe configuration bus with addr, data
   task write_cfg_register;
      input [7:0]  regno;
      input [31:0] value;
      
      begin
	 @(posedge clk);
	 set_addr <= regno;
	 set_data <= value;
	 set_stb  <= 1'b1;
	 @(posedge clk);
	 set_stb  <= 1'b0;
      end
   endtask // write_cfg_register

   // Set RX DDC frequency
   task set_ddc_freq;
      input [31:0] freq;

      write_cfg_register(160, freq);
   endtask // set_ddc_freq

   // Set RX IQ scaling registers
   task set_rx_scale_iq;
      input [15:0] scale_i;
      input [15:0] scale_q;

      write_cfg_register(161, {scale_i,scale_q});
   endtask // set_rx_scale_iq
   
   // Set RX MUX control
   task set_rx_muxctrl;
      input [3:0] muxctrl;

      write_cfg_register(168, muxctrl);
   endtask // set_rx_muxctrl
   
   // Set RX CIC decim and halfband enables
   task set_decim;
      input hb1_ena;
      input hb2_ena;
      input [7:0] decim;

      write_cfg_register(162, {hb1_ena,hb2_ena,decim});
   endtask // set_decim
      
   
///////////////////////////////////////////////////////////////////////////////////
// Individual tests                                                              //
///////////////////////////////////////////////////////////////////////////////////

   task test_rx;
      begin
	 set_impulse_len(10);
	 set_zero_len(990);
	 set_rx_muxctrl(1);
	 set_ddc_freq(32'h10000000);
	 set_rx_scale_iq(1243, 1243);
	 set_decim(1, 1, 1);

	 #100000 $finish;
      end
   endtask // test_rx
   
      
///////////////////////////////////////////////////////////////////////////////////
// Top-level test                                                                //
///////////////////////////////////////////////////////////////////////////////////

   // Execute tests
   initial
     begin
	power_on();
	test_rx();
     end

endmodule // dsp_core_tb
