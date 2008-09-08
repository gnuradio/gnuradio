
// Model of the Xilinx mult18x18s for signed 18x18 bit multiplies,
// As in the Spartan 3 series

module MULT18X18S
  (output reg signed [35:0] P,
   input signed [17:0] A,
   input signed [17:0] B,
   input C,    // Clock
   input CE,   // Clock Enable
   input R     // Synchronous Reset
   );
   
   always @(posedge C)
     if(R)
       P <= 36'sd0;
     else if(CE)
       P <= A * B;

endmodule // MULT18X18S
