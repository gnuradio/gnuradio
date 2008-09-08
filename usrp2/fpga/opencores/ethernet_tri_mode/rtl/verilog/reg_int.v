module reg_int (
input                   Reset                   ,
input                   Clk_reg                 ,
input                   CSB                     ,
input                   WRB                     ,
input           [15:0]  CD_in                   ,
output   reg    [15:0]  CD_out                  ,
input           [7:0]   CA                      ,
                        //Tx host interface 
output          [4:0]   Tx_Hwmark               ,
output          [4:0]   Tx_Lwmark               ,   
output                  pause_frame_send_en     ,               
output          [15:0]  pause_quanta_set        ,
output                  MAC_tx_add_en           ,               
output                  FullDuplex              ,
output          [3:0]   MaxRetry                ,
output          [5:0]   IFGset                  ,
output          [7:0]   MAC_tx_add_prom_data    ,
output          [2:0]   MAC_tx_add_prom_add     ,
output                  MAC_tx_add_prom_wr      ,
output                  tx_pause_en             ,
output                  xoff_cpu                ,
output                  xon_cpu                 ,
                        //Rx host interface     
output                  MAC_rx_add_chk_en       ,   
output          [7:0]   MAC_rx_add_prom_data    ,   
output          [2:0]   MAC_rx_add_prom_add     ,   
output                  MAC_rx_add_prom_wr      ,   
output                  broadcast_filter_en     ,
output          [15:0]  broadcast_bucket_depth              ,
output          [15:0]  broadcast_bucket_interval           ,
output                  RX_APPEND_CRC           ,
output          [4:0]   Rx_Hwmark           ,
output          [4:0]   Rx_Lwmark           ,
output                  CRC_chk_en              ,               
output          [5:0]   RX_IFG_SET              ,
output          [15:0]  RX_MAX_LENGTH           ,// 1518
output          [6:0]   RX_MIN_LENGTH           ,// 64
                        //RMON host interface
output          [5:0]   CPU_rd_addr             ,
output                  CPU_rd_apply            ,
input                   CPU_rd_grant            ,
input           [31:0]  CPU_rd_dout             ,
                        //Phy int host interface     
output                  Line_loop_en            ,
output          [2:0]   Speed                   ,
                        //MII to CPU 
output          [7:0]   Divider                 ,// Divider for the host clock
output          [15:0]  CtrlData                ,// Control Data (to be written to the PHY reg.)
output          [4:0]   Rgad                    ,// Register Address (within the PHY)
output          [4:0]   Fiad                    ,// PHY Address
output                  NoPre                   ,// No Preamble (no 32-bit preamble)
output                  WCtrlData               ,// Write Control Data operation
output                  RStat                   ,// Read Status operation
output                  ScanStat                ,// Scan Status operation
input                   Busy                    ,// Busy Signal
input                   LinkFail                ,// Link Integrity Signal
input                   Nvalid                  ,// Invalid Status (qualifier for the valid scan result)
input           [15:0]  Prsd                    ,// Read Status Data (data read from the PHY)
input                   WCtrlDataStart          ,// This signals resets the WCTRLDATA bit in the MIIM Command register
input                   RStatStart              ,// This signal resets the RSTAT BIT in the MIIM Command register
input                   UpdateMIIRX_DATAReg     // Updates MII RX_DATA register with read data
);

    RegCPUData U_0_000(Tx_Hwmark                ,7'd000,16'h0009,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_001(Tx_Lwmark                ,7'd001,16'h0008,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_002(pause_frame_send_en      ,7'd002,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_003(pause_quanta_set         ,7'd003,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_004(IFGset                   ,7'd004,16'h000c,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_005(FullDuplex               ,7'd005,16'h0001,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_006(MaxRetry                 ,7'd006,16'h0002,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_007(MAC_tx_add_en            ,7'd007,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_008(MAC_tx_add_prom_data     ,7'd008,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_009(MAC_tx_add_prom_add      ,7'd009,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_010(MAC_tx_add_prom_wr       ,7'd010,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_011(tx_pause_en              ,7'd011,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_012(xoff_cpu                 ,7'd012,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_013(xon_cpu                  ,7'd013,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_014(MAC_rx_add_chk_en        ,7'd014,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_015(MAC_rx_add_prom_data     ,7'd015,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_016(MAC_rx_add_prom_add      ,7'd016,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_017(MAC_rx_add_prom_wr       ,7'd017,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_018(broadcast_filter_en      ,7'd018,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_019(broadcast_bucket_depth   ,7'd019,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_020(broadcast_bucket_interval,7'd020,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_021(RX_APPEND_CRC            ,7'd021,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_022(Rx_Hwmark                ,7'd022,16'h001a,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_023(Rx_Lwmark                ,7'd023,16'h0010,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_024(CRC_chk_en               ,7'd024,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_025(RX_IFG_SET               ,7'd025,16'h000c,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_026(RX_MAX_LENGTH            ,7'd026,16'h2710,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_027(RX_MIN_LENGTH            ,7'd027,16'h0040,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_028(CPU_rd_addr              ,7'd028,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_029(CPU_rd_apply             ,7'd029,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
//  RegCPUData U_0_030(CPU_rd_grant             ,7'd030,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
//  RegCPUData U_0_031(CPU_rd_dout_l            ,7'd031,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
//  RegCPUData U_0_032(CPU_rd_dout_h            ,7'd032,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_033(Line_loop_en             ,7'd033,16'h0000,Reset,Clk_reg,!WRB,CSB,CA,CD_in);
    RegCPUData U_0_034(Speed                    ,7'd034,16'h0004,Reset,Clk_reg,!WRB,CSB,CA,CD_in);

always @ (posedge Clk_reg or posedge Reset)
    if (Reset)
        CD_out  <=0;
    else if (!CSB&&WRB)
        case (CA[7:1])
                7'd00:    CD_out<=Tx_Hwmark                  ;
                7'd01:    CD_out<=Tx_Lwmark                  ; 
                7'd02:    CD_out<=pause_frame_send_en        ; 
                7'd03:    CD_out<=pause_quanta_set           ;
                7'd04:    CD_out<=IFGset                     ; 
                7'd05:    CD_out<=FullDuplex                 ; 
                7'd06:    CD_out<=MaxRetry                   ;
                7'd07:    CD_out<=MAC_tx_add_en              ; 
                7'd08:    CD_out<=MAC_tx_add_prom_data       ;
                7'd09:    CD_out<=MAC_tx_add_prom_add        ; 
                7'd10:    CD_out<=MAC_tx_add_prom_wr         ; 
                7'd11:    CD_out<=tx_pause_en                ; 
                7'd12:    CD_out<=xoff_cpu                   ;
                7'd13:    CD_out<=xon_cpu                    ; 
                7'd14:    CD_out<=MAC_rx_add_chk_en          ; 
                7'd15:    CD_out<=MAC_rx_add_prom_data       ;
                7'd16:    CD_out<=MAC_rx_add_prom_add        ; 
                7'd17:    CD_out<=MAC_rx_add_prom_wr         ; 
                7'd18:    CD_out<=broadcast_filter_en        ; 
                7'd19:    CD_out<=broadcast_bucket_depth     ;    
                7'd20:    CD_out<=broadcast_bucket_interval  ;   
                7'd21:    CD_out<=RX_APPEND_CRC              ; 
                7'd22:    CD_out<=Rx_Hwmark                  ; 
                7'd23:    CD_out<=Rx_Lwmark                  ; 
                7'd24:    CD_out<=CRC_chk_en                 ; 
                7'd25:    CD_out<=RX_IFG_SET                 ; 
                7'd26:    CD_out<=RX_MAX_LENGTH              ; 
                7'd27:    CD_out<=RX_MIN_LENGTH              ; 
                7'd28:    CD_out<=CPU_rd_addr                ; 
                7'd29:    CD_out<=CPU_rd_apply               ;
                7'd30:    CD_out<=CPU_rd_grant               ;
                7'd31:    CD_out<=CPU_rd_dout[15:0]          ; 
                7'd32:    CD_out<=CPU_rd_dout[31:16]         ;                 
                7'd33:    CD_out<=Line_loop_en               ;
                7'd34:    CD_out<=Speed                      ; 
                default:  CD_out<=0                          ;
        endcase
    else
        CD_out<=0                          ;

endmodule   

module RegCPUData(
RegOut,   
CA_reg_set, 
RegInit,  
          
Reset,    
Clk,      
CWR_pulse,
CCSB,
CA_reg,     
CD_in_reg
);
output[15:0]    RegOut; 
input[6:0]      CA_reg_set;  
input[15:0]     RegInit;
//
input           Reset;
input           Clk;
input           CWR_pulse;
input           CCSB;
input[7:0]      CA_reg;
input[15:0]     CD_in_reg;
// 
reg[15:0]       RegOut; 

always  @(posedge Reset or posedge Clk)
    if(Reset)
        RegOut      <=RegInit;
    else if (CWR_pulse && !CCSB && CA_reg[7:1] ==CA_reg_set[6:0])  
        RegOut      <=CD_in_reg;

endmodule           
