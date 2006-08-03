

module coeff_rom (input clock, input [2:0] addr, output reg [15:0] data);

   always @(posedge clock)
     case (addr)
       3'd0 : data <= #1 -16'd16;
       3'd1 : data <= #1 16'd74;
       3'd2 : data <= #1 -16'd254;
       3'd3 : data <= #1 16'd669;
       3'd4 : data <= #1 -16'd1468;
       3'd5 : data <= #1 16'd2950;
       3'd6 : data <= #1 -16'd6158;
       3'd7 : data <= #1 16'd20585;
     endcase // case(addr)
      
endmodule // coeff_rom


