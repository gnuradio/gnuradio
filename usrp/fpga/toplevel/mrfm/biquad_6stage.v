`include "mrfm.vh"

module mrfm_iir (input clock, input reset, input strobe_in,
		 input serial_strobe, input [6:0] serial_addr, input [31:0] serial_data,
		 input wire [15:0] sample_in, output reg [15:0] sample_out);

   wire [5:0] 	       coeff_addr, coeff_wr_addr;
   wire [4:0] 	       data_addr, data_wr_addr;
   reg [4:0] 	       cur_offset, data_addr_int, data_wr_addr_int;
   
   wire [15:0] 	       coeff, coeff_wr_data, data, data_wr_data;
   wire 	       coeff_wr;
   reg 		       data_wr;
   
   wire [30:0] 	       product;
   wire [33:0] 	       accum;
   wire [15:0] 	       scaled_accum; 		 
   
   wire [7:0] 	       shift;
   reg [5:0] 	       phase;
   wire 	       enable_mult, enable_acc, latch_out, select_input;
   reg 		       done, clear_acc;
   
   setting_reg #(`FR_MRFM_IIR_COEFF) sr_coeff(.clock(clock),.reset(reset),
					      .strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
					      .out({coeff_wr_addr,coeff_wr_data}),.changed(coeff_wr));
   
   setting_reg #(`FR_MRFM_IIR_SHIFT) sr_shift(.clock(clock),.reset(reset),
					      .strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
					      .out(shift),.changed());
   
   ram64 coeff_ram(.clock(clock),.write(coeff_wr),.wr_addr(coeff_wr_addr),.wr_data(coeff_wr_data),
		   .rd_addr(coeff_addr),.rd_data(coeff));
   
   ram32 data_ram(.clock(clock),.write(data_wr),.wr_addr(data_wr_addr),.wr_data(data_wr_data),
		  .rd_addr(data_addr),.rd_data(data));
   
   mult mult (.clock(clock),.x(data),.y(coeff),.product(product),.enable_in(enable_mult),.enable_out() );
   
   acc acc (.clock(clock),.reset(reset),.clear(clear_acc),.enable_in(enable_acc),.enable_out(),
	    .addend(product),.sum(accum) );
   
   shifter shifter (.in(accum),.out(scaled_accum),.shift(shift));
   
   assign 	       data_wr_data = select_input ? sample_in : scaled_accum;
   assign 	       enable_mult = 1'b1;
   
   always @(posedge clock)
     if(reset)
       cur_offset <= #1 5'd0;
     else if(latch_out)
       cur_offset <= #1 cur_offset + 5'd1;
   
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
       phase <= #1 6'd0;
     else if(strobe_in)
       phase <= #1 6'd0;
     else if(!done)
       phase <= #1 phase + 6'd1;
   
   always @(phase)
     case(phase)
       6'd0 : data_addr_int = 5'd0;
       default : data_addr_int = 5'd0;
     endcase // case(phase)
   
   assign 	       coeff_addr = phase;
   
   always @(phase)
     case(phase)
       6'd01 : data_addr_int = 5'd00; 6'd02 : data_addr_int = 5'd01; 6'd03 : data_addr_int = 5'd02;
       6'd04 : data_addr_int = 5'd03; 6'd05 : data_addr_int = 5'd04;
       
       6'd07 : data_addr_int = 5'd03; 6'd08 : data_addr_int = 5'd04; 6'd09 : data_addr_int = 5'd05;
       6'd10 : data_addr_int = 5'd06; 6'd11 : data_addr_int = 5'd07;
       
       6'd13 : data_addr_int = 5'd06; 6'd14 : data_addr_int = 5'd07; 6'd15 : data_addr_int = 5'd08;
       6'd16 : data_addr_int = 5'd09; 6'd17 : data_addr_int = 5'd10;
       
       6'd19 : data_addr_int = 5'd09; 6'd20 : data_addr_int = 5'd10; 6'd21 : data_addr_int = 5'd11;
       6'd22 : data_addr_int = 5'd12; 6'd23 : data_addr_int = 5'd13;
       
       6'd25 : data_addr_int = 5'd12; 6'd26 : data_addr_int = 5'd13; 6'd27 : data_addr_int = 5'd14;
       6'd28 : data_addr_int = 5'd15; 6'd29 : data_addr_int = 5'd16;
       
       6'd31 : data_addr_int = 5'd15; 6'd32 : data_addr_int = 5'd16; 6'd33 : data_addr_int = 5'd17;
       6'd34 : data_addr_int = 5'd18; 6'd35 : data_addr_int = 5'd19;
       
       default : data_addr_int = 5'd00;
     endcase // case(phase)
   
   always @(phase)
     case(phase)
       6'd0 : data_wr_addr_int = 5'd2;
       6'd8 : data_wr_addr_int = 5'd5;
       6'd14 : data_wr_addr_int = 5'd8;
       6'd20 : data_wr_addr_int = 5'd11;
       6'd26 : data_wr_addr_int = 5'd14;
       6'd32 : data_wr_addr_int = 5'd17;
       6'd38 : data_wr_addr_int = 5'd20;
       default : data_wr_addr_int = 5'd0;
     endcase // case(phase)

   always @(phase)
     case(phase)
       6'd0, 6'd8, 6'd14, 6'd20, 6'd26, 6'd32, 6'd38: data_wr = 1'b1;
       default : data_wr = 1'b0;
     endcase // case(phase)
      
   always @(phase)
     case(phase)
       6'd0, 6'd1, 6'd2, 6'd3, 6'd9, 6'd15, 6'd21, 6'd27, 6'd33 : clear_acc = 1'd1;
       default : clear_acc = 1'b0;
     endcase // case(phase)
   
   assign 	       enable_acc = ~clear_acc;
   assign 	       latch_out = (phase == 6'd38);
   
   always @(posedge clock)
     if(reset)
       sample_out <= #1 16'd0;
     else if(latch_out)
       sample_out <= #1 scaled_accum;
   
endmodule // mrfm_iir
