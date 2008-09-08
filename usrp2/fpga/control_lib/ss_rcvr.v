

// Source-synchronous receiver
// Assumes both clocks are at the same rate
// Relative clock phase is
//    unknown
//    variable
//    bounded
// The output will come several cycles later than the input

// This should synthesize efficiently in Xilinx distributed ram cells,
//   which is why we use a buffer depth of 16

// FIXME Async reset on rxclk side?

module ss_rcvr
  #(parameter WIDTH=16)
    (input rxclk,
     input sysclk,
     input rst,
     
     input [WIDTH-1:0] data_in,
     output [WIDTH-1:0] data_out,
     output reg clock_present);
   
   wire [3:0] rd_addr, wr_addr;

   // Distributed RAM
   reg [WIDTH-1:0] buffer [0:15];
   always @(posedge rxclk)
     buffer[wr_addr] <= data_in;
   
   assign 	   data_out = buffer[rd_addr];
   
   // Write address generation
   reg [3:0] 	   wr_counter;
   always @(posedge rxclk or posedge rst)
     if (rst)
       wr_counter <= 0;
     else
       wr_counter <= wr_counter + 1;
   
   assign 	   wr_addr = {wr_counter[3], ^wr_counter[3:2], ^wr_counter[2:1], ^wr_counter[1:0]};
   
   // Read Address generation
   wire [3:0] 	   wr_ctr_sys, diff, abs_diff;
   reg [3:0] 	   wr_addr_sys_d1, wr_addr_sys_d2;
   reg [3:0] 	   rd_counter;
   
   assign 	   rd_addr = {rd_counter[3], ^rd_counter[3:2], ^rd_counter[2:1], ^rd_counter[1:0]};
   
   always @(posedge sysclk)
     wr_addr_sys_d1 <= wr_addr;
   
   always @(posedge sysclk)
     wr_addr_sys_d2 <= wr_addr_sys_d1;
   
   assign 	   wr_ctr_sys = {wr_addr_sys_d2[3],^wr_addr_sys_d2[3:2],^wr_addr_sys_d2[3:1],^wr_addr_sys_d2[3:0]};
   
   assign 	   diff = wr_ctr_sys - rd_counter;
   assign 	   abs_diff = diff[3] ? (~diff+1) : diff;
   
   always @(posedge sysclk)
     if(rst)
       begin
	  clock_present <= 0;
	  rd_counter <= 0;
       end
     else 
       if(~clock_present)
	 if(abs_diff > 5)
	   clock_present <= 1;
	 else
	   ;
       else
	 if(abs_diff<3)
	   clock_present <= 0;
	 else
	   rd_counter <= rd_counter + 1;

endmodule // ss_rcvr
