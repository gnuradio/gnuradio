

module mux8
  #(parameter WIDTH=32, parameter DISABLED=0)
    (input en,
     input [2:0] sel,
     input [WIDTH-1:0] i0,
     input [WIDTH-1:0] i1,
     input [WIDTH-1:0] i2,
     input [WIDTH-1:0] i3,
     input [WIDTH-1:0] i4,
     input [WIDTH-1:0] i5,
     input [WIDTH-1:0] i6,
     input [WIDTH-1:0] i7,
     output [WIDTH-1:0] o);

   assign 		o = en ? (sel[2] ? (sel[1] ? (sel[0] ? i7 : i6) : (sel[0] ? i5 : i4)) :
				  (sel[1] ? (sel[0] ? i3 : i2) : (sel[0] ? i1 : i0))) :
			DISABLED;
   
endmodule // mux8
