
// Wishbone module for spi communications with an SD Card
// The programming interface is simple -- 
//      Write the desired clock divider to address 1 (should be 1 or higher)
//      Status is in address 0.  A 1 indicates the last transaction is done and it is safe to
//          send another
//      Writing a byte to address 2 sends that byte over SPI.  When it is done, 
//          status (addr 0) goes high again, and the received byte can be read from address 3.

module sd_spi_wb
  (input clk,
   input rst,
   
   // SD Card interface
   output sd_clk,
   output sd_csn,
   output sd_mosi,
   input sd_miso,

   input wb_cyc_i,
   input wb_stb_i,
   input wb_we_i,
   input [1:0] wb_adr_i,
   input [7:0] wb_dat_i,
   output reg [7:0] wb_dat_o,
   output reg wb_ack_o);

   localparam ADDR_STATUS = 0;
   localparam ADDR_CLKDIV = 1;
   localparam ADDR_WRITE = 2;
   localparam ADDR_READ = 3;

   wire [7:0] status, rcv_dat;
   reg [7:0]  clkdiv;
   wire       ready;
   reg 	      ack_d1;

   reg 	      cs_reg;
   assign     sd_csn = ~cs_reg;  // FIXME
   
   always @(posedge clk)
     if(rst) ack_d1 <= 0;
     else ack_d1 <= wb_ack_o;
   
   always @(posedge clk)
     if(rst) wb_ack_o <= 0;
     else wb_ack_o <= wb_cyc_i & wb_stb_i & ~ack_d1;
   
   always @(posedge clk)
     case(wb_adr_i)
       ADDR_STATUS : wb_dat_o <= {7'd0,ready};
       ADDR_CLKDIV : wb_dat_o <= clkdiv;
       ADDR_READ : wb_dat_o <= rcv_dat;
       default : wb_dat_o <= 0;
     endcase // case(wb_adr_i)

   always @(posedge clk)
     if(rst)
       begin
	  clkdiv <= 200;
	  cs_reg <= 0;
       end
     else if(wb_we_i & wb_stb_i & wb_cyc_i & wb_ack_o)
       case(wb_adr_i)
	 ADDR_STATUS : cs_reg <= wb_dat_i;
	 ADDR_CLKDIV : clkdiv <= wb_dat_i;
       endcase // case(wb_adr_i)

   wire       go = wb_we_i & wb_stb_i & wb_cyc_i & wb_ack_o & (wb_adr_i == ADDR_WRITE);
   
   sd_spi sd_spi(.clk(clk),.rst(rst),
		 .sd_clk(sd_clk),.sd_mosi(sd_mosi),.sd_miso(sd_miso),
		 .clk_div(clkdiv),.send_dat(wb_dat_i),.rcv_dat(rcv_dat),
		 .go(go),.ready(ready) );

endmodule // sd_spi_wb
