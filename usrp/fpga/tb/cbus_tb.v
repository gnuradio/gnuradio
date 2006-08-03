module cbus_tb;

`define ch1in_freq 0
`define ch2in_freq 1
`define ch3in_freq 2
`define ch4in_freq 3
`define ch1out_freq 4
`define ch2out_freq 5
`define ch3out_freq 6
`define ch4out_freq 7
`define rates 8
`define misc 9
  
   task send_config_word;
      input [7:0] addr;
      input [31:0] data;
      integer i;
      
      begin
	 #10 serenable = 1;
	 for(i=7;i>=0;i=i-1)
	   begin
	      #10 serdata = addr[i];
	      #10 serclk = 0;
	      #10 serclk = 1;
	      #10 serclk = 0;
	   end
	 for(i=31;i>=0;i=i-1)
	   begin
	      #10 serdata = data[i];
	      #10 serclk = 0;
	      #10 serclk = 1;
	      #10 serclk = 0;
	   end
	 #10 serenable = 0;
	 // #10 serclk = 1;
	 // #10 serclk = 0;
      end
   endtask // send_config_word
   
   initial $dumpfile("cbus_tb.vcd");
   initial $dumpvars(0,cbus_tb);

   initial reset = 1;
   initial #500 reset = 0;
      
   reg serclk, serdata, serenable, reset;
   wire SDO;
   
  control_bus control_bus
    ( .serial_clock(serclk),
      .serial_data_in(serdata),
      .enable(serenable),
      .reset(reset),
      .serial_data_out(SDO) );


   initial 
     begin
	#1000 send_config_word(8'd1,32'hDEAD_BEEF);
	#1000 send_config_word(8'd3,32'hDDEE_FF01);
	#1000 send_config_word(8'd19,32'hFFFF_FFFF);
	#1000 send_config_word(8'd23,32'h1234_FEDC);
	#1000 send_config_word(8'h80,32'h0);
	#1000 send_config_word(8'h81,32'h0);
	#1000 send_config_word(8'h82,32'h0);
	#1000 reset = 1;
	#1 $finish;
     end
   
endmodule // cbus_tb
