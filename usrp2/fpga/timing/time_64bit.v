

module time_64bit
  #(parameter TICKS_PER_SEC = 32'd100000000,
    parameter BASE = 0)
    (input clk, input rst,
     input set_stb, input [7:0] set_addr, input [31:0] set_data,  
     input pps,
     output [63:0] vita_time, output pps_int
     );

   localparam 	   NEXT_SECS = 0;   
   localparam 	   NEXT_TICKS = 1;
<<<<<<< HEAD:usrp2/fpga/timing/time_64bit.v
   localparam      PPS_POL = 2;
=======
   localparam      PPS_POLSRC = 2;
   localparam      PPS_IMM = 3;
>>>>>>> 23e45f33ba50b6658072e9d17c3cd25c0fb1ca7a:usrp2/fpga/timing/time_64bit.v
   
   localparam 	   ROLLOVER = TICKS_PER_SEC - 1;	   
   
   reg [31:0] 	   seconds;
   reg [31:0] 	   ticks;
   wire 	   end_of_second;
   assign 	   vita_time = {seconds,ticks};
   
   wire [31:0] 	   next_ticks_preset;
   wire [31:0] 	   next_seconds_preset;
   wire 	   set_on_pps_trig;
   reg 		   set_on_next_pps;
   wire 	   pps_polarity;
<<<<<<< HEAD:usrp2/fpga/timing/time_64bit.v
=======
   wire            set_imm;
   wire 	   pps_source;
>>>>>>> 23e45f33ba50b6658072e9d17c3cd25c0fb1ca7a:usrp2/fpga/timing/time_64bit.v
   
   setting_reg #(.my_addr(BASE+NEXT_TICKS)) sr_next_ticks
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(next_ticks_preset),.changed());
   
   setting_reg #(.my_addr(BASE+NEXT_SECS)) sr_next_secs
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(next_seconds_preset),.changed(set_on_pps_trig));

<<<<<<< HEAD:usrp2/fpga/timing/time_64bit.v
   setting_reg #(.my_addr(BASE+PPS_POL)) sr_pps_pol
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(pps_polarity),.changed());
=======
   setting_reg #(.my_addr(BASE+PPS_POLSRC)) sr_pps_polsrc
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out({pps_source,pps_polarity}),.changed());

   setting_reg #(.my_addr(BASE+PPS_IMM)) sr_pps_imm
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(set_imm),.changed());
>>>>>>> 23e45f33ba50b6658072e9d17c3cd25c0fb1ca7a:usrp2/fpga/timing/time_64bit.v

   reg [1:0] 	   pps_del;
   reg 		   pps_reg_p, pps_reg_n, pps_reg;
   wire 	   pps_edge;
   
   always @(posedge clk)  pps_reg_p <= pps;   
   always @(negedge clk)  pps_reg_n <= pps;
   always @* pps_reg <= pps_polarity ? pps_reg_p : pps_reg_n;
   
   always @(posedge clk)
     if(rst)
       pps_del <= 2'b00;
     else
       pps_del <= {pps_del[0],pps_reg};

   assign pps_edge = pps_del[0] & ~pps_del[1];
   
   always @(posedge clk)
     if(rst)
       set_on_next_pps <= 0;
     else if(set_on_pps_trig)
       set_on_next_pps <= 1;
<<<<<<< HEAD:usrp2/fpga/timing/time_64bit.v
     else if(pps_edge)
=======
     else if(set_imm | pps_edge)
>>>>>>> 23e45f33ba50b6658072e9d17c3cd25c0fb1ca7a:usrp2/fpga/timing/time_64bit.v
       set_on_next_pps <= 0;
   
   always @(posedge clk)
     if(rst)
       begin
	  seconds <= 32'd0;
	  ticks <= 32'd0;
       end
<<<<<<< HEAD:usrp2/fpga/timing/time_64bit.v
     else if(pps_edge & set_on_next_pps)
=======
     else if((set_imm | pps_edge) & set_on_next_pps)
>>>>>>> 23e45f33ba50b6658072e9d17c3cd25c0fb1ca7a:usrp2/fpga/timing/time_64bit.v
       begin
	  seconds <= next_seconds_preset;
	  ticks <= next_ticks_preset;
       end
     else if(ticks == ROLLOVER)
       begin
	  seconds <= seconds + 1;
	  ticks <= 0;
       end
     else
       ticks <= ticks + 1;

   assign pps_int = pps_edge;
   
endmodule // time_64bit
