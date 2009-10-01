
module wb_reg
  #(parameter ADDR=0,
    parameter DEFAULT=0)
   (input clk, input rst, 
    input [5:0] adr, input wr_acc,
    input [31:0] dat_i, output reg [31:0] dat_o);

   always @(posedge clk)
     if(rst)
       dat_o <= DEFAULT;
     else if(wr_acc & (adr == ADDR))
       dat_o <= dat_i;
   
endmodule // wb_reg

   

module simple_gemac_wb
  (input wb_clk, input wb_rst,
   input wb_cyc, input wb_stb, output reg wb_ack, input wb_we,
   input [7:0] wb_adr, input [31:0] wb_dat_i, output reg [31:0] wb_dat_o,
   
   inout mdio, output mdc,
   output [47:0] ucast_addr, output [47:0] mcast_addr,
   output pass_ucast, output pass_mcast, output pass_bcast,
   output pass_pause, output pass_all, 
   output pause_respect_en, output pause_request_en, 
   output [15:0] pause_time, output [15:0] pause_thresh  );

   wire   acc 	  = wb_cyc & wb_stb;
   wire   wr_acc  = wb_cyc & wb_stb & wb_we;
   wire   rd_acc  = wb_cyc & wb_stb & ~wb_we;

   always @(posedge wb_clk)
     if(wb_rst)
       wb_ack 	 <= 0;
     else
       wb_ack 	 <= acc & ~wb_ack;
       
   wire [6:0] misc_settings;
   assign {pause_request_en, pass_ucast, pass_mcast, pass_bcast, pass_pause, pass_all, pause_respect_en} = misc_settings;

   wb_reg #(.ADDR(0),.DEFAULT(7'b0111001))
   wb_reg_settings (.clk(wb_clk), .rst(wb_rst), .adr(wb_adr[7:2]), .wr_acc(wr_acc),
		    .dat_i(wb_dat_i), .dat_o(misc_settings) );
   wb_reg #(.ADDR(1),.DEFAULT(0))
   wb_reg_ucast_h (.clk(wb_clk), .rst(wb_rst), .adr(wb_adr[7:2]), .wr_acc(wr_acc),
		   .dat_i(wb_dat_i), .dat_o(ucast_addr[47:32]) );
   wb_reg #(.ADDR(2),.DEFAULT(0))
   wb_reg_ucast_l (.clk(wb_clk), .rst(wb_rst), .adr(wb_adr[7:2]), .wr_acc(wr_acc),
		   .dat_i(wb_dat_i), .dat_o(ucast_addr[31:0]) );
   wb_reg #(.ADDR(3),.DEFAULT(0))
   wb_reg_mcast_h (.clk(wb_clk), .rst(wb_rst), .adr(wb_adr[7:2]), .wr_acc(wr_acc),
		   .dat_i(wb_dat_i), .dat_o(mcast_addr[47:32]) );
   wb_reg #(.ADDR(4),.DEFAULT(0))
   wb_reg_mcast_l (.clk(wb_clk), .rst(wb_rst), .adr(wb_adr[7:2]), .wr_acc(wr_acc),
		   .dat_i(wb_dat_i), .dat_o(mcast_addr[31:0]) );

   //MII to CPU 
   wire [7:0] Divider;            // Divider for the host clock
   wire [15:0] CtrlData;          // Control Data (to be written to the PHY reg.)
   wire [4:0]  Rgad;               // Register Address (within the PHY)
   wire [4:0]  Fiad;               // PHY Address
   wire        NoPre;              // No Preamble (no 32-bit preamble)
   wire        WCtrlData;          // Write Control Data operation
   wire        RStat;              // Read Status operation
   wire        ScanStat;           // Scan Status operation
   wire        Busy;               // Busy Signal
   wire        LinkFail;           // Link Integrity Signal
   wire        Nvalid;             // Invalid Status (qualifier for the valid scan result)
   wire [15:0] Prsd;               // Read Status Data (data read from the PHY)
   wire        WCtrlDataStart;     // This signals resets the WCTRLDATA bit in the MIIM Command register
   wire        RStatStart;         // This signal resets the RSTAT BIT in the MIIM Command register
   wire        UpdateMIIRX_DATAReg; // Updates MII RX_DATA register with read data
   
   // registers for controlling the MII interface
   reg [2:0]   MIICOMMAND;
   wire [12:0] MIIADDRESS;
   reg [15:0]  MIIRX_DATA;
   wire [2:0]  MIISTATUS;

   wb_reg #(.ADDR(5),.DEFAULT(0))
   wb_reg_miimoder (.clk(wb_clk), .rst(wb_rst), .adr(wb_adr[7:2]), .wr_acc(wr_acc),
		    .dat_i(wb_dat_i), .dat_o({NoPre,Divider}) );
   
   wb_reg #(.ADDR(6),.DEFAULT(0))
   wb_reg_miiaddr (.clk(wb_clk), .rst(wb_rst), .adr(wb_adr[7:2]), .wr_acc(wr_acc),
		   .dat_i(wb_dat_i), .dat_o(MIIADDRESS) );
   
   wb_reg #(.ADDR(7),.DEFAULT(0))
   wb_reg_miidata (.clk(wb_clk), .rst(wb_rst), .adr(wb_adr[7:2]), .wr_acc(wr_acc),
		   .dat_i(wb_dat_i), .dat_o(CtrlData) );
   
   // MIICOMMAND register - needs special treatment because of auto-resetting bits
   always @ (posedge wb_clk)
     if (wb_rst)
       MIICOMMAND <= 0;
     else
       if (wr_acc & (wb_adr[7:2] == 6'd8))
         MIICOMMAND <= wb_dat_i;
       else
         begin
            if ( WCtrlDataStart )
              MIICOMMAND[2] <= 0;
            if ( RStatStart )
              MIICOMMAND[1] <= 0;
         end
   
   // MIIRX_DATA register
   always @(posedge wb_clk)
     if (wb_rst)
       MIIRX_DATA <= 0;
     else
       if (UpdateMIIRX_DATAReg )
         MIIRX_DATA <= Prsd;

   // MIICOMMAND
   assign WCtrlData  = MIICOMMAND[2];
   assign RStat      = MIICOMMAND[1];
   assign ScanStat   = MIICOMMAND[0];
   // MIIADDRESS
   assign Rgad 	     = MIIADDRESS[12:8];
   assign Fiad 	     = MIIADDRESS[4:0];
   // MIISTATUS
   assign MIISTATUS[2:0] = { Nvalid, Busy, LinkFail };
   
   eth_miim eth_miim
     (.Clk(wb_clk), .Reset(wb_rst), 
      .Divider(Divider), .NoPre(NoPre), .CtrlData(CtrlData), .Rgad(Rgad), .Fiad(Fiad), 
      .WCtrlData(WCtrlData), .RStat(RStat), .ScanStat(ScanStat), .Mdio(mdio), .Mdc(mdc), 
      .Busy(Busy), .Prsd(Prsd), .LinkFail(LinkFail), .Nvalid(Nvalid), 
      .WCtrlDataStart(WCtrlDataStart), .RStatStart(RStatStart), 
      .UpdateMIIRX_DATAReg(UpdateMIIRX_DATAReg) );

   wb_reg #(.ADDR(11),.DEFAULT(0))
   wb_reg_pausetime (.clk(wb_clk), .rst(wb_rst), .adr(wb_adr[7:2]), .wr_acc(wr_acc),
		     .dat_i(wb_dat_i), .dat_o(pause_time) );
   
   wb_reg #(.ADDR(12),.DEFAULT(0))
   wb_reg_pausethresh (.clk(wb_clk), .rst(wb_rst), .adr(wb_adr[7:2]), .wr_acc(wr_acc),
		       .dat_i(wb_dat_i), .dat_o(pause_thresh) );
   
   always @(posedge wb_clk)
     case(wb_adr[7:2])
       0 : wb_dat_o <= misc_settings;
       1 : wb_dat_o <= ucast_addr[47:32];
       2 : wb_dat_o <= ucast_addr[31:0];
       3 : wb_dat_o <= mcast_addr[47:32];
       4 : wb_dat_o <= mcast_addr[31:0];
       5 : wb_dat_o <= {NoPre,Divider};
       6 : wb_dat_o <= MIIADDRESS;
       7 : wb_dat_o <= CtrlData;
       8 : wb_dat_o <= MIICOMMAND;
       9 : wb_dat_o <= MIISTATUS;
       10: wb_dat_o <= MIIRX_DATA;
       11: wb_dat_o <= pause_time;
       12: wb_dat_o <= pause_thresh;
     endcase // case (wb_adr[7:2])
   
endmodule // simple_gemac_wb
