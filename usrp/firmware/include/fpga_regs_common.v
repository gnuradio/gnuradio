//
// This file is machine generated from ./fpga_regs_common.h
// Do not edit by hand; your edits will be overwritten.
//

// This file defines registers common to all FPGA configurations.
// Registers 0 to 31 are reserved for use in this file.


// The FPGA needs to know the rate that samples are coming from and
// going to the A/D's and D/A's.  div = 128e6 / sample_rate

`define FR_TX_SAMPLE_RATE_DIV     7'd0
`define FR_RX_SAMPLE_RATE_DIV     7'd1

// 2 and 3 are defined in the ATR section

`define FR_MASTER_CTRL            7'd4	// master enable and reset controls

// i/o direction registers for pins that go to daughterboards.
// Setting the bit makes it an output from the FPGA to the d'board.
// top 16 is mask, low 16 is value

`define FR_OE_0                   7'd5	// slot 0
`define FR_OE_1                   7'd6
`define FR_OE_2                   7'd7
`define FR_OE_3                   7'd8

// i/o registers for pins that go to daughterboards.
// top 16 is a mask, low 16 is value

`define FR_IO_0                   7'd9	// slot 0
`define FR_IO_1                   7'd10
`define FR_IO_2                   7'd11
`define FR_IO_3                   7'd12

`define FR_MODE                   7'd13


// If the corresponding bit is set, internal FPGA debug circuitry
// controls the i/o pins for the associated bank of daughterboard
// i/o pins.  Typically used for debugging FPGA designs.

`define FR_DEBUG_EN               7'd14


// If the corresponding bit is set, enable the automatic DC
// offset correction control loop.
//
// The 4 low bits are significant:
//
//   ADC0 = (1 << 0)
//   ADC1 = (1 << 1)
//   ADC2 = (1 << 2)
//   ADC3 = (1 << 3)
//
// This control loop works if the attached daugherboard blocks DC.
// Currently all daughterboards do block DC.  This includes:
// basic rx, dbs_rx, tv_rx, flex_xxx_rx.

`define FR_DC_OFFSET_CL_EN        7'd15			// DC Offset Control Loop Enable


// offset corrections for ADC's and DAC's (2's complement)

`define FR_ADC_OFFSET_0           7'd16
`define FR_ADC_OFFSET_1           7'd17
`define FR_ADC_OFFSET_2           7'd18
`define FR_ADC_OFFSET_3           7'd19


// ------------------------------------------------------------------------
// Automatic Transmit/Receive switching
//
// If automatic transmit/receive (ATR) switching is enabled in the
// FR_ATR_CTL register, the presence or absence of data in the FPGA
// transmit fifo selects between two sets of values for each of the 4
// banks of daughterboard i/o pins.
//
// Each daughterboard slot has 3 16-bit registers associated with it:
//   FR_ATR_MASK_*, FR_ATR_TXVAL_* and FR_ATR_RXVAL_*
//
// FR_ATR_MASK_{0,1,2,3}: 
//
//   These registers determine which of the daugherboard i/o pins are
//   affected by ATR switching.  If a bit in the mask is set, the
//   corresponding i/o bit is controlled by ATR, else it's output
//   value comes from the normal i/o pin output register:
//   FR_IO_{0,1,2,3}.
//
// FR_ATR_TXVAL_{0,1,2,3}:
// FR_ATR_RXVAL_{0,1,2,3}:
//
//   If the Tx fifo contains data, then the bits from TXVAL that are
//   selected by MASK are output.  Otherwise, the bits from RXVAL that
//   are selected by MASK are output.
                      
`define FR_ATR_MASK_0             7'd20	// slot 0
`define FR_ATR_TXVAL_0            7'd21
`define FR_ATR_RXVAL_0            7'd22

`define FR_ATR_MASK_1             7'd23	// slot 1
`define FR_ATR_TXVAL_1            7'd24
`define FR_ATR_RXVAL_1            7'd25

`define FR_ATR_MASK_2             7'd26	// slot 2
`define FR_ATR_TXVAL_2            7'd27
`define FR_ATR_RXVAL_2            7'd28

`define FR_ATR_MASK_3             7'd29	// slot 3
`define FR_ATR_TXVAL_3            7'd30
`define FR_ATR_RXVAL_3            7'd31

// Clock ticks to delay rising and falling edge of T/R signal
`define FR_ATR_TX_DELAY           7'd2
`define FR_ATR_RX_DELAY           7'd3

