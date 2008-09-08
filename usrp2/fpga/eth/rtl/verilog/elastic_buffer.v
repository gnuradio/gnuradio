

module elastic_buffer
  ( input rx_clk,
    input tx_clk,
    input rst,
    
    input [7:0] rxd,
    input 	rx_dv,
    input 	rx_er,
    input 	crs,
    input 	col,
    
    output [7:0] rxd_ret,
    output 	 rx_dv_ret,
    output 	 rx_er_ret,
    output 	 crs_ret,
    output 	 col_ret );

   reg [3:0] 	 addr_wr,addr_wr_gray,awg_d1,awg_d2,addr_wr_gray_ret,awgr_d1,addr_wr_ungray,addr_rd;
   
   reg [11:0] 	 buffer [0:15];
   integer 	 i;
   initial
     for(i=0;i<16;i=i+1)
       buffer[i] <= 0;
   
   reg [7:0] 	 rxd_d1, rxd_d2;
   reg 		 rx_dv_d1,rx_er_d1,crs_d1,col_d1, rx_dv_d2,rx_er_d2,crs_d2,col_d2;		 
   wire 	 rx_dv_ret_adv;
   reg 		 rx_dv_ontime;

   always @(posedge rx_clk)
     {col_d1,crs_d1,rx_er_d1,rx_dv_d1,rxd_d1} <= {col,crs,rx_er,rx_dv,rxd};

   always @(posedge rx_clk)
     {col_d2,crs_d2,rx_er_d2,rx_dv_d2,rxd_d2} <= {col_d1,crs_d1,rx_er_d1,rx_dv_d1,rxd_d1};
   
   always @(posedge rx_clk)
     buffer[addr_wr] <= {col_d2,crs_d2,rx_er_d2,rx_dv_d1,rxd_d2};
   
   always @(posedge rx_clk or posedge rst)
     if(rst) addr_wr <= 0;
     else addr_wr <= addr_wr + 1;
   
   always @(posedge rx_clk)
     begin
	addr_wr_gray <= {addr_wr[3],^addr_wr[3:2],^addr_wr[2:1],^addr_wr[1:0]};
	awg_d1 <= addr_wr_gray;
	awg_d2 <= awg_d1;
     end
   
   always @(posedge tx_clk)
     begin
	addr_wr_gray_ret <= awg_d2;
	awgr_d1 <= addr_wr_gray_ret;
	addr_wr_ungray <= {awgr_d1[3],^awgr_d1[3:2],^awgr_d1[3:1],^awgr_d1[3:0]};
     end

   wire [3:0] addr_delta = addr_rd-addr_wr_ungray;
   reg 	[1:0] direction;
   localparam retard = 2'd0;
   localparam good = 2'd1;
   localparam advance = 2'd2;
   localparam wayoff = 2'd3;
   
   always @*
     case(addr_delta)
       4'd1, 4'd2, 4'd3, 4'd4, 4'd5 : direction <= retard;
       4'd15, 4'd14, 4'd13, 4'd12, 4'd11 : direction <= advance;
       4'd0 : direction <= good;
       default : direction <= wayoff;
     endcase // case(addr_delta)
	
   always @(posedge tx_clk or posedge rst)
     if(rst)
       addr_rd <= 0;
     else if(rx_dv_ret_adv | rx_dv_ontime)
       addr_rd <= addr_rd + 1;
     else
       case(direction)
	 retard : addr_rd <= addr_rd;
	 advance : addr_rd <= addr_rd + 2;
	 good : addr_rd <= addr_rd + 1;
	 wayoff : addr_rd <= addr_wr_ungray;
       endcase // case(direction)
   
   assign     {col_ret,crs_ret,rx_er_ret,rx_dv_ret_adv,rxd_ret} = buffer[addr_rd];
   always @(posedge tx_clk)
     rx_dv_ontime <= rx_dv_ret_adv;

   assign 	 rx_dv_ret = rx_dv_ontime;
endmodule // elastic_buffer
