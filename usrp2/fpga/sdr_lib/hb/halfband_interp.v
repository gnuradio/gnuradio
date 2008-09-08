

module halfband_interp 
  (input clock, input reset, input enable,
   input strobe_in, input strobe_out,
   input [15:0] signal_in_i, input [15:0] signal_in_q, 
   output reg [15:0] signal_out_i, output reg [15:0] signal_out_q,
   output wire [12:0] debug);
   
   wire [15:0] 	coeff_ram_out;
   wire [15:0] 	data_ram_out_i;
   wire [15:0] 	data_ram_out_q;

   wire [3:0] 	data_rd_addr;
   reg [3:0] 	data_wr_addr;
   reg [2:0] 	coeff_rd_addr;

   wire 		filt_done;
   
   wire [15:0] 	mac_out_i;
   wire [15:0] 	mac_out_q;
   reg [15:0] 	delayed_middle_i, delayed_middle_q;
   wire [7:0] 	shift = 8'd9;

   reg 		stb_out_happened;

   wire [15:0] 	data_ram_out_i_b;
   
   always @(posedge clock)
     if(strobe_in)
       stb_out_happened <= #1 1'b0;
     else if(strobe_out)
       stb_out_happened <= #1 1'b1;
   
assign debug = {filt_done,data_rd_addr,data_wr_addr,coeff_rd_addr};

   wire [15:0] 	signal_out_i = stb_out_happened ? mac_out_i : delayed_middle_i;
   wire [15:0] 	signal_out_q = stb_out_happened ? mac_out_q : delayed_middle_q;

/*   always @(posedge clock)
     if(reset)
       begin
	  signal_out_i <= #1 16'd0;
	  signal_out_q <= #1 16'd0;
       end
     else if(strobe_in)
       begin
	  signal_out_i <= #1 delayed_middle_i; // Multiply by 1 for middle coeff
	  signal_out_q <= #1 delayed_middle_q;
       end
     //else if(filt_done&stb_out_happened)
     else if(stb_out_happened)
       begin
	  signal_out_i <= #1 mac_out_i;
	  signal_out_q <= #1 mac_out_q;
       end
*/
   
   always @(posedge clock)
     if(reset)
       coeff_rd_addr <= #1 3'd0;
     else if(coeff_rd_addr != 3'd0)
       coeff_rd_addr <= #1 coeff_rd_addr + 3'd1;
     else if(strobe_in)
       coeff_rd_addr <= #1 3'd1;

   reg filt_done_d1;
   always@(posedge clock)
     filt_done_d1 <= #1 filt_done;
   
   always @(posedge clock)
     if(reset)
       data_wr_addr <= #1 4'd0;
   //else if(strobe_in)
     else if(filt_done & ~filt_done_d1)
       data_wr_addr <= #1 data_wr_addr + 4'd1;

   always @(posedge clock)
     if(coeff_rd_addr == 3'd7)
       begin
	  delayed_middle_i <= #1 data_ram_out_i_b;
	//  delayed_middle_q <= #1 data_ram_out_q_b;
       end
   
//   always @(posedge clock)
//     if(reset)
//       data_rd_addr <= #1 4'd0;
//     else if(strobe_in)
//       data_rd_addr <= #1 data_wr_addr + 4'd1;
//     else if(!filt_done)
//       data_rd_addr <= #1 data_rd_addr + 4'd1;
//     else
//       data_rd_addr <= #1 data_wr_addr;
  
   wire [3:0] data_rd_addr1 = data_wr_addr + {1'b0,coeff_rd_addr};
   wire [3:0] data_rd_addr2 = data_wr_addr + 15 - {1'b0,coeff_rd_addr};
//   always @(posedge clock)
//     if(reset)
//       filt_done <= #1 1'b1;
//     else if(strobe_in)
 //      filt_done <= #1 1'b0;
//     else if(coeff_rd_addr == 4'd0)
//       filt_done <= #1 1'b1;

   assign filt_done = (coeff_rd_addr == 3'd0);
   
   coeff_ram coeff_ram ( .clock(clock),.rd_addr({1'b0,coeff_rd_addr}),.rd_data(coeff_ram_out) );
   
   ram16_2sum data_ram_i ( .clock(clock),.write(strobe_in),.wr_addr(data_wr_addr),.wr_data(signal_in_i),
		      .rd_addr1(data_rd_addr1),.rd_addr2(data_rd_addr2),.rd_data(data_ram_out_i_b),.sum(data_ram_out_i));
   
   ram16_2sum data_ram_q ( .clock(clock),.write(strobe_in),.wr_addr(data_wr_addr),.wr_data(signal_in_q),
		      .rd_addr1(data_rd_addr1),.rd_addr2(data_rd_addr2),.rd_data(data_ram_out_q));
   
   mac mac_i (.clock(clock),.reset(reset),.enable(~filt_done),.clear(strobe_in),
	      .x(data_ram_out_i),.y(coeff_ram_out),.shift(shift),.z(mac_out_i) );
   
   mac mac_q (.clock(clock),.reset(reset),.enable(~filt_done),.clear(strobe_in),
	      .x(data_ram_out_q),.y(coeff_ram_out),.shift(shift),.z(mac_out_q) );

endmodule // halfband_interp
