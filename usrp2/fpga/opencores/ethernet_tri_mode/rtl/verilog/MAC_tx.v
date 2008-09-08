//////////////////////////////////////////////////////////////////////
////                                                              ////
////  MAC_tx.v                                                    ////
////                                                              ////
////  This file is part of the Ethernet IP core project           ////
////  http://www.opencores.org/projects.cgi/web/ethernet_tri_mode/////
////                                                              ////
////  Author(s):                                                  ////
////      - Jon Gao (gaojon@yahoo.com)                            ////
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
// $Log: MAC_tx.v,v $
// Revision 1.4  2006/11/17 17:53:07  maverickist
// no message
//
// Revision 1.3  2006/01/19 14:07:53  maverickist
// verification is complete.
//
// Revision 1.2  2005/12/16 06:44:14  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.1.1.1  2005/12/13 01:51:44  Administrator
// no message
//   
module MAC_tx(
input           Reset               ,
input           Clk                 ,
input           Clk_user            ,
                //PHY interface
output  [7:0]   TxD                 ,
output          TxEn                ,   
input           CRS                 ,
                //RMON
output  [2:0]   Tx_pkt_type_rmon    ,
output  [15:0]  Tx_pkt_length_rmon  ,
output          Tx_apply_rmon       ,
output  [2:0]   Tx_pkt_err_type_rmon,
                //user interface 
output          Tx_mac_wa           ,
input           Tx_mac_wr           ,
input   [31:0]  Tx_mac_data         ,
input   [1:0]   Tx_mac_BE           ,//big endian
input           Tx_mac_sop          ,
input           Tx_mac_eop          ,
                //host interface 
input   [4:0]   Tx_Hwmark           ,
input   [4:0]   Tx_Lwmark           ,   
input           pause_frame_send_en ,               
input   [15:0]  pause_quanta_set    ,
input           MAC_tx_add_en       ,               
input           FullDuplex          ,
input   [3:0]   MaxRetry            ,
input   [5:0]   IFGset              ,
input   [7:0]   MAC_add_prom_data   ,
input   [2:0]   MAC_add_prom_add    ,
input           MAC_add_prom_wr     ,
input           tx_pause_en         ,
input           xoff_cpu            ,
input           xon_cpu             ,
                //MAC_rx_flow       ,
input   [15:0]  pause_quanta        ,   
input           pause_quanta_val       
);
//******************************************************************************        
//internal signals                                                              
//******************************************************************************   
                //CRC_gen Interface 
wire            CRC_init            ;
wire[7:0]       Frame_data          ;
wire            Data_en             ;
wire            CRC_rd              ;
wire            CRC_end             ;
wire[7:0]       CRC_out             ;
                //Ramdon_gen interface
wire            Random_init         ;
wire[3:0]       RetryCnt            ;
wire            Random_time_meet    ;//levle hight indicate random time passed away
                //flow control
wire            pause_apply         ;
wire            pause_quanta_sub    ;
wire            xoff_gen            ;
wire            xoff_gen_complete   ;
wire            xon_gen             ;
wire            xon_gen_complete    ;               
                //MAC_rx_FF
wire[7:0]       Fifo_data           ;
wire            Fifo_rd             ;
wire            Fifo_eop            ;
wire            Fifo_da             ;
wire            Fifo_rd_finish      ;
wire            Fifo_rd_retry       ;
wire            Fifo_ra             ;
wire            Fifo_data_err_empty ;
wire            Fifo_data_err_full  ;
                //MAC_tx_addr_add
wire            MAC_tx_addr_init    ;
wire            MAC_tx_addr_rd      ;
wire[7:0]       MAC_tx_addr_data    ;

//******************************************************************************        
//instantiation                                                              
//****************************************************************************** 
MAC_tx_ctrl U_MAC_tx_ctrl(
.Reset                    (Reset                  ),                    
.Clk                      (Clk                    ),            
 //CRC_gen Interface      (//CRC_gen Interface    ),           
.CRC_init                 (CRC_init               ),        
.Frame_data               (Frame_data             ),            
.Data_en                  (Data_en                ),            
.CRC_rd                   (CRC_rd                 ),            
.CRC_end                  (CRC_end                ),            
.CRC_out                  (CRC_out                ),            
 //Random_gen interfac    (//Random_gen interfac  ),           
.Random_init              (Random_init            ),            
.RetryCnt                 (RetryCnt               ),        
.Random_time_meet         (Random_time_meet       ),        
 //flow control           (//flow control         ),           
.pause_apply              (pause_apply            ),            
.pause_quanta_sub         (pause_quanta_sub       ),        
.xoff_gen                 (xoff_gen               ),        
.xoff_gen_complete        (xoff_gen_complete      ),            
.xon_gen                  (xon_gen                ),            
.xon_gen_complete         (xon_gen_complete       ),        
 //MAC_tx_FF              (//MAC_tx_FF            ),           
.Fifo_data                (Fifo_data              ),            
.Fifo_rd                  (Fifo_rd                ),            
.Fifo_eop                 (Fifo_eop               ),        
.Fifo_da                  (Fifo_da                ),            
.Fifo_rd_finish           (Fifo_rd_finish         ),            
.Fifo_rd_retry            (Fifo_rd_retry          ),            
.Fifo_ra                  (Fifo_ra                ),            
.Fifo_data_err_empty      (Fifo_data_err_empty    ),            
.Fifo_data_err_full       (Fifo_data_err_full     ),            
 //RMII                   (//RMII                 ),           
.TxD                      (TxD                    ),            
.TxEn                     (TxEn                   ),        
.CRS                      (CRS                    ),            
 //MAC_tx_addr_add        (//MAC_tx_addr_add      ),           
.MAC_tx_addr_rd           (MAC_tx_addr_rd         ),            
.MAC_tx_addr_data         (MAC_tx_addr_data       ),        
.MAC_tx_addr_init         (MAC_tx_addr_init       ),           
 //RMON                   (//RMON                 ),           
.Tx_pkt_type_rmon         (Tx_pkt_type_rmon       ),        
.Tx_pkt_length_rmon       (Tx_pkt_length_rmon     ),            
.Tx_apply_rmon            (Tx_apply_rmon          ),            
.Tx_pkt_err_type_rmon     (Tx_pkt_err_type_rmon   ),           
 //CPU                    (//CPU                  ),           
.pause_frame_send_en      (pause_frame_send_en    ),            
.pause_quanta_set         (pause_quanta_set       ),                
.MAC_tx_add_en            (MAC_tx_add_en          ),            
.FullDuplex               (FullDuplex             ),            
.MaxRetry                 (MaxRetry               ),        
.IFGset                   (IFGset                 )            
);

