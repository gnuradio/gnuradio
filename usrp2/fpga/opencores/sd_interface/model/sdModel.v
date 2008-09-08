`include "timescale.v"

module sdModel(
  spiClk,
  spiDataIn,
  spiDataOut,
  spiCS_n
);
input spiClk;
input spiDataIn;
output spiDataOut;
reg spiDataOut;
input spiCS_n;

//local wires and regs
reg [7:0] rxByte;
reg [7:0] respByte;
reg [1:0] smSt;
reg [7:0] cnt;

`define START 2'b00
`define WAIT_FF 2'b01
`define WAIT_FF_FIN 2'b10

initial 
begin
  smSt = `START; 
end


// ------------------------------ txRxByte --------------------------
task txRxByte;
input [7:0] txData;
output [7:0] rxData;

integer i;
begin
  spiDataOut <= txData[7];
  //@(negedge spiCS_n);
  for (i=0;i<=7;i=i+1) begin
    @(posedge spiClk);
    rxData[0] <= spiDataIn;
    rxData = rxData << 1;
    @(negedge spiClk);
    spiDataOut <= txData[6];
    txData = txData << 1;
  end
end
endtask


//response state machine
always begin
    case (smSt)
      `START: begin
        txRxByte(8'hff, rxByte);
        if (rxByte == 8'hff) begin
          smSt <= `WAIT_FF;
          cnt <= 8'h00;
        end
      end
      `WAIT_FF: begin
        txRxByte(8'hff, rxByte);
        if (rxByte == 8'hff) begin
          cnt <= cnt + 1'b1;
          if (cnt == 8'h04) begin
            txRxByte(respByte, rxByte); 
            smSt <= `WAIT_FF_FIN;
          end
        end
        else begin
          smSt <= `START;
          cnt <= 8'h00;
        end
      end
      `WAIT_FF_FIN: begin
        txRxByte(8'hff, rxByte);
        if (rxByte == 8'h04) begin
          cnt <= cnt + 1'b1;
          if (cnt == 8'hff) begin
            txRxByte(respByte, rxByte); 
            smSt <= `START;
          end
        end
        else
          smSt <= `START;
      end
    endcase
end

// setRespByte
task setRespByte;
  input [7:0] dataByte;
  begin
    respByte = dataByte;
  end
endtask

endmodule
