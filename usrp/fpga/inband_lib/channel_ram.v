module channel_ram 
   ( // System
     input txclk, input reset,
     // USB side
     input [31:0] datain, input WR, input WR_done, output have_space,
     // Reader side 
     output [31:0] dataout, input RD, input RD_done, output packet_waiting);
	
   reg [6:0] wr_addr, rd_addr;
   reg [1:0] which_ram_wr, which_ram_rd;
   reg [2:0] nb_packets;
	
   reg [31:0] ram0 [0:127];
   reg [31:0] ram1 [0:127];
   reg [31:0] ram2 [0:127];
   reg [31:0] ram3 [0:127];
	
   reg [31:0] dataout0;
   reg [31:0] dataout1;
   reg [31:0] dataout2;
   reg [31:0] dataout3;
	
   wire wr_done_int;
   wire rd_done_int;
   wire [6:0] rd_addr_final;
   wire [1:0] which_ram_rd_final;
	
   // USB side
   always @(posedge txclk)
       if(WR & (which_ram_wr == 2'd0)) ram0[wr_addr] <= datain;
			
   always @(posedge txclk)
       if(WR & (which_ram_wr == 2'd1)) ram1[wr_addr] <= datain;

   always @(posedge txclk)
       if(WR & (which_ram_wr == 2'd2)) ram2[wr_addr] <= datain;

   always @(posedge txclk)
       if(WR & (which_ram_wr == 2'd3)) ram3[wr_addr] <= datain;

   assign wr_done_int = ((WR && (wr_addr == 7'd127)) || WR_done);
   
   always @(posedge txclk)
       if(reset)
           wr_addr <= 0;
       else if (WR_done)
           wr_addr <= 0;
       else if (WR) 
           wr_addr <= wr_addr + 7'd1;
		
   always @(posedge txclk)
      if(reset)
          which_ram_wr <= 0;
      else if (wr_done_int) 
          which_ram_wr <= which_ram_wr + 2'd1;
	
   assign have_space = (nb_packets < 3'd3);
		
   // Reader side
   // short hand fifo
   // rd_addr_final is what rd_addr is going to be next clock cycle
   // which_ram_rd_final is what which_ram_rd is going to be next clock cycle
   always @(posedge txclk)  dataout0 <= ram0[rd_addr_final];
   always @(posedge txclk)  dataout1 <= ram1[rd_addr_final];
   always @(posedge txclk)  dataout2 <= ram2[rd_addr_final];
   always @(posedge txclk)  dataout3 <= ram3[rd_addr_final];
	
   assign dataout = (which_ram_rd_final[1]) ? 
                    (which_ram_rd_final[0] ? dataout3 : dataout2) :
                    (which_ram_rd_final[0] ? dataout1 : dataout0);

   //RD_done is the only way to signal the end of one packet
   assign rd_done_int = RD_done;   

   always @(posedge txclk)
       if (reset)
           rd_addr <= 0;
       else if (RD_done)
           rd_addr <= 0;
       else if (RD) 
           rd_addr <= rd_addr + 7'd1;
			
   assign rd_addr_final = (reset|RD_done) ? (6'd0) : 
	                  ((RD)?(rd_addr+7'd1):rd_addr); 
	
   always @(posedge txclk)
       if (reset)
           which_ram_rd <= 0;
       else if (rd_done_int)
           which_ram_rd <= which_ram_rd + 2'd1;

   assign which_ram_rd_final = (reset) ? (2'd0):
	                       ((rd_done_int) ? (which_ram_rd + 2'd1) : which_ram_rd);
	                        
   //packet_waiting is set to zero if rd_done_int is high
   //because there is no guarantee that nb_packets will be pos.

   assign packet_waiting = (nb_packets > 1) | ((nb_packets == 1)&(~rd_done_int));
   always @(posedge txclk)
       if (reset)
           nb_packets <= 0;
       else if (wr_done_int & ~rd_done_int)
           nb_packets <= nb_packets + 3'd1;
       else if (rd_done_int & ~wr_done_int)
           nb_packets <= nb_packets - 3'd1;
			
endmodule
