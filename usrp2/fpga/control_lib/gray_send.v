


module gray_send
  #(parameter WIDTH = 8)
    (input clk_in, input [WIDTH-1:0] addr_in,
     input clk_out, output reg [WIDTH-1:0] addr_out);

   reg [WIDTH-1:0] gray_clkin, gray_clkout, gray_clkout_d1;
   wire [WIDTH-1:0] gray, bin;

   bin2gray #(.WIDTH(WIDTH)) b2g (.bin(addr_in), .gray(gray) );

   always @(posedge clk_in)
     gray_clkin <= gray;

   always @(posedge clk_out)
     gray_clkout <= gray_clkin;

   always @(posedge clk_out)
     gray_clkout_d1 <= gray_clkout;
   
   gray2bin #(.WIDTH(WIDTH)) g2b (.gray(gray_clkout_d1), .bin(bin) );

   // FIXME we may not need the next register, but it may help timing
   always @(posedge clk_out)
     addr_out <= bin;
   
endmodule // gray_send
