
module elastic_buffer_tb;

   reg 	      rx_clk = 0, tx_clk = 0, rst = 1;

   reg [7:0]  rxd;
   wire [7:0] rxd_ret;
   reg 	      rx_dv, rx_er, crs, col;
   wire       rx_dv_ret, rx_er_ret, crs_ret, col_ret;
   
   elastic_buffer elastic_buffer
     (.rx_clk(rx_clk),.tx_clk(tx_clk),.rst(rst),
      .rxd(rxd),.rx_dv(rx_dv),.rx_er(rx_er),.crs(crs),.col(col),
      .rxd_ret(rxd_ret),.rx_dv_ret(rx_dv_ret),.rx_er_ret(rx_er_ret),
      .crs_ret(crs_ret),.col_ret(col_ret) );

   always #100 rx_clk = ~rx_clk;
   always #101 tx_clk = ~tx_clk;
   initial #950 rst = 0;

   initial    
     begin
	{col,crs,rx_er,rx_dv,rxd} <= 0;
	@(negedge rst);
	@(posedge rx_clk);

	repeat (13)
	  begin
	     repeat (284)
	       @(posedge rx_clk);
	     SendPKT;
	  end
	repeat (100)
	  @(posedge rx_clk);
	$finish;
     end // initial begin

   reg [7:0] rxd_ret_d1;
   always @(posedge tx_clk)
     rxd_ret_d1 <= rxd_ret;
   
   wire [7:0] diff = rxd_ret_d1 - rxd_ret;

   wire       error = rx_dv_ret && (diff != 8'hFF);
   
   task SendPKT;
      begin
	 {col,crs,rx_er,rx_dv,rxd} <= 0;
	 @(posedge rx_clk);
	 {col,crs,rx_er,rx_dv,rxd} <= {4'hF,8'd1};
	 @(posedge rx_clk);
	 repeat (250)
	   begin
	      rxd <= rxd + 1;
	      @(posedge rx_clk);
	   end
	 {col,crs,rx_er,rx_dv,rxd} <= 0;
	 @(posedge rx_clk);
      end
   endtask // SendPKT
   	   
   initial begin
      $dumpfile("elastic_buffer_tb.vcd");
      $dumpvars(0,elastic_buffer_tb);
   end
endmodule // elastic_buffer_tb
