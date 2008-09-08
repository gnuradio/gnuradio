
// Dual ported RAM
//    Addresses are byte-oriented, so botton 2 address bits are ignored.
//    AWIDTH of 13 allows you to address 8K bytes.  
//    For Spartan 3, if the total RAM size is not a multiple of 8K then BRAM space is wasted
// RAM_SIZE parameter allows odd-sized RAMs, like 24K

module dpram32 #(parameter AWIDTH=15, 
		 parameter RAM_SIZE=16384)
  (input clk,
   
   input [AWIDTH-1:0] adr1_i,
   input [31:0] dat1_i,
   output reg [31:0] dat1_o,
   input we1_i,
   input en1_i,
   input [3:0] sel1_i,

   input [AWIDTH-1:0] adr2_i,
   input [31:0] dat2_i,
   output reg [31:0] dat2_o,
   input we2_i,
   input en2_i,
   input [3:0] sel2_i );
   
   reg [7:0]   ram0 [0:(RAM_SIZE/4)-1];
   reg [7:0]   ram1 [0:(RAM_SIZE/4)-1];
   reg [7:0]   ram2 [0:(RAM_SIZE/4)-1];
   reg [7:0]   ram3 [0:(RAM_SIZE/4)-1];

   //  This is how we used to size the RAM -->  
   //      reg [7:0]   ram3 [0:(1<<(AWIDTH-2))-1];
   
   // Port 1
   always @(posedge clk)
     if(en1_i) dat1_o[31:24] <= ram3[adr1_i[AWIDTH-1:2]];
   always @(posedge clk)
     if(en1_i) dat1_o[23:16] <= ram2[adr1_i[AWIDTH-1:2]];
   always @(posedge clk)
     if(en1_i) dat1_o[15:8] <= ram1[adr1_i[AWIDTH-1:2]];
   always @(posedge clk)
     if(en1_i) dat1_o[7:0] <= ram0[adr1_i[AWIDTH-1:2]];
   
   always @(posedge clk)
     if(we1_i & en1_i & sel1_i[3])
       ram3[adr1_i[AWIDTH-1:2]] <= dat1_i[31:24];
   always @(posedge clk)
     if(we1_i & en1_i & sel1_i[2])
       ram2[adr1_i[AWIDTH-1:2]] <= dat1_i[23:16];
   always @(posedge clk)
     if(we1_i & en1_i & sel1_i[1])
       ram1[adr1_i[AWIDTH-1:2]] <= dat1_i[15:8];
   always @(posedge clk)
     if(we1_i & en1_i & sel1_i[0])
       ram0[adr1_i[AWIDTH-1:2]] <= dat1_i[7:0];
   
   // Port 2
   always @(posedge clk)
     if(en2_i) dat2_o[31:24] <= ram3[adr2_i[AWIDTH-1:2]];
   always @(posedge clk)
     if(en2_i) dat2_o[23:16] <= ram2[adr2_i[AWIDTH-1:2]];
   always @(posedge clk)
     if(en2_i) dat2_o[15:8] <= ram1[adr2_i[AWIDTH-1:2]];
   always @(posedge clk)
     if(en2_i) dat2_o[7:0] <= ram0[adr2_i[AWIDTH-1:2]];
   
   always @(posedge clk)
     if(we2_i & en2_i & sel2_i[3])
       ram3[adr2_i[AWIDTH-1:2]] <= dat2_i[31:24];
   always @(posedge clk)
     if(we2_i & en2_i & sel2_i[2])
       ram2[adr2_i[AWIDTH-1:2]] <= dat2_i[23:16];
   always @(posedge clk)
     if(we2_i & en2_i & sel2_i[1])
       ram1[adr2_i[AWIDTH-1:2]] <= dat2_i[15:8];
   always @(posedge clk)
     if(we2_i & en2_i & sel2_i[0])
       ram0[adr2_i[AWIDTH-1:2]] <= dat2_i[7:0];
   
endmodule // dpram32


