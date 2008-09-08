

module coeff_ram (input clock, input [3:0] rd_addr, output reg [15:0] rd_data);

   always @(posedge clock)
     case (rd_addr)
       4'd0 : rd_data <= #1 -16'd16;
       4'd1 : rd_data <= #1 16'd74;
       4'd2 : rd_data <= #1 -16'd254;
       4'd3 : rd_data <= #1 16'd669;
       4'd4 : rd_data <= #1 -16'd1468;
       4'd5 : rd_data <= #1 16'd2950;
       4'd6 : rd_data <= #1 -16'd6158;
       4'd7 : rd_data <= #1 16'd20585;
       4'd8 : rd_data <= #1 16'd20585;
       4'd9 : rd_data <= #1 -16'd6158;
       4'd10 : rd_data <= #1 16'd2950;
       4'd11 : rd_data <= #1 -16'd1468;
       4'd12 : rd_data <= #1 16'd669;
       4'd13 : rd_data <= #1 -16'd254;
       4'd14 : rd_data <= #1 16'd74;
       4'd15 : rd_data <= #1 -16'd16;
       default : rd_data <= #1 16'd0;
     endcase // case(rd_addr)
   
endmodule // ram
