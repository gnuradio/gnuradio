
module medfifo
  #(parameter WIDTH=32,
    parameter DEPTH=1)
    (input clk, input rst,
     input [WIDTH-1:0] datain,
     output [WIDTH-1:0] dataout,
     input read,
     input write,
     input clear,
     output full,
     output empty,
     output [7:0] space,
     output [7:0] occupied);

   localparam 		NUM_FIFOS = (1<<DEPTH);
   
   wire [WIDTH-1:0] 	dout [0:NUM_FIFOS-1];
   wire [0:NUM_FIFOS-1] full_x;
   wire [0:NUM_FIFOS-1] empty_x;

   shortfifo #(.WIDTH(WIDTH))
     head (.clk(clk),.rst(rst),
	   .datain(datain),.write(write),.full(full),
	   .dataout(dout[0]),.read(~empty_x[0] & ~full_x[1]),.empty(empty_x[0]),
	   .clear(clear),.space(space[4:0]),.occupied() );
   
   shortfifo #(.WIDTH(WIDTH))
     tail (.clk(clk),.rst(rst),
	   .datain(dout[NUM_FIFOS-2]),.write(~empty_x[NUM_FIFOS-2] & ~full_x[NUM_FIFOS-1]),.full(full_x[NUM_FIFOS-1]),
	   .dataout(dataout),.read(read),.empty(empty),
	   .clear(clear),.space(),.occupied(occupied[4:0]) );

   genvar 		i;
   generate
      for(i = 1; i < NUM_FIFOS-1 ; i = i + 1)
	begin : gen_depth
	   shortfifo #(.WIDTH(WIDTH))
	     shortfifo (.clk(clk),.rst(rst),
			.datain(dout[i-1]),.write(~full_x[i] & ~empty_x[i-1]),.full(full_x[i]),
			.dataout(dout[i]),.read(~full_x[i+1] & ~empty_x[i]),.empty(empty_x[i]),
			.clear(clear),.space(),.occupied() );
	end
   endgenerate

   assign space[7:5] = 0;
   assign occupied[7:5] = 0;
   
endmodule // medfifo
