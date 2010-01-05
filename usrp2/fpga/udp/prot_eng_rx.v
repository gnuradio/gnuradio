


// Protocol Engine Receiver
//  Checks each line (16 bits) against values in setting regs
//  3 options for each line -- 
//      Error if mismatch, Slowpath if mismatch, or ignore line
//  The engine increases the length of each packet by 32 or 48 bits,
//   bringing the total length to a multiple of 32 bits.  The last line
//   is entirely new, and contains the results of the matching operation:
//      16 bits of flags, 16 bits of data.  Flags indicate error or slowpath
//      Data indicates line that caused mismatch if any.


//   Flags[2:0] is {occ, eop, sop}
//   Protocol word format is:
//             22   Last Header Line
//             21   SLOWPATH if mismatch
//             20   ERROR if mismatch
//             19   This is the IP checksum
//             18   This is the UDP checksum
//             17   Compute IP checksum on this word
//             16   Compute UDP checksum on this word
//           15:0   data word to be matched

module prot_eng_rx
  #(parameter BASE=0)
   (input clk, input reset, input clear,
    input set_stb, input [7:0] set_addr, input [31:0] set_data,
    input [18:0] datain, input src_rdy_i, output dst_rdy_o,
    output [18:0] dataout, output src_rdy_o, input dst_rdy_i);

   localparam HDR_WIDTH  = 16 + 7;  // 16 bits plus flags
   localparam HDR_LEN 	 = 32;      // Up to 64 bytes of protocol
   
   // Store header values in a small dual-port (distributed) ram
   reg [HDR_WIDTH-1:0] header_ram[0:HDR_LEN-1];
   wire [HDR_WIDTH-1:0] header_word;
   
   always @(posedge clk)
     if(set_stb & ((set_addr & 8'hE0) == BASE))
       header_ram[set_addr[4:0]] <= set_data;

   assign header_word 	= header_ram[state];

   wire consume_input 	= src_rdy_i & dst_rdy_o;
   wire produce_output 	= src_rdy_o & dst_rdy_i;
   
   // Main State Machine
   reg [15:0] pkt_length, fail_word, dataout_int;
   
   reg slowpath, error, sof_o, eof_o, occ_o, odd;

   assign dataout    = {occ_o, eof_o, sof_o, dataout_int};

   wire [15:0] calc_ip_checksum, calc_udp_checksum;
   reg [15:0] rx_ip_checksum, rx_udp_checksum;

   always @(posedge clk) 
     if(header_word[19]) 
       rx_ip_checksum  <= datain[15:0];
   always @(posedge clk) 
     if(header_word[18]) 
       rx_udp_checksum <= datain[15:0];
   
   always @(posedge clk)
     if(reset | clear)
       begin
	  slowpath     <= 0;
	  error        <= 0;
	  state        <= 0;
	  fail_word    <= 0;
	  eof_o        <= 0;
	  occ_o        <= 0;
       end
     else if(src_rdy_i & dst_rdy_i)
       case (state)
	 0 :
	   begin
	      slowpath 	   <= 0;
	      error 	   <= 0;
	      eof_o 	   <= 0;
	      occ_o 	   <= 0;
	      state 	   <= 1;
	   end

	 ST_SLOWPATH :
	   ;
	 ST_ERROR :
	   ;
	 ST_PAYLOAD :
	   ;
	 ST_FILLER :
	   ;
	 ST_END1 :
	   ;
	 ST_END2 :
	   ;
	 default :
	   if(header_word[21] && mismatch)
	     state <= ST_SLOWPATH;
	   else if(header_word[20] && mismatch)
	     state <= ST_ERROR;
	   else if(header_word[22])
	     state <= ST_PAYLOAD;
	   else
	     state <= state + 1;
       endcase // case (state)
   


   // IP + UDP checksum state machines
   checksum_sm ip_chk
     (.clk(clk), .reset(reset), .in(datain), 
      .calc(consume_input & header_word[17]), .clear(state==0), .checksum(ip_checksum));
   
   checksum_sm udp_chk
     (.clk(clk), .reset(reset), .in(datain), 
      .calc(consume_input & header_word[16]), .clear(state==0), .checksum(udp_checksum));
   
endmodule // prot_eng_rx
