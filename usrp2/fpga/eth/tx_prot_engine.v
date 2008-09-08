
module tx_prot_engine
  (input clk, input rst,
   
   // To MAC
   input Tx_mac_wa,
   output Tx_mac_wr,
   output [31:0] Tx_mac_data,
   output [1:0] Tx_mac_BE,
   output Tx_mac_sop,
   output Tx_mac_eop,

   // To buffer interface
   input [31:0] rd_dat_i,
   output rd_read_o,
   output rd_done_o,
   output rd_error_o,
   input rd_sop_i,
   input rd_eop_i,

   // To control
   input set_stb,
   input [7:0] set_addr,
   input [31:0] set_data,

   // Protocol Stuff
   input [15:0] rx_fifo_status,
   input [7:0] rx_seqnum
   //input [7:0] tx_channel,
   //input [7:0] tx_flags
   );

   wire [3:0] 	hdr_adr;
   wire [31:0] 	hdr_dat;
   wire [7:0] 	tx_channel;
   
   header_ram #(.REGNUM(32),.WIDTH(32)) tx_header_ram
     (.clk(clk),.set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .addr(hdr_adr),.q(hdr_dat));

   setting_reg #(.my_addr(32)) sr_channel
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),.in(set_data),
      .out(tx_channel),.changed());
   
   // Might as well use a shortfifo here since they are basically free
   wire  empty, full, sfifo_write, sfifo_read;
   wire [33:0] sfifo_in, sfifo_out;
   
   shortfifo #(.WIDTH(34)) txmac_sfifo
     (.clk(clk),.rst(rst),.clear(0),
      .datain(sfifo_in),.write(sfifo_write),.full(full),
      .dataout(sfifo_out),.read(sfifo_read),.empty(empty));

   // MAC side signals
   // Inputs -- Tx_mac_wa, sfifo_out, empty
   // outputs -- sfifo_read, Tx_mac_data, Tx_mac_wr, Tx_mac_BE, Tx_mac_sop, Tx_mac_eop
   
   //  We are allowed to do one more write after we are told the FIFO is full
   //  This allows us to register the _wa signal and speed up timing.
   reg 	       tx_mac_wa_d1;
   always @(posedge clk)
     tx_mac_wa_d1 <= Tx_mac_wa;

   reg [2:0]   prot_state;
   localparam  PROT_IDLE = 0;
   localparam  PROT_HDR1 = 1;
   localparam  PROT_HDR2 = 2;
   localparam  PROT_HDR3 = 3;
   localparam  PROT_HDR4 = 4;
   localparam  PROT_HDR5 = 5;
   localparam  PROT_PKT  = 6;

   reg [7:0]   tx_seqnum;
   reg 	       all_match;
   always @(posedge clk)
     if(rst) 
       tx_seqnum <= 0;
     else if(set_stb & (set_addr == 36))
       tx_seqnum <= set_data[7:0];
     else if(tx_mac_wa_d1 & all_match & (prot_state == PROT_HDR5))
       tx_seqnum <= tx_seqnum + 1;
   
   always @(posedge clk)
     if(rst)
       prot_state <= PROT_IDLE;
     else
       if(tx_mac_wa_d1 & ~empty)
	 case(prot_state)
	   PROT_IDLE :
	     prot_state <= PROT_HDR1;
	   PROT_HDR1 :
	     prot_state <= PROT_HDR2;
	   PROT_HDR2 :
	     prot_state <= PROT_HDR3;
	   PROT_HDR3 :
	     prot_state <= PROT_HDR4;
	   PROT_HDR4 :
	     prot_state <= PROT_HDR5;
	   PROT_HDR5 :
	     prot_state <= PROT_PKT;
	   PROT_PKT :
	     if(sfifo_out[32] & ~empty)
	       prot_state <= PROT_IDLE;
	   default :
	     prot_state <= PROT_IDLE;
	 endcase // case(prot_state)

   assign      hdr_adr = {1'b0,prot_state};
   wire        match = (hdr_dat == sfifo_out[31:0]);   
   always @(posedge clk)
     if(prot_state == PROT_IDLE)
       all_match <= 1;
     else if(tx_mac_wa_d1 & ~empty &
	     ((prot_state==PROT_HDR1)|(prot_state==PROT_HDR2)|(prot_state==PROT_HDR3)))
       all_match <= all_match & match;
   
   localparam  ETH_TYPE = 16'hBEEF;
   assign      Tx_mac_data = 
	       ((prot_state == PROT_HDR5) & all_match) ? {rx_fifo_status,tx_seqnum,rx_seqnum} :
	       sfifo_out[31:0];
   assign      sfifo_read = (prot_state != PROT_IDLE) & ~empty & tx_mac_wa_d1;
   assign      Tx_mac_wr = sfifo_read;
   assign      Tx_mac_BE = 0;  // Since we only deal with packets that are multiples of 32 bits long
   assign      Tx_mac_sop = sfifo_out[33];
   assign      Tx_mac_eop = sfifo_out[32];

   // BUFFER side signals
   reg 	       xfer_active;
   always @(posedge clk)
     if(rst)
       xfer_active <= 0;
     else if(rd_eop_i & ~full)
       xfer_active <= 0;
     else if(rd_sop_i)
       xfer_active <= 1;
   
   assign      sfifo_in = {rd_sop_i, rd_eop_i, rd_dat_i};
   assign      sfifo_write = xfer_active & ~full;

   assign      rd_read_o = sfifo_write;
   assign      rd_done_o = 0;  // Always send everything we're given?
   assign      rd_error_o = 0;  // No possible error situations?
   
endmodule // tx_prot_engine
