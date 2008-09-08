// ---------------------------------- testcase0.v ----------------------------
`include "timescale.v"
`include "spiMaster_defines.v"

module testCase0();

reg ack;
reg [7:0] data;
reg [15:0] dataWord;
reg [7:0] dataRead;
reg [7:0] dataWrite;
integer i;
integer j;

initial
begin
  $write("\n\n");
  //testHarness.reset;
  #1000;

  //write to block addr reg, and read back
  //testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`TRANS_TYPE_REG , 8'h5a);
  $write("Testing register read/write\n");
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`SPI_CLK_DEL_REG , 8'h10);
  testHarness.u_wb_master_model.wb_cmp(1, `CTRL_STS_REG_BASE+`SPI_CLK_DEL_REG , 8'h10);
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`SD_ADDR_7_0_REG , 8'h78);
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`SD_ADDR_15_8_REG , 8'h56);
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`SD_ADDR_23_16_REG , 8'h34);
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`SD_ADDR_31_24_REG , 8'h12);
  testHarness.u_wb_master_model.wb_cmp(1, `CTRL_STS_REG_BASE+`SD_ADDR_7_0_REG , 8'h78);
  testHarness.u_wb_master_model.wb_cmp(1, `CTRL_STS_REG_BASE+`SD_ADDR_15_8_REG , 8'h56);
  testHarness.u_wb_master_model.wb_cmp(1, `CTRL_STS_REG_BASE+`SD_ADDR_23_16_REG , 8'h34);
  testHarness.u_wb_master_model.wb_cmp(1, `CTRL_STS_REG_BASE+`SD_ADDR_31_24_REG , 8'h12);

  //write one byte to spi bus, and wait for complete
  $write("Testing SPI bus direct access\n");
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`TRANS_TYPE_REG , {6'b000000, `DIRECT_ACCESS});
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`DIRECT_ACCESS_DATA_REG , 8'h5f);
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`TRANS_CTRL_REG , {7'b0000000, `TRANS_START});
  testHarness.u_wb_master_model.wb_read(1, `CTRL_STS_REG_BASE+`TRANS_STS_REG , dataRead);
  while (dataRead[0] == `TRANS_BUSY) 
    testHarness.u_wb_master_model.wb_read(1, `CTRL_STS_REG_BASE+`TRANS_STS_REG , dataRead);

  //write one byte to spi bus, and wait for complete
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`DIRECT_ACCESS_DATA_REG , 8'haa);
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`TRANS_CTRL_REG , {7'b0000000, `TRANS_START});
  testHarness.u_wb_master_model.wb_read(1, `CTRL_STS_REG_BASE+`TRANS_STS_REG , dataRead);
  while (dataRead[0] == `TRANS_BUSY) 
    testHarness.u_wb_master_model.wb_read(1, `CTRL_STS_REG_BASE+`TRANS_STS_REG , dataRead);

  //init test
  $write("Testing SD init\n");
  testHarness.u_sdModel.setRespByte(8'h01);
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`TRANS_TYPE_REG , {6'b000000, `INIT_SD});
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`TRANS_CTRL_REG , {7'b0000000, `TRANS_START});
  #60000;
  testHarness.u_sdModel.setRespByte(8'h00);
  testHarness.u_wb_master_model.wb_read(1, `CTRL_STS_REG_BASE+`TRANS_STS_REG , dataRead);
  while (dataRead[0] == `TRANS_BUSY) 
    testHarness.u_wb_master_model.wb_read(1, `CTRL_STS_REG_BASE+`TRANS_STS_REG , dataRead);
  testHarness.u_wb_master_model.wb_read(1, `CTRL_STS_REG_BASE+`TRANS_ERROR_REG , dataRead);
  if (dataRead[1:0] == `INIT_NO_ERROR)
    $write("SD init test passed\n");
  else
    $write("---- ERROR: SD init test failed. Error code = 0x%01x\n", dataRead[1:0] );

  //block write
  $write("Testing block write\n");
  dataWrite = 8'h00;
  for (i=0; i<=511; i=i+1) begin
    testHarness.u_wb_master_model.wb_write(1, `TX_FIFO_BASE+`FIFO_DATA_REG , dataWrite);
    dataWrite = dataWrite + 1'b1;
  end
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`TRANS_TYPE_REG , {6'b000000, `RW_WRITE_SD_BLOCK});
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`TRANS_CTRL_REG , {7'b0000000, `TRANS_START});
  #100000;
  testHarness.u_sdModel.setRespByte(8'h05); //write response
  #8000000;
  testHarness.u_wb_master_model.wb_read(1, `CTRL_STS_REG_BASE+`TRANS_STS_REG , dataRead);
  if (dataRead[0] == `TRANS_BUSY) begin
    $write("---- ERROR: SD block write failed to complete\n");
  end
  else begin
    testHarness.u_wb_master_model.wb_read(1, `CTRL_STS_REG_BASE+`TRANS_ERROR_REG , dataRead);
    if (dataRead[5:4] == `WRITE_NO_ERROR)
      $write("SD block write passed\n");
    else
      $write("---- ERROR: SD block write failed. Error code = 0x%01x\n", dataRead[5:4] );
  end

  //block read
  $write("Testing block read\n");
  testHarness.u_sdModel.setRespByte(8'h00); //cmd response
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`TRANS_TYPE_REG , {6'b000000, `RW_READ_SD_BLOCK});
  testHarness.u_wb_master_model.wb_write(1, `CTRL_STS_REG_BASE+`TRANS_CTRL_REG , {7'b0000000, `TRANS_START});
  #100000;
  testHarness.u_sdModel.setRespByte(8'hfe); //read response
  #8000000;
  testHarness.u_wb_master_model.wb_read(1, `CTRL_STS_REG_BASE+`TRANS_STS_REG , dataRead);
  if (dataRead[0] == `TRANS_BUSY) begin
    $write("---- ERROR: SD block read failed to complete\n");
  end
  else begin
    testHarness.u_wb_master_model.wb_read(1, `CTRL_STS_REG_BASE+`TRANS_ERROR_REG , dataRead);
    if (dataRead[3:2] == `READ_NO_ERROR) begin
      $write("SD block read passed\n");
      for (j=0; j<=15; j=j+1) begin
        $write("Data 0x%0x = ",j*32);
        for (i=0; i<=31; i=i+1) begin
          testHarness.u_wb_master_model.wb_read(1, `RX_FIFO_BASE+`FIFO_DATA_REG , dataRead);
          $write("0x%0x ",dataRead);
        end
        $write("\n");
      end
    end
    else
      $write("---- ERROR: SD block read failed. Error code = 0x%01x\n", dataRead[3:2] );
  end

  $write("Finished all tests\n");
  $stop;	

end

endmodule

