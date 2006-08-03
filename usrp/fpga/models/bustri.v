
// Model for tristate bus on altera
// FIXME do we really need to use a megacell for this?

module bustri (data,
	       enabledt,
	       tridata);
   
   input [15:0]  data;
   input 	 enabledt;
   inout [15:0]  tridata;
   
   assign 	 tridata = enabledt ? data :16'bz;
   
endmodule // bustri


