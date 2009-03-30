
// FIFO intended to be interchangeable with shortfifo, but
//  based on block ram instead of SRL16's
//  only one clock domain

// Port A is write port, Port B is read port

module fifo_long
  #(parameter WIDTH=32, SIZE=9)
   (input clk, input reset, input clear,
    input [WIDTH-1:0] datain,
    input src_rdy_i,
    output dst_rdy_o,
    output [WIDTH-1:0] dataout,
    output src_rdy_o,
    input dst_rdy_i,
    
    output reg [15:0] space,
    output reg [15:0] occupied);
   
   wire write 	     = src_rdy_i & dst_rdy_o;
   wire read 	     = dst_rdy_i & src_rdy_o;
   wire full, empty;
   
   assign dst_rdy_o  = ~full;
   assign src_rdy_o  = ~empty;
   
   // Read side states
   localparam 	  EMPTY = 0;
   localparam 	  PRE_READ = 1;
   localparam 	  READING = 2;

   reg [SIZE-1:0] wr_addr, rd_addr;
   reg [1:0] 	  read_state;

   reg 	  empty_reg, full_reg;
   always @(posedge clk)
     if(reset)
       wr_addr <= 0;
     else if(clear)
       wr_addr <= 0;
     else if(write)
       wr_addr <= wr_addr + 1;

   ram_2port #(.DWIDTH(WIDTH),.AWIDTH(SIZE))
     ram (.clka(clk),
	  .ena(1'b1),
	  .wea(write),
	  .addra(wr_addr),
	  .dia(datain),
	  .doa(),

	  .clkb(clk),
	  .enb((read_state==PRE_READ)|read),
	  .web(0),
	  .addrb(rd_addr),
	  .dib(0),
	  .dob(dataout));

   always @(posedge clk)
     if(reset)
       begin
	  read_state <= EMPTY;
	  rd_addr <= 0;
	  empty_reg <= 1;
       end
     else
       if(clear)
	 begin
	    read_state <= EMPTY;
	    rd_addr <= 0;
	    empty_reg <= 1;
	 end
       else 
	 case(read_state)
	   EMPTY :
	     if(write)
	       begin
		  //rd_addr <= wr_addr;
		  read_state <= PRE_READ;
	       end
	   PRE_READ :
	     begin
		read_state <= READING;
		empty_reg <= 0;
		rd_addr <= rd_addr + 1;
	     end
	   
	   READING :
	     if(read)
	       if(rd_addr == wr_addr)
		 begin
		    empty_reg <= 1;
		    if(write)
		      read_state <= PRE_READ;
		    else
		      read_state <= EMPTY;
		 end
	       else
		 rd_addr <= rd_addr + 1;
	 endcase // case(read_state)

   wire [SIZE-1:0] dont_write_past_me = rd_addr - 3;
   wire 	   becoming_full = wr_addr == dont_write_past_me;
     
   always @(posedge clk)
     if(reset)
       full_reg <= 0;
     else if(clear)
       full_reg <= 0;
     else if(read & ~write)
       full_reg <= 0;
     //else if(write & ~read & (wr_addr == (rd_addr-3)))
     else if(write & ~read & becoming_full)
       full_reg <= 1;

   //assign empty = (read_state != READING);
   assign empty = empty_reg;

   // assign full = ((rd_addr - 1) == wr_addr);
   assign full = full_reg;

   //////////////////////////////////////////////
   // space and occupied are for diagnostics only
   // not guaranteed exact

   localparam NUMLINES = (1<<SIZE)-2;
   always @(posedge clk)
     if(reset)
       space <= NUMLINES;
     else if(clear)
       space <= NUMLINES;
     else if(read & ~write)
       space <= space + 1;
     else if(write & ~read)
       space <= space - 1;
   
   always @(posedge clk)
     if(reset)
       occupied <= 0;
     else if(clear)
       occupied <= 0;
     else if(read & ~write)
       occupied <= occupied - 1;
     else if(write & ~read)
       occupied <= occupied + 1;
   
endmodule // fifo_long
