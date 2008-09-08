
// wb_bus_writer
//
// WB Bus Master device to send a sequence of single-word transactions
// based on a list in a RAM or ROM (FASM interface)
// ROM data format is {WB_ADDR[15:0],WB_DATA[31:0]}
// continues until it gets an all-1s entry

module wb_bus_writer (input start,
		      output done,
		      output reg [15:0] rom_addr,
		      input [47:0] rom_data,
		      // WB Master Interface, don't need wb_dat_i
		      input wb_clk_i,
		      input wb_rst_i,
		      output [31:0] wb_dat_o,
		      input wb_ack_i,
		      output [15:0] wb_adr_o,
		      output wb_cyc_o,
		      output [3:0] wb_sel_o,
		      output wb_stb_o,
		      output wb_we_o
		      );

`define IDLE 0
`define READ 1
   
   reg [3:0] 		     state;
   
   assign 		     done = (state != `IDLE) && (&rom_data);  // Done when we see all 1s
   
   always @(posedge wb_clk_i)
     if(wb_rst_i)
       begin
	  rom_addr <= #1 0;
	  state <= #1 0;
       end
     else if(start)
       begin
	  rom_addr <= #1 0;
	  state <= #1 `READ;
       end
     else if((state == `READ) && wb_ack_i)
       if(done)
	 state <= #1 `IDLE;
       else
	 rom_addr <= #1 rom_addr + 1;
   
   assign wb_dat_o = rom_data[31:0];
   assign wb_adr_o = rom_data[47:32];
   assign wb_sel_o = 4'b1111;    // All writes are the full 32 bits
   
   assign wb_cyc_o = !done & (state != `IDLE);
   assign wb_stb_o = !done & (state != `IDLE);
   assign wb_we_o = !done & (state != `IDLE);
   
endmodule // wb_bus_writer
