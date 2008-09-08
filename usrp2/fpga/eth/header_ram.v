
module header_ram
  #(parameter REGNUM=0,
    parameter WIDTH=32)
    (input clk,
     input set_stb,
     input [7:0] set_addr,
     input [31:0] set_data,
     
     input [3:0] addr,
     output [31:0] q 
     );
   
   reg [WIDTH-1:0] mini_ram[0:15];
   wire 	   write_to_ram = (set_stb & (set_addr[7:4]==REGNUM[7:4]));
   wire [3:0] 	   ram_addr = write_to_ram ? set_addr[3:0] : addr;
		   
   always @(posedge clk)
     if(write_to_ram)
       mini_ram[ram_addr] <= set_data;

   assign 	   q = mini_ram[ram_addr];

endmodule // header_ram
