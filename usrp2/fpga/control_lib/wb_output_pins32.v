

// Simple 32-bit Wishbone compatible slave output port
// with 8-bit granularity, modeled after the one in the spec
// Allows for readback
// Assumes a 32-bit wishbone bus
// Lowest order bits get sel[0]
//

module wb_output_pins32
  (wb_rst_i, wb_clk_i, wb_dat_i, wb_dat_o,
   wb_we_i, wb_sel_i, wb_stb_i, wb_ack_o, wb_cyc_i,
   port_output);

   input wb_rst_i;
   input wb_clk_i;
   input wire [31:0] wb_dat_i;
   output wire [31:0] wb_dat_o;
   input  wb_we_i;
   input  wire [3:0] 	 wb_sel_i;
   input  wb_stb_i;
   output wb_ack_o;
   input  wb_cyc_i;

   output wire [31:0] port_output;

   reg [31:0] internal_reg;

   always @(posedge wb_clk_i)
     if(wb_rst_i)
       internal_reg <= #1 32'b0;
     else
       begin
	  if(wb_stb_i & wb_we_i & wb_sel_i[0])
	    internal_reg[7:0] <= #1 wb_dat_i[7:0];
	  if(wb_stb_i & wb_we_i & wb_sel_i[1])
	    internal_reg[15:8] <= #1 wb_dat_i[15:8];
	  if(wb_stb_i & wb_we_i & wb_sel_i[2])
	    internal_reg[23:16] <= #1 wb_dat_i[23:16];
	  if(wb_stb_i & wb_we_i & wb_sel_i[3])
	    internal_reg[31:24] <= #1 wb_dat_i[31:24];
       end // else: !if(wb_rst_i)

   assign wb_dat_o = internal_reg;
   assign port_output = internal_reg;
   assign wb_ack_o = wb_stb_i;
   	     
endmodule // wb_output_pins32

