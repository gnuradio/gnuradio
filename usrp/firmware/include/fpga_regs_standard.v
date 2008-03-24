//
// This file is machine generated from ./fpga_regs_standard.h
// Do not edit by hand; your edits will be overwritten.
//

// Register numbers 0 to 31 are reserved for use in fpga_regs_common.h.
// Registers 64 to 79 are available for custom FPGA builds.


// DDC / DUC

`define FR_INTERP_RATE            7'd32	// [1,1024]
`define FR_DECIM_RATE             7'd33	// [1,256]

// DDC center freq

`define FR_RX_FREQ_0              7'd34
`define FR_RX_FREQ_1              7'd35
`define FR_RX_FREQ_2              7'd36
`define FR_RX_FREQ_3              7'd37

// See below for DDC Starting Phase

// ------------------------------------------------------------------------
//  configure FPGA Rx mux
//
//    3                   2                   1                       
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-----------------------+-------+-------+-------+-------+-+-----+
// |      must be zero     | Q3| I3| Q2| I2| Q1| I1| Q0| I0|Z| NCH |
// +-----------------------+-------+-------+-------+-------+-+-----+
//
// There are a maximum of 4 digital downconverters in the the FPGA.
// Each DDC has two 16-bit inputs, I and Q, and two 16-bit outputs, I & Q.
//
// DDC I inputs are specified by the two bit fields I3, I2, I1 & I0
//
//   0 = DDC input is from ADC 0
//   1 = DDC input is from ADC 1
//   2 = DDC input is from ADC 2
//   3 = DDC input is from ADC 3
//
// If Z == 1, all DDC Q inputs are set to zero
// If Z == 0, DDC Q inputs are specified by the two bit fields Q3, Q2, Q1 & Q0
//
// NCH specifies the number of complex channels that are sent across
// the USB.  The legal values are 1, 2 or 4, corresponding to 2, 4 or
// 8 16-bit values.

`define FR_RX_MUX                 7'd38

// ------------------------------------------------------------------------
//  configure FPGA Tx Mux.
//
//    3                   2                   1                       
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-----------------------+-------+-------+-------+-------+-+-----+
// |                       | DAC3  | DAC2  | DAC1  |  DAC0 |0| NCH |
// +-----------------------------------------------+-------+-+-----+
//
// NCH specifies the number of complex channels that are sent across
// the USB.  The legal values are 1 or 2, corresponding to 2 or 4
// 16-bit values.
//
// There are two interpolators with complex inputs and outputs.
// There are four DACs.  (We use the DUC in each AD9862.)
//
// Each 4-bit DACx field specifies the source for the DAC and
// whether or not that DAC is enabled.  Each subfield is coded
// like this: 
//
//    3 2 1 0
//   +-+-----+
//   |E|  N  |
//   +-+-----+
//
// Where E is set if the DAC is enabled, and N specifies which
// interpolator output is connected to this DAC.
//
//  N   which interp output
// ---  -------------------
//  0   chan 0 I
//  1   chan 0 Q
//  2   chan 1 I
//  3   chan 1 Q

`define FR_TX_MUX                 7'd39

// ------------------------------------------------------------------------
// REFCLK control
//
// Control whether a reference clock is sent to the daughterboards,
// and what frequency.  The refclk is sent on d'board i/o pin 0.
//
//    3                   2                   1                       
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-----------------------------------------------+-+------------+
// |             Reserved (Must be zero)           |E|   DIVISOR  |
// +-----------------------------------------------+-+------------+

//
// Bit 7  -- 1 turns on refclk, 0 allows IO use
// Bits 6:0 Divider value

`define FR_TX_A_REFCLK            7'd40
`define FR_RX_A_REFCLK            7'd41
`define FR_TX_B_REFCLK            7'd42
`define FR_RX_B_REFCLK            7'd43


// ------------------------------------------------------------------------
// DDC Starting Phase

`define FR_RX_PHASE_0             7'd44
`define FR_RX_PHASE_1             7'd45
`define FR_RX_PHASE_2             7'd46
`define FR_RX_PHASE_3             7'd47

// ------------------------------------------------------------------------
// Tx data format control register
//
//    3                   2                   1                       
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-------------------------------------------------------+-------+
// |                    Reserved (Must be zero)            |  FMT  |
// +-------------------------------------------------------+-------+
//
//  FMT values:

`define FR_TX_FORMAT              7'd48

// ------------------------------------------------------------------------
// Rx data format control register
//
//    3                   2                   1                       
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-----------------------------------------+-+-+---------+-------+
// |          Reserved (Must be zero)        |B|Q|  WIDTH  | SHIFT |
// +-----------------------------------------+-+-+---------+-------+
//
//  FMT values:

`define FR_RX_FORMAT              7'd49


// The valid combinations currently are:
//
//   B  Q  WIDTH  SHIFT
//   0  1    16     0
//   0  1     8     8


