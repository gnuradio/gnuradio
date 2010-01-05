

task SendFlowCtrl;
   input [15:0] fc_len;
   begin
      $display("Sending Flow Control, quanta = %d, time = %d", fc_len,$time);
      pause_time <= fc_len;
      @(posedge eth_clk);
      pause_req <= 1;
      @(posedge eth_clk);
      pause_req <= 0;
      $display("Sent Flow Control");
   end
endtask // SendFlowCtrl

task SendPacket_to_fifo19;
   input [31:0] data_start;
   input [15:0] data_len;
   reg [15:0] 	count;
   begin
      $display("Sending Packet Len=%d, %d", data_len, $time);
      count   <= 2;
      tx_f19_data <= {2'b0, 1'b0, 1'b1, data_start};
      tx_f19_src_rdy  <= 1;
      #1;
      while(count < data_len)
	begin
	   while(~tx_f19_dst_rdy)
	     @(posedge sys_clk);
	   @(posedge sys_clk);
	   //tx_f19_data[31:0] = tx_f19_data[31:0] + 32'h0101_0101;
	   count 	   = count + 4;
	   //tx_f19_data[32] <= 0;
	end
      //tx_f19_data[33] 	  <= 1;
      while(~tx_f19_dst_rdy)
	@(posedge sys_clk);
      @(posedge sys_clk);
      tx_f19_src_rdy <= 0;
   end
endtask // SendPacket_to_fifo19

/*
task Waiter;
   input [31:0] wait_length;
   begin
      tx_ll_src_rdy2 <= 0;
      repeat(wait_length)
	@(posedge clk);
      tx_ll_src_rdy2 <= 1;
   end
endtask // Waiter
*/

/*
task SendPacketFromFile_f19;
   input [31:0] data_len;
   input [31:0] wait_length;
   input [31:0] wait_time;
   
   integer count;
   begin
      $display("Sending Packet From File to LL8 Len=%d, %d",data_len,$time);
      $readmemh("test_packet.mem",pkt_rom );     

      while(~tx_f19_dst_rdy)
	@(posedge clk);
      tx_f19_data <= pkt_rom[0];
      tx_f19_src_rdy <= 1;
      tx_f19_eof     <= 0;
      @(posedge clk);
      
      for(i=1;i<data_len-1;i=i+1)
	begin
	   while(~tx_ll_dst_rdy2)
	     @(posedge clk);
	   tx_ll_data2 <= pkt_rom[i];
	   tx_ll_sof2  <= 0;
	   @(posedge clk);
//	   if(i==wait_time)
//	     Waiter(wait_length);
	end
      
      while(~tx_ll_dst_rdy2)
	@(posedge clk);
      tx_ll_eof2 <= 1;
      tx_ll_data2 <= pkt_rom[data_len-1];
      @(posedge clk);
      tx_ll_src_rdy2 <= 0;
   end
endtask
*/
