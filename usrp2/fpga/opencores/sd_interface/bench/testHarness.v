`include "timescale.v"

module testHarness(	);


// -----------------------------------
// Local Wires
// -----------------------------------
reg clk;
reg rst;
reg spiSysClk;
wire [7:0] adr;
wire [7:0] masterDout;
wire [7:0] masterDin;
wire stb;
wire we;
wire ack;
wire spiClk;
wire spiMasterDataIn;
wire spiMasterDataOut;
wire spiCS_n;


initial begin
$dumpfile("wave.vcd");
$dumpvars(0, u_spiMaster); 
end

spiMaster u_spiMaster (
  //Wishbone bus
  .clk_i(clk),
  .rst_i(rst),
  .address_i(adr),
  .data_i(masterDout),
  .data_o(masterDin),
  .strobe_i(stb),
  .we_i(we),
  .ack_o(ack),

  // SPI logic clock
  .spiSysClk(spiSysClk),

  //SPI bus
  .spiClkOut(spiClk),
  .spiDataIn(spiMasterDataIn),
  .spiDataOut(spiMasterDataOut),
  .spiCS_n(spiCS_n)
);

wb_master_model #(.dwidth(8), .awidth(8)) u_wb_master_model (
  .clk(clk), 
  .rst(rst), 
  .adr(adr), 
  .din(masterDin), 
  .dout(masterDout), 
  .cyc(), 
  .stb(stb), 
  .we(we), 
  .sel(), 
  .ack(ack), 
  .err(1'b0), 
  .rty(1'b0)
);

sdModel u_sdModel (
  .spiClk(spiClk),
  .spiDataIn(spiMasterDataOut),
  .spiDataOut(spiMasterDataIn),
  .spiCS_n(spiCS_n)
);
//--------------- reset ---------------
initial begin
  @(posedge clk);
  @(posedge clk);
  @(posedge clk);
  @(posedge clk);
  @(posedge clk);
  @(posedge clk);
  @(posedge clk);
  @(posedge clk);
  rst <= 1'b1;
  @(posedge clk);
  rst <= 1'b0;
  @(posedge clk);
end
 
// ******************************  Clock section  ******************************
`define CLK_50MHZ_HALF_PERIOD 10
`define CLK_25MHZ_HALF_PERIOD 20

always begin
  #`CLK_25MHZ_HALF_PERIOD clk <= 1'b0;
  #`CLK_25MHZ_HALF_PERIOD clk <= 1'b1;
end

always begin
  #`CLK_50MHZ_HALF_PERIOD spiSysClk <= 1'b0;
  #`CLK_50MHZ_HALF_PERIOD spiSysClk <= 1'b1;
end




endmodule

