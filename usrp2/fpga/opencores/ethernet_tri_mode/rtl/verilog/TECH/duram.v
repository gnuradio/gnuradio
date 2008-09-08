

module duram
  (data_a,
   data_b,
   wren_a,
   wren_b,
   address_a,
   address_b,
   clock_a,
   clock_b,
   q_a,
   q_b);

   parameter DATA_WIDTH = 32; 
   parameter ADDR_WIDTH = 5;  
   parameter ADDR_DEPTH = 1<<ADDR_WIDTH;
   
   input [DATA_WIDTH-1:0] data_a;
   input 		  wren_a;
   input [ADDR_WIDTH-1:0] address_a;
   input 		  clock_a;
   output 		  reg [DATA_WIDTH-1:0] q_a;
   
   input [DATA_WIDTH-1:0] data_b;
   input 		  wren_b;
   input [ADDR_WIDTH-1:0] address_b;
   input 		  clock_b;
   output 		  reg [DATA_WIDTH-1:0] q_b;
   
   reg [DATA_WIDTH-1:0]   ram	[0:ADDR_DEPTH-1];
   
   always @(posedge clock_a)
     begin
	if(wren_a)
	  ram[address_a] <= data_a;
	q_a <= ram[address_a];
     end
   
   always @(posedge clock_b)
     begin
	if(wren_b)
	  ram[address_b] <= data_b;
	q_b <= ram[address_b];
     end
   
endmodule // duram

/*   
   altsyncram 
     U_altsyncram (.wren_a         (wren_a),
		   .wren_b         (wren_b),
		   .data_a         (data_a),
		   .data_b         (data_b),
		   .address_a      (address_a),
		   .address_b      (address_b),
		   .clock0         (clock_a),
		   .clock1         (clock_b),
		   .q_a            (q_a),
		   .q_b            (q_b),
		   // synopsys translate_off
		   .aclr0 (),
		   .aclr1 (),
		   .addressstall_a (),
		   .addressstall_b (),
		   .byteena_a (),
		   .byteena_b (),
		   .clocken0 (),
		   .clocken1 (),
		   .rden_b ()
		   // synopsys translate_on
		   );
 
 //parameter  BLK_RAM_TYPE = "AUTO";
 //parameter DURAM_MODE = "BIDIR_DUAL_PORT";
   
 defparam 		    
 U_altsyncram.intended_device_family = "Stratix",
 U_altsyncram.ram_block_type = BLK_RAM_TYPE,
 U_altsyncram.operation_mode = DURAM_MODE,
 U_altsyncram.width_a = DATA_WIDTH,
 U_altsyncram.widthad_a = ADDR_WIDTH,
 U_altsyncram.width_b = DATA_WIDTH,
 U_altsyncram.widthad_b = ADDR_WIDTH,
 U_altsyncram.lpm_type = "altsyncram",
 U_altsyncram.width_byteena_a = 1,
 U_altsyncram.width_byteena_b = 1,
 U_altsyncram.outdata_reg_a = "UNREGISTERED",
 U_altsyncram.outdata_aclr_a = "NONE",
 U_altsyncram.outdata_reg_b = "UNREGISTERED",
 U_altsyncram.indata_aclr_a = "NONE",
 U_altsyncram.wrcontrol_aclr_a = "NONE",
 U_altsyncram.address_aclr_a = "NONE",
 U_altsyncram.indata_reg_b = "CLOCK1",
 U_altsyncram.address_reg_b = "CLOCK1",
 U_altsyncram.wrcontrol_wraddress_reg_b = "CLOCK1",
 U_altsyncram.indata_aclr_b = "NONE",
 U_altsyncram.wrcontrol_aclr_b = "NONE",
 U_altsyncram.address_aclr_b = "NONE",
 U_altsyncram.outdata_aclr_b = "NONE",
 U_altsyncram.power_up_uninitialized = "FALSE";

 */
