

// Since this is a block ram, there are no byte-selects and there is a 1-cycle read latency
//   These have to be a multiple of 512 lines (2K) long

module wb_ram_block
  #(parameter AWIDTH=9)
    (input clk_i,
     input stb_i,
     input we_i,
     input [AWIDTH-1:0] adr_i,
     input [31:0] dat_i,
     output reg [31:0] dat_o,
     output ack_o);

   reg [31:0] distram [0:1<<(AWIDTH-1)];

   always @(posedge clk_i)
     begin
	if(stb_i & we_i)
	  distram[adr_i] <= dat_i;
	dat_o <= distram[adr_i];
     end

   reg stb_d1, ack_d1;
   always @(posedge clk_i)
     stb_d1 <= stb_i;
   
   always @(posedge clk_i)
     ack_d1 <= ack_o;
   
   assign ack_o = stb_i & (we_i | (stb_d1 & ~ack_d1));
endmodule // wb_ram_block


    
