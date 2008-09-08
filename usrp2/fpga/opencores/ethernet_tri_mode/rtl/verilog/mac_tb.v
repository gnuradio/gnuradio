module mac_tb();

MAC_top MAC_top
  (//system signals
   .Reset            (),
   .Clk_125M         (),
   .Clk_user         (),
   .Clk_reg          (),
   .Speed            (),  // 2:0

   // RX interface 
   .Rx_mac_ra        (),
   .Rx_mac_rd        (),
   .Rx_mac_data      (),
   .Rx_mac_BE        (),
   .Rx_mac_pa        (),
   .Rx_mac_sop       (),
   .Rx_mac_eop       (),

   //TX interface 
   .Tx_mac_wa        (),
   .Tx_mac_wr        (),
   .Tx_mac_data      (),
   .Tx_mac_BE        (),//big endian
   .Tx_mac_sop       (),
   .Tx_mac_eop       (),

   //Phy interface         
   .Gtx_clk          (),//used only in GMII mode
   .Rx_clk           (),
   .Tx_clk           (),//used only in MII mode
   .Tx_er            (),
   .Tx_en            (),
   .Txd              (),
   .Rx_er            (),
   .Rx_dv            (),
   .Rxd              (),
   .Crs              (),
   .Col              (),

   //host interface
   .CSB              (),
   .WRB              (),
   .CD_in            (),
   .CD_out           (),
   .CA               (),                

   //mdx
   .Mdio             (),// MII Management Data In
   .Mdc              () // MII Management Data Clock       
   
   );  


endmodule // mac_tb
