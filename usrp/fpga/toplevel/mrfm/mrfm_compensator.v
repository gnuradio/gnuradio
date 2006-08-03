

module mrfm_compensator (input clock, input reset, input strobe_in,
			 input serial_strobe, input [6:0] serial_addr, input [31:0] serial_data,
			 input [15:0] i_in, input [15:0] q_in, output reg [15:0] i_out, output reg [15:0] q_out);

   wire [15:0] 			      a11,a12,a21,a22;
   reg [15:0] 			      i_in_reg, q_in_reg;
   wire [30:0] 			      product;
   reg [3:0] 			      phase;
   wire [15:0] 			      data,coeff;
   wire [7:0] 			      shift;
   wire [33:0] 			      accum;
   wire [15:0] 			      scaled_accum;
   wire enable_acc;

   setting_reg #(`FR_MRFM_COMP_A11) sr_a11(.clock(clock),.reset(reset),
					   .strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
					   .out(a11),.changed());
   setting_reg #(`FR_MRFM_COMP_A12) sr_a12(.clock(clock),.reset(reset),
					   .strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
					   .out(a12),.changed());
   setting_reg #(`FR_MRFM_COMP_A21) sr_a21(.clock(clock),.reset(reset),
					   .strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
					   .out(a21),.changed());
   setting_reg #(`FR_MRFM_COMP_A22) sr_a22(.clock(clock),.reset(reset),
					   .strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
					   .out(a22),.changed());
   setting_reg #(`FR_MRFM_COMP_SHIFT) sr_cshift(.clock(clock),.reset(reset),
						.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
						.out(shift),.changed());
   
   mult mult (.clock(clock),.x(data),.y(coeff),.product(product),.enable_in(1'b1),.enable_out() );
   acc acc (.clock(clock),.reset(reset),.clear(clear_acc),.enable_in(enable_acc),.enable_out(),
	    .addend(product),.sum(accum) );   
   shifter shifter (.in(accum),.out(scaled_accum),.shift(shift));
   
   always @(posedge clock)
     if(reset)
       begin
	  i_in_reg <= #1 16'd0;
	  q_in_reg <= #1 16'd0;
       end
     else if(strobe_in)
       begin
	  i_in_reg <= #1 i_in;
	  q_in_reg <= #1 q_in;
       end	  

   always @(posedge clock)
     if(reset)
       phase <= #1 4'd0;
     else if(strobe_in)
       phase <= #1 4'd1;
     else if(strobe_in != 4'd8)
       phase <= #1 phase + 4'd1;

   assign data = ((phase == 4'd1)||(phase === 4'd4)) ? i_in_reg : 
	  ((phase == 4'd2)||(phase == 4'd5)) ? q_in_reg : 16'd0;

   assign coeff = (phase == 4'd1) ? a11 : (phase == 4'd2) ? a12 : 
	  (phase == 4'd4) ? a21 : (phase == 4'd5) ? a22 : 16'd0;

   assign clear_acc = (phase == 4'd0) || (phase == 4'd1) || (phase == 4'd4) || (phase==4'd8);
   assign enable_acc = ~clear_acc;
   
   always @(posedge clock)
     if(reset)
       i_out <= #1 16'd0;
     else if(phase == 4'd4)
       i_out <= #1 scaled_accum;

   always @(posedge clock)
     if(reset)
       q_out <= #1 16'd0;
     else if(phase == 4'd7)
       q_out <= #1 scaled_accum;
   
       
endmodule // mrfm_compensator