// Possible future values of WIDTH = {4, 2, 1}
// 12 takes a bit more work, since we need to know packet alignment.

// ------------------------------------------------------------------------
// FIXME register numbers 50 to 63 are available

// ------------------------------------------------------------------------
// Registers 64 to 95 are reserved for user custom FPGA builds.
// The standard USRP software will not touch these.

`define FR_USER_0                 7'd64
`define FR_USER_1                 7'd65
`define FR_USER_2                 7'd66
`define FR_USER_3                 7'd67
`define FR_USER_4                 7'd68
`define FR_USER_5                 7'd69
`define FR_USER_6                 7'd70
`define FR_USER_7                 7'd71
`define FR_USER_8                 7'd72
`define FR_USER_9                 7'd73
`define FR_USER_10                7'd74
`define FR_USER_11                7'd75
`define FR_USER_12                7'd76
`define FR_USER_13                7'd77
`define FR_USER_14                7'd78
`define FR_USER_15                7'd79
`define FR_USER_16                7'd80
`define FR_USER_17                7'd81
`define FR_USER_18                7'd82
`define FR_USER_19                7'd83
`define FR_USER_20                7'd84
`define FR_USER_21                7'd85
`define FR_USER_22                7'd86
`define FR_USER_23                7'd87
`define FR_USER_24                7'd88
`define FR_USER_25                7'd89
`define FR_USER_26                7'd90
`define FR_USER_27                7'd91
`define FR_USER_28                7'd92
`define FR_USER_29                7'd93
`define FR_USER_30                7'd94
`define FR_USER_31                7'd95

//Registers needed for multi usrp master/slave configuration
//
//Rx Master/slave control register (FR_RX_MASTER_SLAVE = FR_USER_0)
//
`define FR_RX_MASTER_SLAVE        7'd64
`define bitnoFR_RX_SYNC           0
`define bitnoFR_RX_SYNC_MASTER    1
`define bitnoFR_RX_SYNC_SLAVE     2


//Caution The master settings will output values on the io lines.
//They inheritely enable these lines as output. If you have a daughtercard which uses these lines also as output then you will burn your usrp and daughtercard.
//If you set the slave bits then your usrp won't do anything if you don't connect a master.
// Rx Master/slave control register
//
// The way this is supposed to be used is connecting a (short) 16pin flatcable from an rx daughterboard in RXA master io_rx[8..15] to slave io_rx[8..15] on RXA of slave usrp
// This can be done with basic_rx boards or dbsrx boards
//dbsrx: connect master-J25 to slave-J25
//basic rx: connect J25 to slave-J25
//CAUTION: pay attention to the lineup of your connector.
//The red line (pin1) should be at the same side of the daughterboards on master and slave.
//If you turnaround the cable on one end you will burn your usrp.

//You cannot use a 16pin flatcable if you are using FLEX400 or FLEX2400 daughterboards, since these use a lot of the io pins.
//You can still link them but you must use only a 2pin or 1pin cable
//You can also use a 2-wire link. put a 2pin header on io[15],gnd of the master RXA daughterboard and connect it to io15,gnd of the slave RXA db.
//You can use a cable like the ones found with the leds on the mainbord of a PC.
//Make sure you don't twist the cable, otherwise you connect the sync output to ground.
//To be save you could also just use a single wire from master io[15] to slave io[15], but this is not optimal for signal integrity.


// Since rx_io[0] can normally be used as a refclk and is not exported on all daughterboards this line
// still has the refclk function if you use the master/slave setup (it is not touched by the master/slave settings).
// The master/slave circuitry will only use io pin 15 and does not touch any of the other io pins.
`define bitnoFR_RX_SYNC_INPUT_IOPIN 15
`define bmFR_RX_SYNC_INPUT_IOPIN  (1<<bitnoFR_RX_SYNC_INPUT_IOPIN)
//TODO the output pin is still hardcoded in the verilog code, make it listen to the following define
`define bitnoFR_RX_SYNC_OUTPUT_IOPIN 15
`define bmFR_RX_SYNC_OUTPUT_IOPIN (1<<bitnoFR_RX_SYNC_OUTPUT_IOPIN)
// =======================================================================
// READBACK Registers
// =======================================================================

`define FR_RB_IO_RX_A_IO_TX_A     7'd1	// read back a-side i/o pins
`define FR_RB_IO_RX_B_IO_TX_B     7'd2	// read back b-side i/o pins

// ------------------------------------------------------------------------
// FPGA Capability register
//
//    3                   2                   1                       
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-----------------------------------------------+-+-----+-+-----+
// |                    Reserved (Must be zero)    |T|NDUC |R|NDDC |
// +-----------------------------------------------+-+-----+-+-----+
//
// Bottom 4-bits are Rx capabilities
// Next   4-bits are Tx capabilities

`define FR_RB_CAPS                7'd3


