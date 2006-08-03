

module ram (input clock, input write, 
	    input [4:0] wr_addr, input [15:0] wr_data,
	    input [4:0] rd_addr, output reg [15:0] rd_data);

   reg [15:0] 		ram_array [0:31];

   always @(posedge clock)
     rd_data <= #1 ram_array[rd_addr];

   always @(posedge clock)
     if(write)
       ram_array[wr_addr] <= #1 wr_data;

endmodule // ram
