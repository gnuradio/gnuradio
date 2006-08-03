// Bidirectional registers

module bidir_reg
  ( inout wire [15:0] tristate,
    input wire [15:0] oe,
    input wire [15:0] reg_val );

   // This would be much cleaner if all the tools
   // supported "for generate"........

   assign 	 tristate[0] = oe[0] ? reg_val[0] : 1'bz;
   assign 	 tristate[1] = oe[1] ? reg_val[1] : 1'bz;
   assign 	 tristate[2] = oe[2] ? reg_val[2] : 1'bz;
   assign 	 tristate[3] = oe[3] ? reg_val[3] : 1'bz;
   assign 	 tristate[4] = oe[4] ? reg_val[4] : 1'bz;
   assign 	 tristate[5] = oe[5] ? reg_val[5] : 1'bz;
   assign 	 tristate[6] = oe[6] ? reg_val[6] : 1'bz;
   assign 	 tristate[7] = oe[7] ? reg_val[7] : 1'bz;
   assign 	 tristate[8] = oe[8] ? reg_val[8] : 1'bz;
   assign 	 tristate[9] = oe[9] ? reg_val[9] : 1'bz;
   assign 	 tristate[10] = oe[10] ? reg_val[10] : 1'bz;
   assign 	 tristate[11] = oe[11] ? reg_val[11] : 1'bz;
   assign 	 tristate[12] = oe[12] ? reg_val[12] : 1'bz;
   assign 	 tristate[13] = oe[13] ? reg_val[13] : 1'bz;
   assign 	 tristate[14] = oe[14] ? reg_val[14] : 1'bz;
   assign 	 tristate[15] = oe[15] ? reg_val[15] : 1'bz;
   
endmodule // bidir_reg

