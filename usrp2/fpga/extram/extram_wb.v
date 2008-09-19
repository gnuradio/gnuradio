
module extram_wb
  #(parameter PAGE_SIZE = 10,
    parameter ADDR_WIDTH = 16)
    (input clk, input rst,
     input wb_clk, input wb_rst,
     input cyc_i, input stb_i,
     input [ADDR_WIDTH-1:0] adr_i,
     input we_i,
     input [31:0] dat_i,
     output reg [31:0] dat_o,
     output reg ack_o,
     
     inout [17:0] RAM_D,
     output [PAGE_SIZE-2:0] RAM_A,
     output RAM_CE1n, output RAM_CENn,
     output RAM_CLK, output RAM_WEn,
     output RAM_OEn, output RAM_LDn );

   wire     read_acc = stb_i & cyc_i & ~we_i;
   wire     write_acc = stb_i & cyc_i & we_i;
   wire     acc = stb_i & cyc_i;

   assign   RAM_CLK = wb_clk;   // 50 MHz for now, eventually should be 200 MHz
   assign   RAM_LDn = 0;        // No burst for now
   assign   RAM_CENn = 0;       // Use CE1n as our main CE

   reg [PAGE_SIZE-2:1] RAM_addr_reg;
   always @(posedge wb_clk)
     if(acc)
       RAM_addr_reg[PAGE_SIZE-2:1] <= adr_i[PAGE_SIZE-1:2];
   assign 	       RAM_A[PAGE_SIZE-2:1] = RAM_addr_reg; 

   reg [31:0] 	       ram_out;
   always @(posedge wb_clk)
     if(write_acc)
       ram_out <= dat_i;

   // RAM access state machine
   localparam RAM_idle = 0;
   localparam RAM_read_1 = 1;
   localparam RAM_read_2 = 2;
   localparam RAM_read_3 = 3;
   localparam RAM_read_4 = 4;
   localparam RAM_write_1 = 6;
   localparam RAM_write_2 = 7;
   localparam RAM_write_3 = 8;
   localparam RAM_write_4 = 9;

   reg 	      myOE = 0;
   reg 	      RAM_OE = 0;
   reg 	      RAM_WE = 0;
   reg 	      RAM_EN = 0;
   reg 	      RAM_A0_reg;
   reg [3:0]  RAM_state;
   
   always @(posedge wb_clk)
     if(wb_rst)
       begin
	  RAM_state <= RAM_idle;
	  myOE <= 0; RAM_OE <= 0; RAM_WE <= 0; RAM_EN <= 0; RAM_A0_reg <= 0;
       end
     else
       case(RAM_state)
	 RAM_idle :
	   if(read_acc)
	     begin
		RAM_state <= RAM_read_1;
		myOE <= 0; RAM_OE <= 0; RAM_WE <= 0; RAM_EN <= 1; RAM_A0_reg <= 0;
	     end
	   else if(write_acc)
	     begin
		RAM_state <= RAM_write_1;
		myOE <= 0; RAM_OE <= 0; RAM_WE <= 1; RAM_EN <= 1; RAM_A0_reg <= 0;
	     end
	   else
	     begin
		myOE <= 0; RAM_OE <= 0; RAM_WE <= 0; RAM_EN <= 0; RAM_A0_reg <= 0;
	     end
	 RAM_read_1 : 
	   begin
	      RAM_state <= RAM_read_2;
	      myOE <= 0; RAM_OE <= 0; RAM_WE <= 0; RAM_EN <= 1; RAM_A0_reg <= 1;
	   end
	 RAM_read_2 : 
	   begin
	      RAM_state <= RAM_read_3;
	      myOE <= 0; RAM_OE <= 1; RAM_WE <= 0; RAM_EN <= 0; RAM_A0_reg <= 0;
	   end
	 RAM_read_3 : 
	   begin
	      RAM_state <= RAM_read_4;
	      myOE <= 0; RAM_OE <= 1; RAM_WE <= 0; RAM_EN <= 0; RAM_A0_reg <= 0;
	   end
	 RAM_read_4 :
	   begin
	      RAM_state <= RAM_idle;
	      myOE <= 0; RAM_OE <= 0; RAM_WE <= 0; RAM_EN <= 0; RAM_A0_reg <= 0;
	   end
	 RAM_write_1 : 
	   begin
	      RAM_state <= RAM_write_2;
	      myOE <= 0; RAM_OE <= 0; RAM_WE <= 1; RAM_EN <= 1; RAM_A0_reg <= 1;
	   end
	 RAM_write_2 : 
	   begin
	      RAM_state <= RAM_write_3;
	      myOE <= 1; RAM_OE <= 0; RAM_WE <= 0; RAM_EN <= 0; RAM_A0_reg <= 0;
	   end
	 RAM_write_3 : 
	   begin
	      RAM_state <= RAM_write_4;
	      myOE <= 1; RAM_OE <= 0; RAM_WE <= 0; RAM_EN <= 0; RAM_A0_reg <= 0;
	   end
	 RAM_write_4 : 
	   begin
	      RAM_state <= RAM_idle;
	      myOE <= 0; RAM_OE <= 0; RAM_WE <= 0; RAM_EN <= 0; RAM_A0_reg <= 0;
	   end
	 default : RAM_state <= RAM_idle;
       endcase // case(RAM_state)
   
   assign     RAM_A[0] = RAM_A0_reg;
   assign     RAM_WEn = ~RAM_WE;  // ((RAM_state==RAM_write_1)||(RAM_state==RAM_write_2));
   assign     RAM_OEn = ~RAM_OE;
   assign     RAM_CE1n = ~RAM_EN;    // Active low     (RAM_state != RAM_idle);
   
   assign     RAM_D[17:16] = 2'bzz;
   assign     RAM_D[15:0] = myOE ? ((RAM_state==RAM_write_3)?ram_out[15:0]:ram_out[31:16]) 
	      : 16'bzzzz_zzzz_zzzz_zzzz;

   always @(posedge wb_clk)
     if(RAM_state == RAM_read_3)
       dat_o[15:0] <= RAM_D[15:0];
     else
       dat_o[31:16] <= RAM_D[15:0];
   
   always @(posedge wb_clk)
     if(wb_rst)
       ack_o <= 0;
     else if((RAM_state == RAM_write_4)||(RAM_state == RAM_read_4))
       ack_o <= 1;
     else
       ack_o <= 0;
   
endmodule // extram_wb
