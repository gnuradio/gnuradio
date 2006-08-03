`include "mrfm.vh"

module biquad_2stage (input clock, input reset, input strobe_in,
		      input serial_strobe, input [6:0] serial_addr, input [31:0] serial_data,
		      input wire [15:0] sample_in, output reg [15:0] sample_out, output wire [63:0] debugbus);

   wire [3:0] 	       coeff_addr, coeff_wr_addr;
   wire [3:0] 	       data_addr, data_wr_addr;
   reg [3:0] 	       cur_offset, data_addr_int, data_wr_addr_int;
   
   wire [15:0] 	       coeff, coeff_wr_data, data, data_wr_data;
   wire 	       coeff_wr;
   reg 		       data_wr;
   
   wire [30:0] 	       product;
   wire [33:0] 	       accum;
   wire [15:0] 	       scaled_accum; 		 
   
   wire [7:0] 	       shift;
   reg [3:0] 	       phase;
   wire 	       enable_mult, enable_acc, latch_out, select_input;
   reg 		       done, clear_acc;
   
   setting_reg #(`FR_MRFM_IIR_COEFF) sr_coeff(.clock(clock),.reset(reset),
					      .strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
					      .out({coeff_wr_addr,coeff_wr_data}),.changed(coeff_wr));
   
   setting_reg #(`FR_MRFM_IIR_SHIFT) sr_shift(.clock(clock),.reset(reset),
					      .strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
					      .out(shift),.changed());
   
   ram16 coeff_ram(.clock(clock),.write(coeff_wr),.wr_addr(coeff_wr_addr),.wr_data(coeff_wr_data),
		   .rd_addr(coeff_addr),.rd_data(coeff));
   
   ram16 data_ram(.clock(clock),.write(data_wr),.wr_addr(data_wr_addr),.wr_data(data_wr_data),
		  .rd_addr(data_addr),.rd_data(data));
   
   mult mult (.clock(clock),.x(data),.y(coeff),.product(product),.enable_in(enable_mult),.enable_out() );
   
   acc acc (.clock(clock),.reset(reset),.clear(clear_acc),.enable_in(enable_acc),.enable_out(),
	    .addend(product),.sum(accum) );
   
   shifter shifter (.in(accum),.out(scaled_accum),.shift(shift));
   
   assign 	       data_wr_data = select_input ? sample_in : scaled_accum;
   assign 	       enable_mult = 1'b1;
   
   always @(posedge clock)
     if(reset)
       cur_offset <= #1 4'd0;
     else if(latch_out)
       cur_offset <= #1 cur_offset + 4'd1;
   
   assign 	       data_addr = data_addr_int + cur_offset;		 
   assign 	       data_wr_addr = data_wr_addr_int + cur_offset;		 
   
   always @(posedge clock)
     if(reset)
       done <= #1 1'b0;
     else if(latch_out)
       done <= #1 1'b1;
     else if(strobe_in)
       done <= #1 1'b0;
   
   always @(posedge clock)
     if(reset)
       phase <= #1 4'd0;
     else if(strobe_in)
       phase <= #1 4'd0;
     else if(!done)
       phase <= #1 phase + 4'd1;
   
   assign 	       coeff_addr = phase;
   
   always @(phase)
     case(phase)
       4'd01 : data_addr_int = 4'd00; 4'd02 : data_addr_int = 4'd01; 4'd03 : data_addr_int = 4'd02;
       4'd04 : data_addr_int = 4'd03; 4'd05 : data_addr_int = 4'd04;
       
       4'd07 : data_addr_int = 4'd03; 4'd08 : data_addr_int = 4'd04; 4'd09 : data_addr_int = 4'd05;
       4'd10 : data_addr_int = 4'd06; 4'd11 : data_addr_int = 4'd07;
       default : data_addr_int = 4'd00;
     endcase // case(phase)
   
   always @(phase)
     case(phase)
       4'd0 : data_wr_addr_int = 4'd2;
       4'd8 : data_wr_addr_int = 4'd5;
       4'd14 : data_wr_addr_int = 4'd8;
       default : data_wr_addr_int = 4'd0;
     endcase // case(phase)

   always @(phase)
     case(phase)
       4'd0, 4'd8, 4'd14 : data_wr = 1'b1;
       default : data_wr = 1'b0;
     endcase // case(phase)

   assign 	       select_input = (phase == 4'd0);
   
   always @(phase)
     case(phase)
       4'd0, 4'd1, 4'd2, 4'd3, 4'd9, 4'd15 : clear_acc = 1'd1;
       default : clear_acc = 1'b0;
     endcase // case(phase)
   
   assign 	       enable_acc = ~clear_acc;
   assign 	       latch_out = (phase == 4'd14);
   
   always @(posedge clock)
     if(reset)
       sample_out <= #1 16'd0;
     else if(latch_out)
       sample_out <= #1 scaled_accum;

   ////////////////////////////////////////////////////////
   //  Debug

   wire [3:0] 	       debugmux;
   
   setting_reg #(`FR_MRFM_DEBUG) sr_debugmux(.clock(clock),.reset(reset),
					     .strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
					     .out(debugmux),.changed());

   assign 	       debugbus[15:0] = debugmux[0] ? {coeff_addr,data_addr,data_wr_addr,cur_offset} : {phase,data_addr_int,data_wr_addr_int,cur_offset};
   assign 	       debugbus[31:16] = debugmux[1] ? scaled_accum : {clock, strobe_in, data_wr, enable_mult, enable_acc, clear_acc, latch_out,select_input,done, data_addr_int};
   assign 	       debugbus[47:32] = debugmux[2] ? sample_out : coeff;
   assign 	       debugbus[63:48] = debugmux[3] ? sample_in : data;
      
endmodule // biquad_2stage

