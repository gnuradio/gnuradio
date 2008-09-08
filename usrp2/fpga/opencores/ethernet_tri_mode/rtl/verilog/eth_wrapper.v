module eth_wrapper
  (//system signals
   input           Reset                   ,
   input           Clk_125M                ,
   input           Clk_user                ,
   input           Clk_reg                 ,
   output  [2:0]   Speed                   ,
   //Phy interface         
   output          Gtx_clk                 ,//used only in GMII mode
   input           Rx_clk                  ,
   input           Tx_clk                  ,//used only in MII mode
   output          Tx_er                   ,
   output          Tx_en                   ,
   output  [7:0]   Txd                     ,
   input           Rx_er                   ,
   input           Rx_dv                   ,
   input   [7:0]   Rxd                     ,
   input           Crs                     ,
   input           Col                     ,
   //mdx
   inout           Mdio                    ,// MII Management Data In
   output          Mdc                     , // MII Management Data Clock       

   // Our interfaces to outside
   // Write FIFO Interface
   output [31:0] wr_dat_o,
   output wr_write_o,
   output wr_done_o,
   input wr_ready_i,
   input wr_full_i,
   
   // Read FIFO Interface
   input [31:0] rd_dat_i,
   output rd_read_o,
   output rd_done_o,
   input rd_ready_i,
   input rd_empty_i,

   // Wishbone
   input [31:0] wb_dat_i,
   output [31:0] wb_dat_o,
   input [15:0] wb_adr_i,
   input wb_stb_i,
   input wb_we_i,
   output wb_ack_o
   );

   MAC_top MAC_top
     (//system signals
      .Reset            (Reset),
      .Clk_125M         (Clk_125M),
      .Clk_user         (Clk_user),
      .Clk_reg          (Clk_reg),
      .Speed            (Speed),  // 2:0
      
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
      .Gtx_clk          (Gtx_clk),//used only in GMII mode
      .Rx_clk           (Rx_clk),
      .Tx_clk           (Tx_clk),//used only in MII mode
      .Tx_er            (Tx_er),
      .Tx_en            (Tx_en),
      .Txd              (Txd),
      .Rx_er            (Rx_er),
      .Rx_dv            (Rx_dv),
      .Rxd              (Rxd),
      .Crs              (Crs),
      .Col              (Col),
      
      //host interface
      .CSB              (wb_stb_i),
      .WRB              (wb_we_i & wb_stb_i),
      .CD_in            (wb_dat_i[15:0]),
      .CD_out           (wb_dat_o[15:0]),
      .CA               (wb_adr_i[8:1]),                
      
      //mdx
      .Mdio             (Mdio),// MII Management Data In
      .Mdc              (Mdc) // MII Management Data Clock       
      
      );  

   assign wb_dat_o[31:16] = wb_dat_o[15:0];
   assign wb_ack_o = wb_stb_i;
   
endmodule // eth_wrapper

