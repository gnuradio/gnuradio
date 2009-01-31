

module time_64bit
  #(parameter TICKS_PER_SEC = 32'd100000000,
    parameter BASE = 0)
    (input clk, input rst,
     input set_stb, input [7:0] set_addr, input [31:0] set_data,  
     input pps,
     output [63:0] vita_time
     );

   localparam 	   NEXT_TICKS = 0;
   localparam 	   NEXT_SECS = 1;   
   localparam 	   ROLLOVER = TICKS_PER_SEC - 1;	   
   
   assign 	   vita_time = {seconds,ticks};
   
   wire [31:0] 	   next_ticks_preset;
   wire [31:0] 	   next_seconds_preset;
   wire 	   set_on_pps_trig;
   reg 		   set_on_next_pps;
   
   setting_reg #(.my_addr(BASE+NEXT_TICKS)) sr_next_ticks
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(next_ticks_preset),.changed());
   
   setting_reg #(.my_addr(BASE+NEXT_SECS)) sr_next_secs
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(next_seconds_preset),.changed(set_on_pps_trig));
   
   reg [31:0] 	   seconds;
   reg [31:0] 	   ticks;
   
   wire 	   end_of_second;
   
   always @(posedge clk)
     if(rst)
       set_on_next_pps <= 0;
     else if(set_on_pps_trig)
       set_on_next_pps <= 1;
     else if(pps)
       set_on_next_pps <= 0;
   
   always @(posedge clk)
     if(rst)
       begin
	  seconds <= 32'd0;
	  ticks <= 32'd0;
       end
     else if(pps & set_on_next_pps)
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
   
endmodule // time_64bit
