

module ram16_2port (input clock, input write, 
		    input [3:0] wr_addr, input [15:0] wr_data,
		    input [3:0] rd_addr1, output reg [15:0] rd_data1,
		    input [3:0] rd_addr2, output reg [15:0] rd_data2);
   
   reg [15:0] 			ram_array [0:31];
   
   always @(posedge clock)
     rd_data1 <= #1 ram_array[rd_addr1];
   
   always @(posedge clock)
     rd_data2 <= #1 ram_array[rd_addr2];
   
   always @(posedge clock)
     if(write)
       ram_array[wr_addr] <= #1 wr_data;
   
endmodule // ram16_2port


