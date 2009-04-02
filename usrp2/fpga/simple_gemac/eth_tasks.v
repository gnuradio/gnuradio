

task SendFlowCtrl;
   input [15:0] fc_len;
   begin
      $display("Sending Flow Control, quanta = %d, time = %d", fc_len,$time);
      pause_time <= fc_len;
      @(posedge clk);
      pause_req <= 1;
      @(posedge clk);
      pause_req <= 0;
      $display("Sent Flow Control");
   end
endtask // SendFlowCtrl

task SendPacket2MAC;
   input tx_clk;
   input [7:0] data_start;
   input [31:0] data_len;
   output [7:0] tx_data;
   output tx_valid;
   output tx_error;
   input tx_ack;
   reg [15:0] count;
   begin
      $display("Sending Packet Len=%d, %d", data_len, $time);
      count <= 1;
      tx_data  <= data_start;
      tx_error <= 0;
      tx_valid <= 1;
      while(~tx_ack)
	@(posedge tx_clk);
      $display("Packet Accepted, %d", $time);
      while(count < data_len)
	begin
	   tx_data <= tx_data + 1;
	   count   <= count + 1;
	   @(posedge clk);
	end
      tx_valid <= 0;
      @(posedge tx_clk);
   end
endtask // SendPacket2MAC

task SendPacket_to_ll8;
   input [7:0] data_start;
   input [15:0] data_len;
//   output [7:0] tx_data;
//   output tx_sof;
//   output tx_eof;
//   output tx_src_rdy;
//   input tx_dst_rdy;
   reg [15:0] count;
   begin
      $display("Sending Packet Len=%d, %d", data_len, $time);
      count   <= 2;
      tx_ll_data2 <= data_start;
      tx_ll_src_rdy2 <= 1;
      tx_ll_sof2  <= 1;
      tx_ll_eof2  <= 0;
      #1;
      while(count < data_len)
	begin
	   while(~tx_ll_dst_rdy2)
	     @(posedge clk);
	   @(posedge clk);
	   tx_ll_data2 = tx_ll_data2 + 1;
	   count   = count + 1;
	   tx_ll_sof2 <= 0;
	end
      tx_ll_eof2 	   <= 1;
      while(~tx_ll_dst_rdy2)
	@(posedge clk);
      @(posedge clk);
      tx_ll_src_rdy2 <= 0;
   end
endtask // SendPacket_to_ll8


task SendPacketFromFile;
   input clk;
   input [31:0] data_len;
   output [7:0] tx_data;
   output tx_valid;
   output tx_error;
   input tx_ack;
   reg [15:0] count;
   begin
      $display("Sending Packet From File Len=%d, %d",data_len,$time);
      $readmemh("test_packet.mem",pkt_rom );     
      count 	  = 0;
      tx_data  = pkt_rom[count];
      tx_error = 0;
      tx_valid = 1;
      while(~tx_ack)
	@(posedge clk);
      $display("Packet Accepted, %d",$time);
      count = 1;
      while(count < data_len)
	begin
	   tx_data = pkt_rom[count];
	   count   = count + 1;
	   @(posedge clk);
	end
      tx_valid <= 0;
      @(posedge clk);
   end
endtask // SendPacketFromFile

task SendPacketFromFile_ll8;
   input [31:0] data_len;
   integer count;
   begin
      $display("Sending Packet From File to LL8 Len=%d, %d",data_len,$time);
      $readmemh("test_packet.mem",pkt_rom );     

      while(~tx_ll_dst_rdy2)
	@(posedge clk);
      tx_ll_data2 <= pkt_rom[0];
      tx_ll_src_rdy2 <= 1;
      tx_ll_sof2     <= 1;
      tx_ll_eof2     <= 0;
      @(posedge clk);
      
      for(i=1;i<data_len-1;i=i+1)
	begin
	   while(~tx_ll_dst_rdy2)
	     @(posedge clk);
	   tx_ll_data2 <= pkt_rom[i];
	   tx_ll_sof2  <= 0;
	   @(posedge clk);
	end
      
      while(~tx_ll_dst_rdy2)
	@(posedge clk);
      tx_ll_eof2 <= 1;
      tx_ll_data2 <= pkt_rom[data_len-1];
      @(posedge clk);
      tx_ll_src_rdy2 <= 0;
   end
endtask // SendPacketFromFile_ll8
