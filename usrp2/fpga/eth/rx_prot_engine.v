
module rx_prot_engine
  #(parameter FIFO_SIZE=11)
    (input clk, input rst,
     
     input Rx_mac_ra,
     output Rx_mac_rd,
     input [31:0] Rx_mac_data,
     input [1:0] Rx_mac_BE,
     input Rx_mac_pa,
     input Rx_mac_sop,
     input Rx_mac_eop,
     input Rx_mac_err,
     
     output [31:0] wr_dat_o,
     output wr_write_o,
     output wr_done_o,
     output wr_error_o,
     input wr_ready_i,
     input wr_full_i,
     output wr_flag_o,

     input set_stb,
     input [7:0] set_addr,
     input [31:0] set_data,
     
     output [15:0] rx_fifo_status,
     output reg [7:0] rx_seqnum,
     output reg [7:0] rx_channel,
     output [7:0] rx_flags
     );

   wire 	  read, write, full, empty;
   wire 	  eop_i, err_i, eop_o, err_o, flag_i, sop_i, flag_o, sop_o;
   wire [31:0] 	  dat_i, dat_o;
   reg 		  xfer_active;

   wire [3:0] 	  hdr_adr;
   wire [31:0] 	  hdr_dat;
   
   header_ram #(.REGNUM(48),.WIDTH(32)) rx_header_ram
     (.clk(clk),.set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .addr(hdr_adr),.q(hdr_dat));

   // Buffer interface side
   always @(posedge clk)
     if(rst)
       xfer_active <= 0;
     else if(wr_ready_i & ~empty)
       xfer_active <= 1;
     else if(eop_o | err_o | wr_full_i)
       xfer_active <= 0;

   assign wr_done_o = eop_o & wr_write_o;
   assign wr_error_o = err_o & wr_write_o;
   assign wr_dat_o = dat_o;
   assign wr_write_o = xfer_active & ~empty;
   assign read = wr_write_o;

   // FIFO in the middle
   cascadefifo2 #(.WIDTH(36),.SIZE(11)) rx_prot_fifo
     (.clk(clk),.rst(rst),
      .datain({flag_i,sop_i,eop_i,err_i,dat_i}),.write(write),.full(full),
      .dataout({flag_o,sop_o,eop_o,err_o,dat_o}),.read(read),.empty(empty),
      .clear(0),.fifo_space(rx_fifo_status));

   // MAC side
   localparam ETH_TYPE = 16'hBEEF;
   
   reg [2:0] prot_state;
   localparam PROT_IDLE = 0;
   localparam PROT_HDR1 = 1;
   localparam PROT_HDR2 = 2;
   localparam PROT_HDR3 = 3;
   localparam PROT_HDR4 = 4;
   localparam PROT_HDR5 = 5;
   localparam PROT_PKT = 6;

   // Things to control: flag_i, sop_i, eop_i, err_i, dat_i, write, Rx_mac_rd
   // Inputs to SM: Rx_mac_sop, Rx_mac_eop, Rx_mac_ra, Rx_mac_pa, 
   //                Rx_mac_BE, Rx_mac_err, full

   reg 	      flag;
   assign     dat_i = Rx_mac_data;
   assign     sop_i = Rx_mac_sop;
   assign     eop_i = Rx_mac_eop;
   assign     err_i = Rx_mac_err;
   assign     flag_i = flag;
   assign     wr_flag_o = flag_o;
   assign     Rx_mac_rd = (prot_state != PROT_IDLE) && (~full|~Rx_mac_pa);
   assign     write = (prot_state != PROT_IDLE) && ~full && Rx_mac_pa;	      

   assign     hdr_adr = {1'b0,prot_state[2:0]};

   wire [7:0] rx_seqnum_p1 = rx_seqnum + 1;
   
   always @(posedge clk)
     if(rst)
       begin
	  prot_state <= PROT_IDLE;
	  flag <= 0;
       end
     else if(prot_state == PROT_IDLE)
       begin
	  flag <= 0;
	  if(Rx_mac_ra)
	    prot_state <= PROT_HDR1;
       end
     else if(write)
       case(prot_state)
	 PROT_HDR1 : 
	   begin
	      prot_state <= PROT_HDR2;
	      if(hdr_dat != Rx_mac_data)
		flag <= 1;
	   end
	 PROT_HDR2 :
	   begin
	      prot_state <= PROT_HDR3;
	      if(hdr_dat != Rx_mac_data)
		flag <= 1;
	   end
	 PROT_HDR3 :
	   begin
	      prot_state <= PROT_HDR4;
	      if(hdr_dat != Rx_mac_data)
		flag <= 1;
	   end
	 PROT_HDR4 : 
	   begin
	      prot_state <= PROT_HDR5;
	      if(hdr_dat[31:16] != Rx_mac_data[31:16])
		flag <= 1;
	      rx_channel <= hdr_dat[15:8];
	   end
	 PROT_HDR5 :
	   begin
	      prot_state <= PROT_PKT;
	      if((rx_seqnum_p1) != Rx_mac_data[15:8])
		flag <= 1;
	   end
	 PROT_PKT : 
	   if(Rx_mac_eop | Rx_mac_err)
	     prot_state <= PROT_IDLE;
       endcase // case(prot_state)
   
   always @(posedge clk)
     if(rst)
       rx_seqnum <= 8'hFF;
     else if(set_stb & (set_addr == 54))
       rx_seqnum <= set_data[7:0];
     else if(write & (prot_state == PROT_HDR5) & ((rx_seqnum_p1) == Rx_mac_data[15:8]) & ~flag)
       rx_seqnum <= rx_seqnum + 1;
   
   // Error cases -- Rx_mac_error, BE != 0
endmodule // rx_prot_engine
