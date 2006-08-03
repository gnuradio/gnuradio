

module mac (input clock, input reset, input enable, input clear,
	    input signed [15:0] x, input signed [15:0] y,
	    input [7:0] shift, output [15:0] z );

   reg signed [30:0] product;
   reg signed [39:0] z_int;
   reg signed [15:0] z_shift;

   reg enable_d1;
   always @(posedge clock)
     enable_d1 <= #1 enable;
   
   always @(posedge clock)
     if(reset | clear)
       z_int <= #1 40'd0;
     else if(enable_d1)
       z_int <= #1 z_int + {{9{product[30]}},product};

   always @(posedge clock)
     product <= #1 x*y;

   always @*   // FIXME full case? parallel case?
     case(shift)
       //8'd0 : z_shift <= z_int[39:24];
       //8'd1 : z_shift <= z_int[38:23];
       //8'd2 : z_shift <= z_int[37:22];
       //8'd3 : z_shift <= z_int[36:21];
       //8'd4 : z_shift <= z_int[35:20];
       //8'd5 : z_shift <= z_int[34:19];
       8'd6 : z_shift <= z_int[33:18];
       8'd7 : z_shift <= z_int[32:17];
       8'd8 : z_shift <= z_int[31:16];
       8'd9 : z_shift <= z_int[30:15];
       8'd10 : z_shift <= z_int[29:14];
       8'd11 : z_shift <= z_int[28:13];
       //8'd12 : z_shift <= z_int[27:12];
       //8'd13 : z_shift <= z_int[26:11];
       //8'd14 : z_shift <= z_int[25:10];
       //8'd15 : z_shift <= z_int[24:9];
       //8'd16 : z_shift <= z_int[23:8];
       //8'd17 : z_shift <= z_int[22:7];
       //8'd18 : z_shift <= z_int[21:6];
       //8'd19 : z_shift <= z_int[20:5];
       //8'd20 : z_shift <= z_int[19:4];
       //8'd21 : z_shift <= z_int[18:3];
       //8'd22 : z_shift <= z_int[17:2];
       //8'd23 : z_shift <= z_int[16:1];
       //8'd24 : z_shift <= z_int[15:0];
       default : z_shift <= z_int[15:0];
     endcase // case(shift)
   
   // FIXME do we need to saturate?
   //assign z = z_shift;
   assign z = z_int[15:0];
   
endmodule // mac
