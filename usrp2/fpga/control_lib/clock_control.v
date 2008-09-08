

// AD9510 Register Map (from datasheet Rev. A)

/* INSTRUCTION word format (16 bits)
 * 15       Read = 1, Write = 0
 * 14:13    W1/W0,  Number of bytes 00 - 1, 01 - 2, 10 - 3, 11 - stream
 * 12:0     Address
 */

/* ADDR     Contents             Value (hex)
 * 00       Serial Config Port   10 (def) -- MSB first, SDI/SDO separate
 * 04       A Counter
 * 05-06    B Counter
 * 07-0A    PLL Control
 * 0B-0C    R Divider
 * 0D       PLL Control
 * 34-3A    Fine Delay
 * 3C-3F    LVPECL Outs
 * 40-43    LVDS/CMOS Outs
 * 45       Clock select, power down
 * 48-57    Dividers
 * 58       Func and Sync
 * 5A       Update regs
 */


module clock_control
  (input reset,
   input aux_clk,            // 25MHz, for before fpga clock is active
   input clk_fpga,           // real 100 MHz FPGA clock
   output [1:0] clk_en,      // controls source of reference clock
   output [1:0] clk_sel,     // controls source of reference clock
   input clk_func,          // FIXME needs to be some kind of out SYNC or reset to 9510
   input clk_status,         // Monitor PLL or SYNC status
   
   output sen,        // Enable for the AD9510
   output sclk,       // FIXME these need to be shared
   input sdi,
   output sdo
   );
   
   wire   read = 1'b0;    // Always write for now
   wire [1:0] w = 2'b00;  // Always send 1 byte at a time
   
   assign     clk_sel = 2'b00;  // Both outputs from External Ref (SMA)
   assign     clk_en = 2'b11;   // Both outputs enabled
   
   reg [20:0] addr_data;

   reg [5:0]  entry;
   reg 	      start;
   reg [7:0]  counter;
   reg [23:0] command;
   
   always @*
     case(entry)
       6'd00 : addr_data = {13'h00,8'h10};   // Serial setup
       6'd01 : addr_data = {13'h45,8'h00};   // CLK2 drives distribution, everything on
       6'd02 : addr_data = {13'h3D,8'h80};   // Turn on output 1, normal levels
       6'd03 : addr_data = {13'h4B,8'h80};   // Bypass divider 1 (div by 1)
       6'd04 : addr_data = {13'h08,8'h47};   // POS PFD, Dig LK Det, Charge Pump normal	
       6'd05 : addr_data = {13'h09,8'h70};   // Max Charge Pump current
       6'd06 : addr_data = {13'h0A,8'h04};   // Normal operation, Prescalar Div by 2, PLL On
       6'd07 : addr_data = {13'h0B,8'h00};   // RDIV MSB (6 bits)
       6'd08 : addr_data = {13'h0C,8'h01};   // RDIV LSB (8 bits), Div by 1
       6'd09 : addr_data = {13'h0D,8'h00};   // Everything normal, Dig Lock Det
       6'd10 : addr_data = {13'h07,8'h00};	// Disable LOR detect - LOR causes failure...
       6'd11 : addr_data = {13'h04,8'h00};	// A Counter = Don't Care
       6'd12 : addr_data = {13'h05,8'h00};	// B Counter MSB = 0
       6'd13 : addr_data = {13'h06,8'h05};   // B Counter LSB = 5
       default : addr_data = {13'h5A,8'h01}; // Register Update
     endcase // case(entry)

   wire [5:0]  lastentry = 6'd15;
   wire        done = (counter == 8'd49);
   
   always @(posedge aux_clk)
     if(reset)
       begin
	  entry <= #1 6'd0;
	  start <= #1 1'b1;
       end
     else if(start)
       start <= #1 1'b0;
     else if(done && (entry<lastentry))
       begin
	  entry <= #1 entry + 6'd1;
	  start <= #1 1'b1;
       end
   
   always @(posedge aux_clk)
     if(reset)
       begin
	  counter <= #1 7'd0;
	  command <= #1 20'd0;
       end
     else if(start)
       begin
	  counter <= #1 7'd1;
	  command <= #1 {read,w,addr_data};
       end
     else if( |counter && ~done )
       begin
	  counter <= #1 counter + 7'd1;
	  if(~counter[0])
	    command <= {command[22:0],1'b0};
       end
   
   
   assign sen = (done | counter == 8'd0);  // CSB is high when we're not doing anything
   assign sclk = ~counter[0];
   assign sdo = command[23];
   
endmodule // clock_control
