`timescale 1ns / 1ps
// ////////////////////////////////////////////////////////////////////////////////
// Boot CPLD design, only for u2_rev2
// ////////////////////////////////////////////////////////////////////////////////

module boot_cpld
  (input CLK_25MHZ,
   output CLK_25MHZ_EN,
   output [2:0] LED,
   output [8:0] DEBUG,
   input 	 POR,
   
   // To SD Card
   output 	 SD_nCS,
   output 	 SD_Din,
   output 	 SD_CLK,
   input 	 SD_Dout,
   input 	 SD_DAT1,  // Unused
   input 	 SD_DAT2,  // Unused
   input         SD_prot,  // Write Protect
   input         SD_det,   // Card Detect
   
   // To FPGA Config Interface
   input 	 CFG_INIT_B,
   output 	 CFG_Din,     // Also used in Data interface
   output 	 CFG_CCLK,
   input 	 CFG_DONE,
   output 	 CFG_PROG_B,
   
   // To FPGA data interface
   output 	 CPLD_CLK,
   input 	 START,
   input 	 MODE,
   input 	 DONE,
   output 	 detached,
   input         CPLD_misc   // Unused for now
   );
   
   assign 	 CLK_25MHZ_EN = 1'b1;
   
   assign 	 LED[0] = ~CFG_DONE;
   assign 	 LED[1] = CFG_INIT_B;
   assign 	 LED[2] = ~CFG_PROG_B;
   			
   wire 	 en_outs;
   wire [3:0] 	 set_sel = 4'd0;
   
   assign 	 CPLD_CLK = CFG_CCLK;
   assign 	 DEBUG[8:0] = { CLK_25MHZ, SD_nCS, SD_CLK, SD_Din, SD_Dout, 
				START, MODE, DONE, CPLD_misc};

   // Handle cutover to FPGA control of SD
   wire 	 fpga_takeover = ~CPLD_misc;
   wire 	 SD_CLK_int, SD_nCS_int, SD_Din_int, CFG_Din_int;

   assign 	 SD_CLK = fpga_takeover ? START : SD_CLK_int;
   assign 	 SD_nCS = fpga_takeover ? MODE : SD_nCS_int;
   assign 	 SD_Din = fpga_takeover ? DONE : SD_Din_int;
   assign 	 CFG_Din = fpga_takeover ? SD_Dout : CFG_Din_int;

   spi_boot #(.width_set_sel_g(4),  // How many sets (16)
	      .width_bit_cnt_g(6),  // Block length (12 is faster, 6 is minimum)
	      .width_img_cnt_g(2),  // How many images per set
	      .num_bits_per_img_g(20), // Image size, 20 = 1MB
	      .sd_init_g(1),           // SD-specific initialization
	      .mmc_compat_clk_div_g(0),// No MMC support
	      .width_mmc_clk_div_g(0), // No MMC support
	      .reset_level_g(0))       // Active low reset
     
     spi_boot(.clk_i(CLK_25MHZ), 
	      .reset_i(POR),
	      
	      // To SD Card
	      .spi_clk_o(SD_CLK_int), 
	      .spi_cs_n_o(SD_nCS_int),
	      .spi_data_in_i(SD_Dout), 
	      .spi_data_out_o(SD_Din_int), 
	      .spi_en_outs_o(en_outs), 
	      
	      // Data Port
	      .start_i(START),
	      .mode_i(MODE), // 0->conf mode, 1->data mode  
	      .detached_o(detached), 
	      .dat_done_i(DONE), 
	      .set_sel_i(set_sel),
	      
	      // To FPGA
	      .config_n_o(CFG_PROG_B), 
	      .cfg_init_n_i(CFG_INIT_B), 
	      .cfg_done_i(CFG_DONE), 
	      .cfg_clk_o(CFG_CCLK), 
	      .cfg_dat_o(CFG_Din_int)
	      );
   
endmodule // boot_cpld