CRC_gen U_CRC_gen(
.Reset                    (Reset                  ),
.Clk                      (Clk                    ),
.Init                     (CRC_init               ),
.Frame_data               (Frame_data             ),
.Data_en                  (Data_en                ),
.CRC_rd                   (CRC_rd                 ),
.CRC_out                  (CRC_out                ),
.CRC_end                  (CRC_end                )
);

flow_ctrl U_flow_ctrl(
.Reset                    (Reset                  ),
.Clk                      (Clk                    ),
 //host processor         (//host processor       ),
.tx_pause_en              (tx_pause_en            ),
.xoff_cpu                 (xoff_cpu               ),
.xon_cpu                  (xon_cpu                ),
 //MAC_rx_flow            (//MAC_rx_flow          ),
.pause_quanta             (pause_quanta           ),
.pause_quanta_val         (pause_quanta_val       ),
 //MAC_tx_ctrl            (//MAC_tx_ctrl          ),
.pause_apply              (pause_apply            ),
.pause_quanta_sub         (pause_quanta_sub       ),
.xoff_gen                 (xoff_gen               ),
.xoff_gen_complete        (xoff_gen_complete      ),
.xon_gen                  (xon_gen                ),
.xon_gen_complete         (xon_gen_complete       )
);

`ifdef MAC_SOURCE_REPLACE_EN
MAC_tx_addr_add U_MAC_tx_addr_add(
.Reset                    (Reset                  ),
.Clk                      (Clk                    ),
.MAC_tx_addr_rd           (MAC_tx_addr_rd         ),
.MAC_tx_addr_init         (MAC_tx_addr_init       ),
.MAC_tx_addr_data         (MAC_tx_addr_data       ),
 //CPU                    (//CPU                  ),
.MAC_add_prom_data        (MAC_add_prom_data      ),
.MAC_add_prom_add         (MAC_add_prom_add       ),
.MAC_add_prom_wr          (MAC_add_prom_wr        )
);
`else
assign MAC_tx_addr_data=0;
`endif
MAC_tx_FF #(.MAC_TX_FF_DEPTH(9))
  U_MAC_tx_FF(.Reset                    (Reset                  ),
	      .Clk_MAC                  (Clk                    ),
	      .Clk_SYS                  (Clk_user               ),
	      //MAC_rx_ctrl interf     (//MAC_rx_ctrl interf   ),
	      .Fifo_data                (Fifo_data              ),
	      .Fifo_rd                  (Fifo_rd                ),
	      .Fifo_rd_finish           (Fifo_rd_finish         ),
	      .Fifo_rd_retry            (Fifo_rd_retry          ),
	      .Fifo_eop                 (Fifo_eop               ),
	      .Fifo_da                  (Fifo_da                ),
	      .Fifo_ra                  (Fifo_ra                ),
	      .Fifo_data_err_empty      (Fifo_data_err_empty    ),
	      .Fifo_data_err_full       (Fifo_data_err_full     ),
	      //user interface         (//user interface       ),
	      .Tx_mac_wa                (Tx_mac_wa              ),
	      .Tx_mac_wr                (Tx_mac_wr              ),
	      .Tx_mac_data              (Tx_mac_data            ),
	      .Tx_mac_BE                (Tx_mac_BE              ),
	      .Tx_mac_sop               (Tx_mac_sop             ),
	      .Tx_mac_eop               (Tx_mac_eop             ),
	      //host interface         (//host interface       ),
	      .FullDuplex               (FullDuplex             ),
	      .Tx_Hwmark                (Tx_Hwmark              ),
	      .Tx_Lwmark                (Tx_Lwmark              )
	      );

random_gen U_random_gen(
.Reset                    (Reset                  ),
.Clk                      (Clk                    ),
.Init                     (Random_init            ),
.RetryCnt                 (RetryCnt               ),
.Random_time_meet         (Random_time_meet       ) 
);

endmodule
