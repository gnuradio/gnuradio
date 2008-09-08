
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

module serdes_tx
  #(parameter FIFOSIZE = 9)
    (input clk,
     input rst,
     
     // TX HW Interface
     output ser_tx_clk,
     output reg [15:0] ser_t,
     output reg ser_tklsb,
     output reg ser_tkmsb,
     
     // TX Stream Interface
     input [31:0] rd_dat_i,
     output rd_read_o,
     output rd_done_o,
     output rd_error_o,
     input rd_sop_i,
     input rd_eop_i,

     // Flow control interface
     input inhibit_tx,
     input send_xon,
     input send_xoff,
     output sent,

     // FIFO Levels
     output [15:0] fifo_occupied,
     output fifo_full,
     output fifo_empty,

     // DEBUG
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
   assign     ser_tx_clk = clk;
   
   localparam IDLE = 3'd0;
   localparam RUN1 = 3'd1;
   localparam RUN2 = 3'd2;
   localparam DONE = 3'd3;
   localparam SENDCRC = 3'd4;
   localparam WAIT = 3'd5;
   
   reg [2:0]  state;
   
   reg [15:0] CRC;
   wire [15:0] nextCRC;
   reg [3:0]   wait_count;
   
   // Internal FIFO, size 9 is 2K, size 10 is 4K bytes
   wire        sop_o, eop_o, write, full, read, empty;
   wire [31:0] data_o;
   reg 	       xfer_active;
   
   cascadefifo2 #(.WIDTH(34),.SIZE(FIFOSIZE)) serdes_tx_fifo
     (.clk(clk),.rst(rst),.clear(0),
      .datain({rd_sop_i,rd_eop_i,rd_dat_i}), .write(write), .full(full),
      .dataout({sop_o,eop_o,data_o}), .read(read), .empty(empty),
      .space(), .occupied(fifo_occupied) );
   assign      fifo_full = full;
   assign      fifo_empty = empty;
   
   // Buffer interface to internal FIFO
   always @(posedge clk)
     if(rst)
       xfer_active <= 0;
     else if(rd_eop_i & ~full)  // In case we can't store last line right away
       xfer_active <= 0;
     else if(rd_sop_i)
       xfer_active <= 1;
   
   assign      write = xfer_active & ~full;
   
   assign      rd_read_o = write;
   assign      rd_done_o = 0;        // Always take everything we're given
   assign      rd_error_o = 0;       // No chance for errors anticipated
   
   
   // FIXME Implement flow control
   
   reg [15:0]  second_word;
   reg [33:0]  pipeline;
   
   assign      read = (~send_xon & ~send_xoff & (state==RUN2)) | ((state==IDLE) & ~empty & ~sop_o);
   assign      sent = send_xon | send_xoff;
   // 2nd half of above probably not necessary.  Just in case we get junk between packets
   
   always @(posedge clk)
     if(rst)
       begin
	  state <= IDLE;
	  wait_count <= 0;
	  {ser_tkmsb,ser_tklsb,ser_t} <= 18'd0;
			//{2'b10,K_COMMA,K_COMMA};  
	  // make tkmsb and tklsb different so they can go in IOFFs
       end
     else
       if(send_xon)
	 {ser_tkmsb,ser_tklsb,ser_t} <= {2'b11,K_XON,K_XON};
       else if(send_xoff)
	 {ser_tkmsb,ser_tklsb,ser_t} <= {2'b11,K_XOFF,K_XOFF};
       else
	 case(state)
	   IDLE :
	     begin
		if(sop_o & ~empty & ~inhibit_tx)
		  begin
		     {ser_tkmsb,ser_tklsb,ser_t} <= {2'b11,K_PKT_START,K_PKT_START};
		     state <= RUN1;
		  end
		else
		  {ser_tkmsb,ser_tklsb,ser_t} <= {2'b10,K_COMMA,D_56};
	     end
	   RUN1 :
	     begin
		if(empty | inhibit_tx)
		  {ser_tkmsb,ser_tklsb,ser_t} <= {2'b10,K_COMMA,D_56};
		else
		  begin
		     {ser_tkmsb,ser_tklsb,ser_t} <= {2'b00,data_o[15:0]};
		     state <= RUN2;
		  end
	     end
	   RUN2 :
	     begin
		{ser_tkmsb,ser_tklsb,ser_t} <= {2'b00,data_o[31:16]};
		if(eop_o)
		  state <= DONE;
		else
		  state <= RUN1;
	     end
	   DONE :
	     begin
		{ser_tkmsb,ser_tklsb,ser_t} <= {2'b11,K_PKT_END,K_PKT_END};
		state <= SENDCRC;
	     end
	   SENDCRC :
	     begin
		{ser_tkmsb,ser_tklsb,ser_t} <= {2'b00,CRC};
		state <= WAIT;
		wait_count <= 4'd15;
	     end
	   WAIT :
	     begin
		{ser_tkmsb,ser_tklsb,ser_t} <= {2'b10,K_COMMA,D_56};
		if(wait_count == 0)
		  state <= IDLE;
		else
		  wait_count <= wait_count - 1;
	     end
	   default
	     state <= IDLE;
	 endcase // case(state)
   
   always @(posedge clk)
     if(rst)
       CRC <= 16'hFFFF;
     else if(state == IDLE)
       CRC <= 16'hFFFF;
     else if( (~empty & ~inhibit_tx & (state==RUN1)) || (state==RUN2) )
       CRC <= nextCRC;
   
   CRC16_D16 crc_blk( (state==RUN1) ? data_o[15:0] : data_o[31:16], CRC, nextCRC);

   assign debug = { 26'd0, full, empty, xfer_active, state[2:0] };
   
endmodule // serdes_tx

