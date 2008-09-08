onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -divider ctrlStsRegBI
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/dataIn
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/address
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/writeEn
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/strobe_i
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/busClk
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiSysClk
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/dataOut
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/ctrlStsRegSel
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransType
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransCtrl
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransStatus
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiDirectAccessTxData
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiDirectAccessRxData
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/rstFromWire
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/rstSyncToBusClkOut
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/rstSyncToSpiClkOut
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/SDWriteError
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/SDReadError
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/SDInitError
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/SDAddr
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiClkDelay
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/clk
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/rstShift
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/rstFromBus
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiDirectAccessTxDataSTB
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiDirectAccessRxDataSTB
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransTypeSTB
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransCtrlSTB
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransStatusSTB
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/rstSyncToSpiClkFirst
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransCtrlShift
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransStatusReg1
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransStatusReg2
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/SDWriteErrorSTB
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/SDReadErrorSTB
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/SDInitErrorSTB
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransCtrl_reg1
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransCtrl_reg2
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_ctrlStsRegBI/spiTransCtrl_reg3
add wave -noupdate -divider spiTxRxData
add wave -noupdate -divider readWriteSPIWireData
add wave -noupdate -divider spiCtrl
add wave -noupdate -divider initSD
add wave -noupdate -divider sendCmd
add wave -noupdate -divider sdModel
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_sdModel/spiClk
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_sdModel/spiDataIn
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_sdModel/spiDataOut
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_sdModel/spiCS_n
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_sdModel/rxByte
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_sdModel/respByte
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_sdModel/smSt
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_sdModel/cnt
add wave -noupdate -divider txFifo
add wave -noupdate -divider txFifoBI
add wave -noupdate -divider readWriteSDBlock
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/blockAddr
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/clk
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/readWriteSDBlockReq
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/respByte
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/respTout
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/rst
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/rxDataIn
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/rxDataRdy
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/sendCmdRdy
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/txDataEmpty
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/txDataFull
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/txFifoData
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/checkSumByte
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/cmdByte
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/dataByte1
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/dataByte2
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/dataByte3
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/dataByte4
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/readError
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/readWriteSDBlockRdy
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/rxDataRdyClr
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/rxFifoData
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/rxFifoWen
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/sendCmdReq
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/spiCS_n
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/txDataOut
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/txDataWen
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/txFifoRen
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/writeError
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_checkSumByte
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_cmdByte
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_dataByte1
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_dataByte2
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_dataByte3
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_dataByte4
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_readError
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_readWriteSDBlockRdy
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_rxDataRdyClr
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_rxFifoData
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_rxFifoWen
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_sendCmdReq
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_spiCS_n
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_txDataOut
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_txDataWen
add wave -noupdate -format Logic -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_txFifoRen
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_writeError
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/delCnt1
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_delCnt1
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/delCnt2
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_delCnt2
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/locRespByte
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_locRespByte
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/loopCnt
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_loopCnt
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/timeOutCnt
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/next_timeOutCnt
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/CurrState_rwBlkSt
add wave -noupdate -format Literal -radix hexadecimal /testHarness/u_spiMaster/u_readWriteSDBlock/NextState_rwBlkSt
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {717201000 ps} 0} {{Cursor 2} {102093853 ps} 0}
configure wave -namecolwidth 456
configure wave -valuecolwidth 73
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
update
WaveRestoreZoom {0 ps} {115986988 ps}
