

module rssi (input clock, input reset, input enable,
	     input [11:0] adc, output [15:0] rssi, output [15:0] over_count);

   wire 		  over_hi = (adc == 12'h7FF);
   wire 		  over_lo = (adc == 12'h800);
   wire 		  over = over_hi | over_lo;

   reg [25:0] 		  over_count_int;
   always @(posedge clock)
     if(reset | ~enable)
       over_count_int <= #1 26'd0;
     else
       over_count_int <= #1 over_count_int + (over ? 26'd65535 : 26'd0) - over_count_int[25:10];
   
   assign      over_count = over_count_int[25:10];
   
   wire [11:0] abs_adc = adc[11] ? ~adc : adc;

   reg [25:0]  rssi_int;
   always @(posedge clock)
     if(reset | ~enable)
       rssi_int <= #1 26'd0;
     else
       rssi_int <= #1 rssi_int + abs_adc - rssi_int[25:10];

   assign      rssi = rssi_int[25:10];
   
endmodule // rssi
