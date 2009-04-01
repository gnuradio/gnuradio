
module crc
  (input clk, 
   input reset,
   input clear, 
   input [7:0] data,
   input calc,
   output [31:0] crc_out,
   output match);
   
   function[31:0]  NextCRC;
      input[7:0]      D;
      input[31:0]     C;
      reg[31:0]       NewCRC;
      begin
	 NewCRC[0]   = C[24]^C[30]^D[1]^D[7];
	 NewCRC[1]   = C[25]^C[31]^D[0]^D[6]^C[24]^C[30]^D[1]^D[7];
	 NewCRC[2]   = C[26]^D[5]^C[25]^C[31]^D[0]^D[6]^C[24]^C[30]^D[1]^D[7];
	 NewCRC[3]   = C[27]^D[4]^C[26]^D[5]^C[25]^C[31]^D[0]^D[6];
	 NewCRC[4]   = C[28]^D[3]^C[27]^D[4]^C[26]^D[5]^C[24]^C[30]^D[1]^D[7];
	 NewCRC[5]   = C[29]^D[2]^C[28]^D[3]^C[27]^D[4]^C[25]^C[31]^D[0]^D[6]^C[24]^C[30]^D[1]^D[7];
	 NewCRC[6]   = C[30]^D[1]^C[29]^D[2]^C[28]^D[3]^C[26]^D[5]^C[25]^C[31]^D[0]^D[6];
	 NewCRC[7]   = C[31]^D[0]^C[29]^D[2]^C[27]^D[4]^C[26]^D[5]^C[24]^D[7];
	 NewCRC[8]   = C[0]^C[28]^D[3]^C[27]^D[4]^C[25]^D[6]^C[24]^D[7];
	 NewCRC[9]   = C[1]^C[29]^D[2]^C[28]^D[3]^C[26]^D[5]^C[25]^D[6];
	 NewCRC[10]  = C[2]^C[29]^D[2]^C[27]^D[4]^C[26]^D[5]^C[24]^D[7];
	 NewCRC[11]  = C[3]^C[28]^D[3]^C[27]^D[4]^C[25]^D[6]^C[24]^D[7];
	 NewCRC[12]  = C[4]^C[29]^D[2]^C[28]^D[3]^C[26]^D[5]^C[25]^D[6]^C[24]^C[30]^D[1]^D[7];
	 NewCRC[13]  = C[5]^C[30]^D[1]^C[29]^D[2]^C[27]^D[4]^C[26]^D[5]^C[25]^C[31]^D[0]^D[6];
	 NewCRC[14]  = C[6]^C[31]^D[0]^C[30]^D[1]^C[28]^D[3]^C[27]^D[4]^C[26]^D[5];
	 NewCRC[15]  = C[7]^C[31]^D[0]^C[29]^D[2]^C[28]^D[3]^C[27]^D[4];
	 NewCRC[16]  = C[8]^C[29]^D[2]^C[28]^D[3]^C[24]^D[7];
	 NewCRC[17]  = C[9]^C[30]^D[1]^C[29]^D[2]^C[25]^D[6];
	 NewCRC[18]  = C[10]^C[31]^D[0]^C[30]^D[1]^C[26]^D[5];
	 NewCRC[19]  = C[11]^C[31]^D[0]^C[27]^D[4];
	 NewCRC[20]  = C[12]^C[28]^D[3];
	 NewCRC[21]  = C[13]^C[29]^D[2];
	 NewCRC[22]  = C[14]^C[24]^D[7];
	 NewCRC[23]  = C[15]^C[25]^D[6]^C[24]^C[30]^D[1]^D[7];
	 NewCRC[24]  = C[16]^C[26]^D[5]^C[25]^C[31]^D[0]^D[6];
	 NewCRC[25]  = C[17]^C[27]^D[4]^C[26]^D[5];
	 NewCRC[26]  = C[18]^C[28]^D[3]^C[27]^D[4]^C[24]^C[30]^D[1]^D[7];
	 NewCRC[27]  = C[19]^C[29]^D[2]^C[28]^D[3]^C[25]^C[31]^D[0]^D[6];
	 NewCRC[28]  = C[20]^C[30]^D[1]^C[29]^D[2]^C[26]^D[5];
	 NewCRC[29]  = C[21]^C[31]^D[0]^C[30]^D[1]^C[27]^D[4];
	 NewCRC[30]  = C[22]^C[31]^D[0]^C[28]^D[3];
	 NewCRC[31]  = C[23]^C[29]^D[2];
	 NextCRC     = NewCRC;
      end
   endfunction

   reg [31:0] crc_reg;
   always @ (posedge clk)
     if (reset | clear)
       crc_reg 	    <= 32'hffffffff;
     else if (calc)
       crc_reg 	    <= NextCRC(data,crc_reg);
        
   assign crc_out    = ~{crc_reg[24],crc_reg[25],crc_reg[26],crc_reg[27],crc_reg[28],crc_reg[29],crc_reg[30],crc_reg[31],
		      crc_reg[16],crc_reg[17],crc_reg[18],crc_reg[19],crc_reg[20],crc_reg[21],crc_reg[22],crc_reg[23],
		      crc_reg[8],crc_reg[9],crc_reg[10],crc_reg[11],crc_reg[12],crc_reg[13],crc_reg[14],crc_reg[15],
		      crc_reg[0],crc_reg[1],crc_reg[2],crc_reg[3],crc_reg[4],crc_reg[5],crc_reg[6],crc_reg[7] };

   assign match  = (crc_reg == 32'hc704_dd7b);
		    
endmodule // crc
