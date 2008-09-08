`include "timescale.v"

module spiTxRxData (
  clk,
  rst,

  tx1DataIn,
  tx2DataIn,
  tx3DataIn,
  tx4DataIn,
  tx1DataWEn,
  tx2DataWEn,
  tx3DataWEn,
  tx4DataWEn,

  txDataOut,
  txDataFull,
  txDataFullClr,

  rx1DataRdyClr,
  rx2DataRdyClr,
  rx3DataRdyClr,
  rx4DataRdyClr,

  rxDataIn,
  rxDataOut,
  rxDataRdy,
  rxDataRdySet
);

input clk;
input rst;

input [7:0] tx1DataIn;
input [7:0] tx2DataIn;
input [7:0] tx3DataIn;
input [7:0] tx4DataIn;
input tx1DataWEn;
input tx2DataWEn;
input tx3DataWEn;
input tx4DataWEn;

output [7:0] txDataOut;
reg [7:0] txDataOut;
output txDataFull;
reg txDataFull;
input txDataFullClr;

input rx1DataRdyClr;
input rx2DataRdyClr;
input rx3DataRdyClr;
input rx4DataRdyClr;

input [7:0] rxDataIn;
output [7:0] rxDataOut;
reg [7:0] rxDataOut;
output rxDataRdy;
reg rxDataRdy;
input rxDataRdySet;


// --- Transmit control
always @(posedge clk) begin
  if (rst == 1'b1) begin
    txDataOut <= 8'h00;
    txDataFull <= 1'b0;
  end
  else begin
    if (tx1DataWEn == 1'b1) begin
      txDataOut <= tx1DataIn;
      txDataFull <= 1'b1;
    end
    else if (tx2DataWEn == 1'b1) begin
      txDataOut <= tx2DataIn;
      txDataFull <= 1'b1;
    end
    else if (tx3DataWEn == 1'b1) begin
      txDataOut <= tx3DataIn;
      txDataFull <= 1'b1;
    end
    else if (tx4DataWEn == 1'b1) begin
      txDataOut <= tx4DataIn;
      txDataFull <= 1'b1;
    end
    if (txDataFullClr == 1'b1)
      txDataFull <= 1'b0;
  end
end

// --- Receive control
always @(posedge clk) begin
  if (rst == 1'b1) begin
    rxDataOut <= 8'h00;
    rxDataRdy <= 1'b0;
  end
  else begin
    if (rx1DataRdyClr == 1'b1 || rx2DataRdyClr == 1'b1 || rx3DataRdyClr == 1'b1 || rx4DataRdyClr == 1'b1) begin
      rxDataRdy <= 1'b0;
    end
    if (rxDataRdySet == 1'b1) begin
      rxDataRdy <= 1'b1;
      rxDataOut <= rxDataIn;
    end
  end
end

endmodule

