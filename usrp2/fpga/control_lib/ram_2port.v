

module ram_2port
  #(parameter DWIDTH=32,
    parameter AWIDTH=9)
    (input clka,
     input ena,
     input wea,
     input [AWIDTH-1:0] addra,
     input [DWIDTH-1:0] dia,
     output reg [DWIDTH-1:0] doa,
     
     input clkb,
     input enb,
     input web,
     input [AWIDTH-1:0] addrb,
     input [DWIDTH-1:0] dib,
     output reg [DWIDTH-1:0] dob);
   
   reg [DWIDTH-1:0] ram [(1<<AWIDTH)-1:0];
   integer 	    i;
   initial
     for(i=0;i<(1<<AWIDTH);i=i+1)
       ram[i] <= {AWIDTH{1'b0}};
   
   always @(posedge clka) begin
      if (ena)
        begin
           if (wea)
             ram[addra] <= dia;
           doa <= ram[addra];
        end
   end
   always @(posedge clkb) begin
      if (enb)
        begin
           if (web)
             ram[addrb] <= dib;
           dob <= ram[addrb];
        end
   end
endmodule // ram_2port
