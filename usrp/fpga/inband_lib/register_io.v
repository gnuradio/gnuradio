module register_io
	(input clk, input reset, input wire [1:0] enable, input wire [6:0] addr, 
	 input wire [31:0] datain, output reg [31:0] dataout, output wire [15:0] debugbus,
	 input wire [31:0] rssi_0, input wire [31:0] rssi_1,
	 input wire [31:0] rssi_2, input wire [31:0] rssi_3, 
	 output wire [31:0] threshhold, output wire [31:0] rssi_wait);
	 
	reg strobe;
	wire [31:0] out[7:0];
	assign debugbus = {clk, enable, addr[2:0], datain[4:0], dataout[4:0]};
	assign threshhold = out[1];
	assign rssi_wait = out[2];
	
	always @(*)
        if (reset | ~enable[1])
           begin
             strobe <= 0;
		     dataout <= 0;
		   end
		else
		   begin
	         if (enable[0])
	           begin
	             //read
                 if (addr == 7'd9)
                 	dataout <= rssi_0;
                 else if (addr == 7'd10)
                 	dataout <= rssi_1;
                 else if (addr == 7'd11)
                 	dataout <= rssi_2;
                 else if (addr == 7'd12)
                 	dataout <= rssi_3;
                 else
	             	dataout <= out[addr[2:0]];
	             strobe <= 0;
               end
             else
               begin
                 //write
	             dataout <= dataout;
                 strobe <= 1;
               end
          end

	//register declarations
    setting_reg #(0) setting_reg0(.clock(clk),.reset(reset),
    .strobe(strobe),.addr(addr),.in(datain),.out(out[0]));
    setting_reg #(1) setting_reg1(.clock(clk),.reset(reset),
    .strobe(strobe),.addr(addr),.in(datain),.out(out[1]));
    setting_reg #(2) setting_reg2(.clock(clk),.reset(reset),
    .strobe(strobe),.addr(addr),.in(datain),.out(out[2]));
    setting_reg #(3) setting_reg3(.clock(clk),.reset(reset),
    .strobe(strobe),.addr(addr),.in(datain),.out(out[3]));
    setting_reg #(4) setting_reg4(.clock(clk),.reset(reset),
    .strobe(strobe),.addr(addr),.in(datain),.out(out[4]));
    setting_reg #(5) setting_reg5(.clock(clk),.reset(reset),
    .strobe(strobe),.addr(addr),.in(datain),.out(out[5]));
    setting_reg #(6) setting_reg6(.clock(clk),.reset(reset),
    .strobe(strobe),.addr(addr),.in(datain),.out(out[6]));
    setting_reg #(7) setting_reg7(.clock(clk),.reset(reset),
    .strobe(strobe),.addr(addr),.in(datain),.out(out[7]));
endmodule	