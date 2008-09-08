
// Grab settings off the wishbone bus, send them out to our simpler bus on the fast clock

module settings_bus
  #(parameter AWIDTH=16, parameter DWIDTH=32)
    (input wb_clk, 
     input wb_rst, 
     input [AWIDTH-1:0] wb_adr_i,
     input [DWIDTH-1:0] wb_dat_i,
     input wb_stb_i,
     input wb_we_i,
     output reg wb_ack_o,
     input sys_clk,
     output strobe,
     output reg [7:0] addr,
     output reg [31:0] data);

   reg 	    stb_int, stb_int_d1;
   
   always @(posedge wb_clk)
     if(wb_rst)
       begin
	  stb_int <= 1'b0;
	  addr <= 8'd0;
	  data <= 32'd0;
       end
     else if(wb_we_i & wb_stb_i)
       begin
	  stb_int <= 1'b1;
	  addr <= wb_adr_i[9:2];
	  data <= wb_dat_i;
       end
     else
       stb_int <= 1'b0;

   always @(posedge wb_clk)
     if(wb_rst)
       wb_ack_o <= 0;
     else
       wb_ack_o <= wb_stb_i & ~wb_ack_o;

   always @(posedge wb_clk)
     stb_int_d1 <= stb_int;

   //assign strobe = stb_int & ~stb_int_d1;
   assign strobe = stb_int & wb_ack_o;
          
endmodule // settings_bus

