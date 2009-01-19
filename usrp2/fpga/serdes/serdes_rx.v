
// SERDES Interface

// LS-Byte is sent first, MS-Byte is second
// Invalid K Codes
//  K0.0  000-00000  Error detected
//  K31.7 111-11111  Loss of input signal

// Valid K Codes
//  K28.0 000-11100
//  K28.1 001-11100  Alternate COMMA?
//  K28.2 010-11100
//  K28.3 011-11100
//  K28.4 100-11100
//  K28.5 101-11100  Standard COMMA?
//  K28.6 110-11100
//  K28.7 111-11100  Bad COMMA?
//  K23.7 111-10111
//  K27.7 111-11011
//  K29.7 111-11101
//  K30.7 111-11110

module serdes_rx
  #(parameter FIFOSIZE = 9)
    (input clk,
     input rst,
     
     // RX HW Interface
     input ser_rx_clk,
     input [15:0] ser_r,
     input ser_rklsb,
     input ser_rkmsb,
     
     output [31:0] wr_dat_o,
     output wr_write_o,
     output wr_done_o,
     output wr_error_o,
     input wr_ready_i,
     input wr_full_i,

     output [15:0] fifo_space,
     output xon_rcvd, output xoff_rcvd,

     output [15:0] fifo_occupied, output fifo_full, output fifo_empty,
     output reg serdes_link_up,
     output [31:0] debug
     );

   localparam K_COMMA = 8'b101_11100;     // 0xBC K28.5
   localparam K_IDLE = 8'b001_11100;      // 0x3C K28.1
   localparam K_PKT_START = 8'b110_11100; // 0xDC K28.6
   localparam K_PKT_END = 8'b100_11100;   // 0x9C K28.4
   localparam K_XON = 8'b010_11100;       // 0x5C K28.2
   localparam K_XOFF = 8'b011_11100;      // 0x7C K28.3
   localparam K_LOS = 8'b111_11111;       // 0xFF K31.7
   localparam K_ERROR = 8'b000_00000;     // 0x00 K00.0
   localparam D_56 = 8'b110_00101;        // 0xC5 D05.6
   
   localparam IDLE = 3'd0;
   localparam FIRSTLINE1 = 3'd1;
   localparam FIRSTLINE2 = 3'd2;
   localparam PKT1 = 3'd3;
   localparam PKT2 = 3'd4;
   localparam CRC_CHECK = 3'd5;
   localparam ERROR = 3'd6;
   localparam DONE = 3'd7;
   
   wire [17:0] even_data;
   reg [17:0]  odd_data;
   wire [17:0] chosen_data;
   reg 	       odd;
   
   reg [31:0]  line_i;
   reg 	       sop_i, eop_i, error_i;
   wire        error_o, sop_o, eop_o, write, read, empty, full;
   reg [15:0]  halfline;
   reg [8:0]   holder;
   wire [31:0] line_o;
   
   reg [2:0]   state;

   reg [15:0]  CRC;
   wire [15:0] nextCRC;
   reg 	       write_d;

   oneshot_2clk rst_1s(.clk_in(clk),.in(rst),.clk_out(ser_rx_clk),.out(rst_rxclk));

   /*
   ss_rcvr #(.WIDTH(18)) ss_rcvr
     (.rxclk(ser_rx_clk),.sysclk(clk),.rst(rst),
      .data_in({ser_rkmsb,ser_rklsb,ser_r}),.data_out(even_data),
      .clock_present());
   */
   assign      even_data = {ser_rkmsb,ser_rklsb,ser_r};
   
   always @(posedge ser_rx_clk)
     if(rst_rxclk)
       holder <= 9'd0;
     else
       holder <= {even_data[17],even_data[15:8]};
   
   always @(posedge ser_rx_clk)
     if(rst_rxclk)
       odd_data <= 18'd0;
     else
       odd_data <= {even_data[16],holder[8],even_data[7:0],holder[7:0]};
   
   assign      chosen_data = odd ? odd_data : even_data;

   // Transfer xon and xoff info to the main system clock for flow control purposes
   reg 	       xon_rcvd_rxclk, xoff_rcvd_rxclk;
   always @(posedge ser_rx_clk)
     xon_rcvd_rxclk = ({1'b1,K_XON} == {ser_rkmsb,ser_r[15:8]}) | ({1'b1,K_XON} == {ser_rklsb,ser_r[7:0]} );
   always @(posedge ser_rx_clk)
     xoff_rcvd_rxclk = ({1'b1,K_XOFF} == {ser_rkmsb,ser_r[15:8]}) | ({1'b1,K_XOFF} == {ser_rklsb,ser_r[7:0]} );
   
   oneshot_2clk xon_1s(.clk_in(ser_rx_clk),.in(xon_rcvd_rxclk),.clk_out(clk),.out(xon_rcvd));
   oneshot_2clk xoff_1s(.clk_in(ser_rx_clk),.in(xoff_rcvd_rxclk),.clk_out(clk),.out(xoff_rcvd));

   // If the other side is sending xon or xoff, or is flow controlled (b/c we told them to be), don't fill the fifos
   wire        wait_here = ((chosen_data == {2'b10,K_COMMA,D_56})||
			    (chosen_data == {2'b11,K_XON,K_XON})||
			    (chosen_data == {2'b11,K_XOFF,K_XOFF}) );

   always @(posedge ser_rx_clk)
     if(rst_rxclk) sop_i <= 0;
     else if(state == FIRSTLINE1) sop_i <= 1;
     else if(write_d) sop_i <= 0;
   
   reg 	       write_pre;
   always @(posedge ser_rx_clk)
     if(rst_rxclk)
       begin
	  state <= IDLE;
	  odd <= 0;
	  halfline <= 0;
	  line_i <= 0;
	  eop_i <= 0;
	  error_i <= 0;
	  write_pre <= 0;
       end
     else
       case(state)
	 IDLE :
	   begin
	      error_i <= 0;
	      write_pre <= 0;
	      if(even_data == {2'b11,K_PKT_START,K_PKT_START})
		begin
		   state <= FIRSTLINE1;
		   odd <= 0;
		end
	      else if(odd_data == {2'b11,K_PKT_START,K_PKT_START})
		begin
		   state <= FIRSTLINE1;
		   odd <= 1;
		end
	   end

	 FIRSTLINE1 :
	   if(chosen_data[17:16] == 0)
	     begin
		halfline <= chosen_data[15:0];
		state <= FIRSTLINE2;
	     end
	   else if(wait_here)
	     ;  // Flow Controlled, so wait here and do nothing
	   else
	     state <= ERROR;

	 FIRSTLINE2 :
	   if(chosen_data[17:16] == 0)
	     begin
		line_i <= {chosen_data[15:0],halfline};
		if(full)  // No space to write to!  Should have been avoided by flow control
		  state <= ERROR;
		else
		  begin
		     state <= PKT1;
		     write_pre <= 1;
		  end
	     end // if (chosen_data[17:16] == 0)
	   else if(wait_here)
	     ;  // Flow Controlled, so wait here and do nothing
	   else
	     state <= ERROR;
	 
	 PKT1 :
	   begin
	      write_pre <= 0;
	      if(chosen_data[17:16] == 0)
		begin
		   halfline <= chosen_data[15:0];
		   state <= PKT2;
		end
	      else if(wait_here)
		;  // Flow Controlled
	      else if(chosen_data == {2'b11,K_PKT_END,K_PKT_END})
	  	state <= CRC_CHECK;
	      else
		state <= ERROR;
	   end // case: PKT1
	 
	 PKT2 :
	   if(chosen_data[17:16] == 0)
	     begin
		line_i <= {1'b0,1'b0,1'b0,chosen_data[15:0],halfline};
		if(full)  // No space to write to!
		  state <= ERROR;
		else
		  begin
		     state <= PKT1;
		     write_pre <= 1;
		  end
	     end // if (chosen_data[17:16] == 0)
	   else if(wait_here)
	     ;  // Flow Controlled
	   else
	     state <= ERROR;
	   
	 CRC_CHECK :
	   if(chosen_data[17:0] == {2'b00,CRC})
	     begin
		if(full)
		  state <= ERROR;
		else
		  begin
		     eop_i <= 1;
		     state <= DONE;
		  end
	     end
	   else if(wait_here)
	     ;
	   else
	     state <= ERROR;
	 
	 ERROR :
	   begin
	      error_i <= 1;
	      if(~full)
		state <= IDLE;
	   end
	 DONE :
	   begin
	      state <= IDLE;
	      eop_i <= 0;
	   end
	      
       endcase // case(state)
   
   
   always @(posedge ser_rx_clk)
     if(rst_rxclk)
       CRC <= 16'hFFFF;
     else if(state == IDLE)
       CRC <= 16'hFFFF;
     else if(chosen_data[17:16] == 2'b00)
       CRC <= nextCRC;

   CRC16_D16 crc_blk(chosen_data[15:0],CRC,nextCRC);

   always @(posedge ser_rx_clk)
     if(rst_rxclk) write_d <= 0;
     else write_d <= write_pre;

   // Internal FIFO, size 9 is 2K, size 10 is 4K Bytes
   assign write = eop_i | (error_i & ~full) | (write_d & (state != CRC_CHECK));


//`define CASC 1
`define MYFIFO 1   
//`define XILFIFO 1

`ifdef CASC   
   cascadefifo2 #(.WIDTH(35),.SIZE(FIFOSIZE)) serdes_rx_fifo
     (.clk(clk),.rst(rst),.clear(0),
      .datain({error_i,sop_i,eop_i,line_i}), .write(write), .full(full),
      .dataout({error_o,sop_o,eop_o,line_o}), .read(read), .empty(empty),
      .space(fifo_space),.occupied(fifo_occupied) );
   assign fifo_full = full;
   assign fifo_empty = empty;
`endif

`ifdef MYFIFO
   wire [FIFOSIZE-1:0] level;
    fifo_2clock_casc #(.DWIDTH(35),.AWIDTH(FIFOSIZE)) serdes_rx_fifo
     (.arst(rst),
      .wclk(ser_rx_clk),.datain({error_i,sop_i,eop_i,line_i}), .write(write), .full(full),
      .rclk(clk),.dataout({error_o,sop_o,eop_o,line_o}), .read(read), .empty(empty),
      .level_rclk(level) );
   assign 	       fifo_space = {{(16-FIFOSIZE){1'b0}},{FIFOSIZE{1'b1}}} - 
		       {{(16-FIFOSIZE){1'b0}},level};
   assign 	       fifo_occupied = { {(16-FIFOSIZE){1'b0}} ,level};
   assign 	       fifo_full = full;   // Note -- fifo_full is in the wrong clock domain
   assign 	       fifo_empty = empty;
`endif

`ifdef XILFIFO
   wire [FIFOSIZE-1:0] level;
   fifo_generator_v4_1 ser_rx_fifo
     (.din({error_i,sop_i,eop_i,line_i}),
      .rd_clk(clk),
      .rd_en(read),
      .rst(rst),
      .wr_clk(ser_rx_clk),
      .wr_en(write),
      .dout({error_o,sop_o,eop_o,line_o}),
      .empty(empty),
      .full(full),
      .rd_data_count(level),
      .wr_data_count() );
   assign 	       fifo_space = {{(16-FIFOSIZE){1'b0}},{FIFOSIZE{1'b1}}} - 
		       {{(16-FIFOSIZE){1'b0}},level};
   assign 	       fifo_occupied = { {(16-FIFOSIZE){1'b0}}, level };
   assign 	       fifo_full = full;   // Note -- fifo_full is in the wrong clock domain
   assign 	       fifo_empty = empty;
`endif //  `ifdef XILFIFO
   
   
   // Internal FIFO to Buffer interface
   reg 	       xfer_active;

   always @(posedge clk)
     if(rst)
       xfer_active <= 0;
     else if(xfer_active & ~empty & (eop_o | wr_full_i | error_o))
       xfer_active <= 0;
     else if(wr_ready_i & sop_o)
       xfer_active <= 1;

   assign      read = (xfer_active | ~sop_o) & ~empty;

   assign      wr_write_o = xfer_active & ~empty;
   assign      wr_done_o = eop_o & ~empty & xfer_active;
   //assign      wr_error_o = xfer_active & ((wr_full_i & ~eop_o & ~empty)|error_o);
   assign      wr_error_o = xfer_active & ~empty & error_o;

   assign      wr_dat_o = line_o;

   wire        slu = ~({2'b11,K_ERROR,K_ERROR}=={ser_rkmsb,ser_rklsb,ser_r});
   reg [3:0]   slu_reg;
   
   always @(posedge clk)
     if(rst) slu_reg <= 0;
     else slu_reg <= {slu_reg[2:0],slu};

   always @(posedge clk)
     serdes_link_up <= &slu_reg[3:1];
   
   assign      debug = { full, empty, odd, xfer_active, sop_i, eop_i, error_i, state[2:0] };
   
endmodule // serdes_rx
