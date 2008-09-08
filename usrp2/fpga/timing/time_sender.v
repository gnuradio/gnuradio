

module time_sender
  (input clk, input rst,
   input [31:0] master_time,
   input send_sync,
   output exp_pps_out);

   reg [7:0] datain;
   reg 	     k;
   wire [9:0] dataout;
   reg [9:0] dataout_reg;
   reg 	     disp_reg;
   wire      disp, new_word;
   
   encode_8b10b encode_8b10b 
     (.datain({k,datain}),.dispin(disp_reg),
      .dataout(dataout),.dispout(disp));

   assign    exp_pps_out = dataout_reg[0];

   always @(posedge clk)
     if(rst)
       disp_reg <= 0;
     else if(new_word)
       disp_reg <= disp;
   
   always @(posedge clk)
     if(rst)
       dataout_reg <= 0;
     else if(new_word)
       dataout_reg <= dataout;
     else
       dataout_reg <= {1'b0,dataout_reg[9:1]};
   
   reg [4:0] state;
   reg [3:0] bit_count;

   assign    new_word = (bit_count == 9);
   
   always @(posedge clk)
     if(rst)
       bit_count <= 0;
     else if(new_word | send_sync)
       bit_count <= 0;
     else
       bit_count <= bit_count + 1;

   localparam SEND_IDLE = 0;
   localparam SEND_HEAD = 1;
   localparam SEND_T0 = 2;
   localparam SEND_T1 = 3;
   localparam SEND_T2 = 4;
   localparam SEND_T3 = 5;

   localparam COMMA = 8'hBC;
   localparam HEAD = 8'h3C;

   reg [31:0] master_time_reg;
   
   always @(posedge clk)
     if(rst)
       master_time_reg <= 0;
     else if(send_sync)
       master_time_reg <= master_time;
   
   always @(posedge clk)
     if(rst)
       begin
	  {k,datain} <= 0;
	  state <= SEND_IDLE;
       end
     else
       if(send_sync)
	 state <= SEND_HEAD;
       else if(new_word)
	 case(state)
	   SEND_IDLE :
	     {k,datain} <= {1'b1,COMMA};
	   SEND_HEAD :
	     begin
		{k,datain} <= {1'b1, HEAD};
		state <= SEND_T0;
	     end
	   SEND_T0 :
	     begin
		{k,datain} <= {1'b0, master_time_reg[31:24] };
		state <= SEND_T1;
	     end
	   SEND_T1 :
	     begin
		{k,datain} <= {1'b0, master_time_reg[23:16]};
		state <= SEND_T2;
	     end
	   SEND_T2 :
	     begin
		{k,datain} <= {1'b0, master_time_reg[15:8]};
		state <= SEND_T3;
	     end
	   SEND_T3 :
	     begin
		{k,datain} <= {1'b0, master_time_reg[7:0]};
		state <= SEND_IDLE;
	     end
	   default :
	     state <= SEND_IDLE;
	 endcase // case(state)
   
   
endmodule // time_sender
