`timescale 1 ns/100ps
//////////////////////////////////////////////////////////////////////
////                                                              ////
////  tb_top.v                                                    ////
////                                                              ////
////  This file is part of the Ethernet IP core project           ////
////  http://www.opencores.org/projects.cgi/web/ethernet_tri_mode/////
////                                                              ////
////  Author(s):                                                  ////
////      - Jon Gao (gaojon@yahoo.com)                      	  ////
////                                                              ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2001 Authors                                   ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE.  See the GNU Lesser General Public License for more ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from http://www.opencores.org/lgpl.shtml                     ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
//                                                                    
// CVS Revision History                                               
//                                                                    
// $Log: tb_top.v,v $
// Revision 1.3  2006/01/19 14:07:51  maverickist
// verification is complete.
//
// Revision 1.2  2005/12/16 06:44:13  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.1.1.1  2005/12/13 01:51:44  Administrator
// no message
// 

module tb_top ();
   // ******************************************************************************
   //internal signals                                                              
   // ******************************************************************************
   //system signals

   reg				Reset					;
   reg				Clk_125M				;
   reg				Clk_user				;
   reg				Clk_reg					;
   //user interface 
   wire 			Rx_mac_ra				;
   wire 			Rx_mac_rd				;
   wire [31:0] 			Rx_mac_data				;
   wire [1:0] 			Rx_mac_BE				;
   wire 			Rx_mac_pa				;
   wire 			Rx_mac_sop				;
   wire 			Rx_mac_eop				;
   //user interface 
   wire 			Tx_mac_wa	        	;
   wire 			Tx_mac_wr	        	;
   wire [31:0] 			Tx_mac_data	        	;
   wire [1:0] 			Tx_mac_BE				;//big endian
   wire 			Tx_mac_sop	        	;
   wire 			Tx_mac_eop				;
   //Phy interface     	 
   //Phy interface			
   wire 			Gtx_clk					;//used only in GMII mode
   wire 			Rx_clk					;
   wire 			Tx_clk					;//used only in MII mode
   wire 			Tx_er					;
   wire 			Tx_en					;
   wire [7:0] 			Txd						;
   wire 			Rx_er					;
   wire 			Rx_dv					;
   wire [7:0] 			Rxd						;
   wire 			Crs						;
   wire 			Col						;
   wire 			CSB                     ;
   wire 			WRB                     ;
   wire [15:0] 			CD_in                   ;
   wire [15:0] 			CD_out                  ;
   wire [7:0] 			CA                      ;				
   //Phy int host interface     
   wire 			Line_loop_en			;
   wire [2:0] 			Speed					;
   //mii
   wire 			Mdio                	;// MII Management Data In
   wire 			Mdc                		;// MII Management Data Clock	
   wire 			CPU_init_end            ;
   // ******************************************************************************
   //internal signals                                                              
   // ******************************************************************************
   
   initial 
     begin
	Reset	=1;
	#20		Reset	=0;
     end
   always 
     begin
	#4		Clk_125M=0;
	#4		Clk_125M=1;
     end
   
   always 
     begin
	#5		Clk_user=0;
	#5		Clk_user=1;
     end
   
   always 
     begin
	#10		Clk_reg=0;
	#10		Clk_reg=1;
     end
   
//   initial	
  //   begin
	//$shm_open("tb_top.shm",,900000000,);
//	$shm_probe("AS");
 //    end

   MAC_top 
     U_MAC_top(//system signals     			(//system signals           ),
	       .Reset					        (Reset					    ),
	       .Clk_125M				        (Clk_125M				    ),
	       .Clk_user				        (Clk_user				    ),
	       .Clk_reg					    (Clk_reg					),
	       .Speed                          (Speed                      ),
	       //user interface               (//user interface           ),
	       .Rx_mac_ra				        (Rx_mac_ra				    ),
	       .Rx_mac_rd				        (Rx_mac_rd				    ),
	       .Rx_mac_data				    (Rx_mac_data				),
	       .Rx_mac_BE				        (Rx_mac_BE				    ),
	       .Rx_mac_pa				        (Rx_mac_pa				    ),
	       .Rx_mac_sop				        (Rx_mac_sop				    ),
	       .Rx_mac_eop				        (Rx_mac_eop				    ),
	       //user interface               (//user interface           ),
	       .Tx_mac_wa	        	        (Tx_mac_wa	        	    ),
	       .Tx_mac_wr	        	        (Tx_mac_wr	        	    ),
	       .Tx_mac_data	        	    (Tx_mac_data	        	),
	       .Tx_mac_BE				        (Tx_mac_BE				    ),
	       .Tx_mac_sop	        	        (Tx_mac_sop	        	    ),
	       .Tx_mac_eop				        (Tx_mac_eop				    ),
	       //Phy interface     	        (//Phy interface     	    ),
	       //Phy interface			    (//Phy interface			),
	       .Gtx_clk					    (Gtx_clk					),
	       .Rx_clk					        (Rx_clk					    ),
	       .Tx_clk					        (Tx_clk					    ),
	       .Tx_er					        (Tx_er					    ),
	       .Tx_en					        (Tx_en					    ),
	       .Txd						    (Txd	   		),
	       .Rx_er					        (Rx_er					    ),
	       .Rx_dv					        (Rx_dv			    ),
	       .Rxd					        (Rxd				),
	       .Crs					        (Crs				),
	       .Col					        (Col				),
	       //host interface
	       .CSB                            (CSB                        ),
	       .WRB                            (WRB                        ),
	       .CD_in                          (CD_in                      ),
	       .CD_out                         (CD_out                     ),
	       .CA                             (CA                         ),
	       //MII interface signals        (//MII interface signals    ),
	       .Mdio                	        (Mdio                	    ),
	       .Mdc                		    (Mdc                		)
	       );
   
   Phy_sim 
     U_Phy_sim (.Gtx_clk						(Gtx_clk	       	),
		.Rx_clk		                    (Rx_clk		                ),
		.Tx_clk		                    (Tx_clk		                ),
		.Tx_er		                    (Tx_er		                ),
		.Tx_en		                    (Tx_en		                ),
		.Txd			                (Txd			            ),
		.Rx_er		                    (Rx_er		                ),
		.Rx_dv		                    (Rx_dv		                ),
		.Rxd			                (Rxd			            ),
		.Crs			                (Crs			            ),
		.Col			                (Col			            ),
		.Speed		                    (Speed		                )
		);
   
   User_int_sim 
     U_User_int_sim(.Reset							(Reset			),
		    .Clk_user			            (Clk_user			       ),
		    .CPU_init_end                   (CPU_init_end               ),
		    //user inputerface             (//user inputerface         ),
		    .Rx_mac_ra			            (Rx_mac_ra			        ),
		    .Rx_mac_rd			            (Rx_mac_rd			        ),
		    .Rx_mac_data			        (Rx_mac_data			    ),
		    .Rx_mac_BE			            (Rx_mac_BE			        ),
		    .Rx_mac_pa			            (Rx_mac_pa			        ),
		    .Rx_mac_sop			            (Rx_mac_sop			        ),
		    .Rx_mac_eop			            (Rx_mac_eop			        ),
		    //user inputerface             (//user inputerface         ),
		    .Tx_mac_wa	                    (Tx_mac_wa	                ),
		    .Tx_mac_wr	                    (Tx_mac_wr	                ),
		    .Tx_mac_data	                (Tx_mac_data	            ),
		    .Tx_mac_BE			            (Tx_mac_BE			        ),
		    .Tx_mac_sop	                    (Tx_mac_sop	                ),
		    .Tx_mac_eop			            (Tx_mac_eop			        )
		    );
   
   host_sim 
     U_host_sim(.Reset	               			(Reset	                  	),    
		.Clk_reg                  		(Clk_reg                 	), 
		.CSB                            (CSB                        ),
		.WRB                            (WRB                        ),
		.CD_in                          (CD_in                      ),
		.CD_out                         (CD_out                     ),
		.CPU_init_end                   (CPU_init_end               ),
		.CA                             (CA                         )
		);

endmodule // tb_top
