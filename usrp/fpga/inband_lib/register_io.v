module register_io
	(clk, reset, enable, addr, datain, dataout, debugbus, addr_wr, data_wr, strobe_wr,
	 rssi_0, rssi_1, rssi_2, rssi_3, threshhold, rssi_wait, reg_0, reg_1, reg_2, reg_3, 
     debug_en, misc, txmux);   
	
	input clk;
	input reset;
	input wire [1:0] enable;
	input wire [6:0] addr; 
	input wire [31:0] datain;
	output reg [31:0] dataout;
	output wire [15:0] debugbus;
	output reg [6:0] addr_wr;
	output reg [31:0] data_wr;
	output wire strobe_wr; 
	input wire [31:0] rssi_0;
	input wire [31:0] rssi_1;
	input wire [31:0] rssi_2; 
	input wire [31:0] rssi_3; 
	output wire [31:0] threshhold;
	output wire [31:0] rssi_wait;
	input wire [15:0] reg_0;
	input wire [15:0] reg_1; 
	input wire [15:0] reg_2; 
	input wire [15:0] reg_3;
	input wire [3:0]  debug_en;
	input wire [7:0]  misc;
	input wire [31:0] txmux;
	
	reg strobe;
	wire [31:0] out[2:1];
	assign debugbus = {clk, enable, addr[2:0], datain[4:0], dataout[4:0]};
	assign threshhold = out[1];
	assign rssi_wait = out[2];
	assign strobe_wr = strobe;
	
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
				if (addr <= 7'd52 && addr > 7'd50)
					dataout <= out[addr-7'd50];
				else
					dataout <= 32'hFFFFFFFF; 	
	            strobe <= 0;
              end
             else
               begin
                 //write
	             dataout <= dataout;
                 strobe <= 1;
				 data_wr <= datain;
				 addr_wr <= addr;
               end
          end

//register declarations
    /*setting_reg #(50) setting_reg0(.clock(clk),.reset(reset),
    .strobe(strobe_wr),.addr(addr_wr),.in(data_wr),.out(out[0]));*/
    setting_reg #(51) setting_reg1(.clock(clk),.reset(reset),
    .strobe(strobe_wr),.addr(addr_wr),.in(data_wr),.out(out[1]));
    setting_reg #(52) setting_reg2(.clock(clk),.reset(reset),
    .strobe(strobe_wr),.addr(addr_wr),.in(data_wr),.out(out[2]));
    /*setting_reg #(53) setting_reg3(.clock(clk),.reset(reset),
    .strobe(strobe_wr),.addr(addr_wr),.in(data_wr),.out(out[3]));
    setting_reg #(54) setting_reg4(.clock(clk),.reset(reset),
    .strobe(strobe_wr),.addr(addr_wr),.in(data_wr),.out(out[4]));
    setting_reg #(55) setting_reg5(.clock(clk),.reset(reset),
    .strobe(strobe_wr),.addr(addr_wr),.in(data_wr),.out(out[5]));
    setting_reg #(56) setting_reg6(.clock(clk),.reset(reset),
    .strobe(strobe_wr),.addr(addr_wr),.in(data_wr),.out(out[6]));
    setting_reg #(57) setting_reg7(.clock(clk),.reset(reset),
    .strobe(strobe_wr),.addr(addr_wr),.in(data_wr),.out(out[7]));*/

endmodule	
