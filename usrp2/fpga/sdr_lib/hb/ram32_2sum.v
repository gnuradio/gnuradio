

module ram32_2sum (input clock, input write, 
		   input [4:0] wr_addr, input [15:0] wr_data,
		   input [4:0] rd_addr1, input [4:0] rd_addr2,
		   output reg [15:0] sum);
   
   reg [15:0] 			ram_array [0:31];
   wire [16:0] 			sum_int;
   
   always @(posedge clock)
     if(write)
       ram_array[wr_addr] <= #1 wr_data;

   assign sum_int = ram_array[rd_addr1] + ram_array[rd_addr2];

   always @(posedge clock)
     sum <= #1 sum_int[16:1] + (sum_int[16]&sum_int[0]);

   
endmodule // ram32_2sum

