/* -*- c -*- */
/*
 * Copyright 2007,2008,2009 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Overall Memory Map
 *   0000-7FFF  32K   RAM space (16K on 1500, 24K on 2000, 32K on DSP)
 *   8000-BFFF  16K   Buffer Pool
 *   C000-FFFF  16K   Peripherals
 */


#ifndef INCLUDED_MEMORY_MAP_H
#define INCLUDED_MEMORY_MAP_H

#include <stdint.h>


#define MASTER_CLK_RATE        100000000		// 100 MHz


////////////////////////////////////////////////////////////////
//
//         Memory map for embedded wishbone bus
//
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// Main RAM, Slave 0

#define RAM_BASE 0x0000

////////////////////////////////////////////////////////////////
// Buffer Pool RAM, Slave 1
//
// The buffers themselves are located in Slave 1, Buffer Pool RAM.
// The status registers are in Slave 5, Buffer Pool Status.
// The control register is in Slave 7, Settings Bus.

#define BUFFER_POOL_RAM_BASE 0x8000

#define	NBUFFERS                8
#define BP_NLINES	   0x0200	// number of 32-bit lines in a buffer
#define BP_LAST_LINE	(BP_NLINES - 1)	// last line in a buffer

#define buffer_pool_ram \
  ((uint32_t *) BUFFER_POOL_RAM_BASE)

#define buffer_ram(n) (&buffer_pool_ram[(n) * BP_NLINES])


/////////////////////////////////////////////////////
// SPI Core, Slave 2.  See core docs for more info
#define SPI_BASE 0xC000   // Base address (16-bit)

typedef struct {
  volatile uint32_t	txrx0;
  volatile uint32_t	txrx1;
  volatile uint32_t	txrx2;
  volatile uint32_t	txrx3;
  volatile uint32_t	ctrl;
  volatile uint32_t	div;
  volatile uint32_t	ss;
} spi_regs_t;

#define spi_regs ((spi_regs_t *) SPI_BASE)


// Masks for controlling different peripherals
#define SPI_SS_AD9510    1
#define SPI_SS_AD9777    2
#define SPI_SS_RX_DAC    4
#define SPI_SS_RX_ADC    8
#define SPI_SS_RX_DB    16
#define SPI_SS_TX_DAC   32
#define SPI_SS_TX_ADC   64
#define SPI_SS_TX_DB   128

// Masks for different parts of CTRL reg
#define SPI_CTRL_ASS      (1<<13)
#define SPI_CTRL_IE       (1<<12)
#define SPI_CTRL_LSB      (1<<11)
#define SPI_CTRL_TXNEG    (1<<10)
#define SPI_CTRL_RXNEG    (1<< 9)
#define SPI_CTRL_GO_BSY   (1<< 8)
#define SPI_CTRL_CHAR_LEN_MASK 0x7F

////////////////////////////////////////////////
// I2C, Slave 3
// See Wishbone I2C-Master Core Specification.

#define I2C_BASE 0xC400

typedef struct {
  volatile uint32_t  prescaler_lo;	// r/w
  volatile uint32_t  prescaler_hi;	// r/w
  volatile uint32_t  ctrl;		// r/w
  volatile uint32_t  data;		// wr = transmit reg; rd = receive reg
  volatile uint32_t  cmd_status;	// wr = command reg;  rd = status reg
} i2c_regs_t;

#define i2c_regs ((i2c_regs_t *) I2C_BASE)

#define	I2C_CTRL_EN	(1 << 7)	// core enable
#define	I2C_CTRL_IE	(1 << 6)	// interrupt enable

//
// STA, STO, RD, WR, and IACK bits are cleared automatically
//
#define	I2C_CMD_START	(1 << 7)	// generate (repeated) start condition
#define I2C_CMD_STOP	(1 << 6)	// generate stop condition
#define	I2C_CMD_RD	(1 << 5)	// read from slave
#define I2C_CMD_WR	(1 << 4)	// write to slave
#define	I2C_CMD_NACK	(1 << 3)	// when a rcvr, send ACK (ACK=0) or NACK (ACK=1)
#define I2C_CMD_RSVD_2	(1 << 2)	// reserved
#define	I2C_CMD_RSVD_1	(1 << 1)	// reserved
#define I2C_CMD_IACK	(1 << 0)	// set to clear pending interrupt

#define I2C_ST_RXACK	(1 << 7)	// Received acknowledgement from slave (1 = NAK, 0 = ACK)
#define	I2C_ST_BUSY	(1 << 6)	// 1 after START signal detected; 0 after STOP signal detected
#define	I2C_ST_AL	(1 << 5)	// Arbitration lost.  1 when core lost arbitration
#define	I2C_ST_RSVD_4	(1 << 4)	// reserved
#define	I2C_ST_RSVD_3	(1 << 3)	// reserved
#define	I2C_ST_RSVD_2	(1 << 2)	// reserved
#define I2C_ST_TIP	(1 << 1)	// Transfer-in-progress
#define	I2C_ST_IP	(1 << 0)	// Interrupt pending


////////////////////////////////////////////////
// GPIO, Slave 4
//
// These go to the daughterboard i/o pins

#define GPIO_BASE 0xC800

typedef struct {
  volatile uint32_t	io;	  // tx data in high 16, rx in low 16
  volatile uint32_t     ddr;      // 32 bits, 1 means output. tx in high 16, rx in low 16
  volatile uint32_t	tx_sel;   // 16 2-bit fields select which source goes to TX DB
  volatile uint32_t	rx_sel;   // 16 2-bit fields select which source goes to RX DB
} gpio_regs_t;

// each 2-bit sel field is layed out this way
#define GPIO_SEL_SW	   0 // if pin is an output, set by software in the io reg
#define	GPIO_SEL_ATR	   1 // if pin is an output, set by ATR logic
#define	GPIO_SEL_DEBUG_0   2 // if pin is an output, debug lines from FPGA fabric
#define	GPIO_SEL_DEBUG_1   3 // if pin is an output, debug lines from FPGA fabric

#define gpio_base ((gpio_regs_t *) GPIO_BASE)

///////////////////////////////////////////////////
// Buffer Pool Status, Slave 5
//
// The buffers themselves are located in Slave 1, Buffer Pool RAM.
// The status registers are in Slave 5, Buffer Pool Status.
// The control register is in Slave 7, Settings Bus.

#define BUFFER_POOL_STATUS_BASE 0xCC00

typedef struct {
  volatile uint32_t last_line[NBUFFERS]; // last line xfer'd in buffer
  volatile uint32_t status;	         // error and done flags
  volatile uint32_t hw_config;	         // see below
  volatile uint32_t dummy[3];
  volatile uint32_t irqs;
  volatile uint32_t pri_enc_bp_status;
  volatile uint32_t cycle_count;
} buffer_pool_status_t;

#define buffer_pool_status ((buffer_pool_status_t *) BUFFER_POOL_STATUS_BASE)

/*
 * Buffer n's xfer is done.
 * Clear this bit by issuing bp_clear_buf(n)
 */
#define BPS_DONE(n)     (0x00000001 << (n))
#define BPS_DONE_0	BPS_DONE(0)
#define BPS_DONE_1	BPS_DONE(1)
#define BPS_DONE_2	BPS_DONE(2)
#define BPS_DONE_3	BPS_DONE(3)
#define BPS_DONE_4	BPS_DONE(4)
#define BPS_DONE_5	BPS_DONE(5)
#define BPS_DONE_6	BPS_DONE(6)
#define BPS_DONE_7	BPS_DONE(7)

/*
 * Buffer n's xfer had an error.
 * Clear this bit by issuing bp_clear_buf(n)
 */
#define BPS_ERROR(n)	(0x00000100 << (n))
#define BPS_ERROR_0	BPS_ERROR(0)
#define BPS_ERROR_1	BPS_ERROR(1)
#define BPS_ERROR_2	BPS_ERROR(2)
#define BPS_ERROR_3	BPS_ERROR(3)
#define BPS_ERROR_4	BPS_ERROR(4)
#define BPS_ERROR_5	BPS_ERROR(5)
#define BPS_ERROR_6	BPS_ERROR(6)
#define BPS_ERROR_7	BPS_ERROR(7)

/*
 * Buffer n is idle.  A buffer is idle if it's not
 * DONE, ERROR, or processing a transaction.  If it's
 * IDLE, it's safe to start a new transaction.
 *
 * Clear this bit by starting a xfer with
 * bp_send_from_buf or bp_receive_to_buf.
 */
#define BPS_IDLE(n)     (0x00010000 << (n))
#define BPS_IDLE_0	BPS_IDLE(0)
#define BPS_IDLE_1	BPS_IDLE(1)
#define BPS_IDLE_2	BPS_IDLE(2)
#define BPS_IDLE_3	BPS_IDLE(3)
#define BPS_IDLE_4	BPS_IDLE(4)
#define BPS_IDLE_5	BPS_IDLE(5)
#define BPS_IDLE_6	BPS_IDLE(6)
#define BPS_IDLE_7	BPS_IDLE(7)

/*
 * Buffer n has a "slow path" packet in it.
 * This bit is orthogonal to the bits above and indicates that
 * the FPGA ethernet rx protocol engine has identified this packet
 * as one requiring firmware intervention.
 */
#define BPS_SLOWPATH(n) (0x01000000 << (n))
#define BPS_SLOWPATH_0	BPS_SLOWPATH(0)
#define BPS_SLOWPATH_1	BPS_SLOWPATH(1)
#define BPS_SLOWPATH_2	BPS_SLOWPATH(2)
#define BPS_SLOWPATH_3	BPS_SLOWPATH(3)
#define BPS_SLOWPATH_4	BPS_SLOWPATH(4)
#define BPS_SLOWPATH_5	BPS_SLOWPATH(5)
#define BPS_SLOWPATH_6	BPS_SLOWPATH(6)
#define BPS_SLOWPATH_7	BPS_SLOWPATH(7)


#define BPS_DONE_ALL	  0x000000ff	// mask of all dones
#define BPS_ERROR_ALL	  0x0000ff00	// mask of all errors
#define BPS_IDLE_ALL      0x00ff0000	// mask of all idles
#define BPS_SLOWPATH_ALL  0xff000000	// mask of all slowpaths

// The hw_config register

#define	HWC_SIMULATION		0x80000000
#define	HWC_WB_CLK_DIV_MASK	0x0000000f

/*!
 * \brief return non-zero if we're running under the simulator
 */
inline static int
hwconfig_simulation_p(void)
{
  return buffer_pool_status->hw_config & HWC_SIMULATION;
}

/*!
 * \brief Return Wishbone Clock divisor.
 * The processor runs at the Wishbone Clock rate which is MASTER_CLK_RATE / divisor.
 */
inline static int
hwconfig_wishbone_divisor(void)
{
  return buffer_pool_status->hw_config & HWC_WB_CLK_DIV_MASK;
}

///////////////////////////////////////////////////
// Ethernet Core, Slave 6

#define ETH_BASE 0xD000

#include "eth_mac_regs.h"

#define eth_mac ((eth_mac_regs_t *) ETH_BASE)

////////////////////////////////////////////////////
// Settings Bus, Slave #7, Not Byte Addressable!
//
// Output-only from processor point-of-view.
// 1KB of address space (== 256 32-bit write-only regs)


#define MISC_OUTPUT_BASE        0xD400
#define	TX_PROTOCOL_ENGINE_BASE 0xD480
#define	RX_PROTOCOL_ENGINE_BASE 0xD4C0
#define BUFFER_POOL_CTRL_BASE   0xD500
#define DSP_TX_BASE             0xD600
#define DSP_RX_BASE             0xD680

#define LAST_SETTING_REG        0xD7FC	// last valid setting register

// --- buffer pool control regs ---

typedef struct {
  volatile uint32_t ctrl;
} buffer_pool_ctrl_t;

// buffer pool ports

#define	PORT_SERDES	0	// serial/deserializer
#define	PORT_DSP	1	// DSP tx or rx pipeline
#define	PORT_ETH	2	// ethernet tx or rx
#define	PORT_RAM	3	// RAM tx or rx

// the buffer pool ctrl register fields

#define BPC_BUFFER(n) (((n) & 0xf) << 28)
#define   BPC_BUFFER_MASK      BPC_BUFFER(~0)
#define   BPC_BUFFER_0	       BPC_BUFFER(0)
#define   BPC_BUFFER_1	       BPC_BUFFER(1)
#define   BPC_BUFFER_2	       BPC_BUFFER(2)
#define   BPC_BUFFER_3	       BPC_BUFFER(3)
#define   BPC_BUFFER_4	       BPC_BUFFER(4)
#define   BPC_BUFFER_5	       BPC_BUFFER(5)
#define   BPC_BUFFER_6	       BPC_BUFFER(6)
#define   BPC_BUFFER_7	       BPC_BUFFER(7)
#define	  BPC_BUFFER_NIL       BPC_BUFFER(0x8)	// disable

#define BPC_PORT(n) (((n) & 0x7) << 25)
#define   BPC_PORT_MASK        BPC_PORT(~0)
#define   BPC_PORT_SERDES      BPC_PORT(PORT_SERDES)
#define   BPC_PORT_DSP	       BPC_PORT(PORT_DSP)
#define   BPC_PORT_ETH         BPC_PORT(PORT_ETH)
#define   BPC_PORT_RAM         BPC_PORT(PORT_RAM)
#define   BPC_PORT_NIL	       BPC_PORT(0x4)   	// disable

#define	BPC_CLR	       	       (1 << 24)  // mutually excl commands
#define	BPC_READ	       (1 << 23)
#define BPC_WRITE              (1 << 22)

#define BPC_STEP(step) (((step) & 0xf) << 18)
#define   BPC_STEP_MASK	       BPC_STEP(~0)
#define BPC_LAST_LINE(line) (((line) & 0x1ff) << 9)
#define   BPC_LAST_LINE_MASK   BPC_LAST_LINE(~0)
#define BPC_FIRST_LINE(line) (((line) & 0x1ff) << 0)
#define   BPC_FIRST_LINE_MASK  BPC_FIRST_LINE(~0)

#define buffer_pool_ctrl ((buffer_pool_ctrl_t *) BUFFER_POOL_CTRL_BASE)

// --- misc outputs ---

typedef struct {
  volatile uint32_t	clk_ctrl;
  volatile uint32_t	serdes_ctrl;
  volatile uint32_t	adc_ctrl;
  volatile uint32_t	leds;
  volatile uint32_t	phy_ctrl;	// LSB is reset line to eth phy
  volatile uint32_t	debug_mux_ctrl;
  volatile uint32_t     ram_page;       // FIXME should go somewhere else...
  volatile uint32_t     flush_icache;   // Flush the icache
  volatile uint32_t     led_src;        // HW or SW control for LEDs
} output_regs_t;

#define SERDES_ENABLE 8
#define SERDES_PRBSEN 4
#define SERDES_LOOPEN 2
#define SERDES_RXEN   1

#define	ADC_CTRL_ON	0x0F
#define	ADC_CTRL_OFF	0x00

// crazy order that matches the labels on the case

#define	LED_A		(1 << 4)
#define	LED_B		(1 << 1)
#define	LED_C		(1 << 3)
#define	LED_D		(1 << 0)
#define	LED_E		(1 << 2)
//      LED_F		// controlled by CPLD
#define	LED_RJ45	(1 << 5)

#define output_regs ((output_regs_t *) MISC_OUTPUT_BASE)

// --- dsp tx regs ---

#define MIN_CIC_INTERP	1
#define	MAX_CIC_INTERP  128

typedef struct {
  volatile int32_t	freq;
  volatile uint32_t	scale_iq;	// {scale_i,scale_q}
  volatile uint32_t     interp_rate;
  volatile uint32_t     clear_state;	// clears out state machine, fifos,
                                        //   NOT freq, scale, interp
  /*!
   * \brief output mux configuration.
   *
   * <pre>
   *     3                   2                   1                       
   *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   *  +-------------------------------+-------+-------+-------+-------+
   *  |                                               | DAC1  |  DAC0 |
   *  +-------------------------------+-------+-------+-------+-------+
   * 
   *  There are N DUCs (1 now) with complex inputs and outputs.
   *  There are two DACs.
   * 
   *  Each 4-bit DACx field specifies the source for the DAC
   *  Each subfield is coded like this: 
   * 
   *     3 2 1 0
   *    +-------+
   *    |   N   |
   *    +-------+
   * 
   *  N specifies which DUC output is connected to this DAC.
   * 
   *   N   which interp output
   *  ---  -------------------
   *   0   DUC 0 I
   *   1   DUC 0 Q
   *   2   DUC 1 I
   *   3   DUC 1 Q
   *   F   All Zeros
   *   
   * The default value is 0x10
   * </pre>
   */
  volatile uint32_t	tx_mux;

} dsp_tx_regs_t;
  
#define dsp_tx_regs ((dsp_tx_regs_t *) DSP_TX_BASE)

// --- dsp rx regs ---

#define T_NOW (-1)

#define	MIN_CIC_DECIM	1
#define	MAX_CIC_DECIM   128

typedef struct {
  volatile int32_t	freq;
  volatile uint32_t	scale_iq;	// {scale_i,scale_q}
  volatile uint32_t     decim_rate;
  volatile uint32_t     rx_time;	// when to begin reception
  volatile uint32_t     rx_command;	// {now, chain, num_lines(21), lines_per_frame(9)
  volatile uint32_t     clear_state;    // clears out state machine, fifos,
                                        //   cmd queue, NOT freq, scale, decim
  volatile uint32_t     dcoffset_i;     // Bit 31 high sets fixed offset mode, using lower 14 bits,
                                        // otherwise it is automatic 
  volatile uint32_t     dcoffset_q;     // Bit 31 high sets fixed offset mode, using lower 14 bits

  /*!
   * \brief input mux configuration.
   *
   * This determines which ADC (or constant zero) is connected to 
   * each DDC input.  There are N DDCs (1 now).  Each has two inputs.
   *
   * <pre>
   * Mux value:
   *
   *    3                   2                   1                       
   *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   * +-------+-------+-------+-------+-------+-------+-------+-------+
   * |                                                       |Q0 |I0 |
   * +-------+-------+-------+-------+-------+-------+-------+-------+
   *
   * Each 2-bit I field is either 00 (A/D A), 01 (A/D B) or 1X (const zero)
   * Each 2-bit Q field is either 00 (A/D A), 01 (A/D B) or 1X (const zero)
   *
   * The default value is 0x4
   * </pre>
   */
  volatile uint32_t     rx_mux;        // called adc_mux in dsp_core_rx.v

  /*!
   * \brief Streaming GPIO configuration
   *
   * This determines whether the LSBs of I and Q samples come from the DSP
   * pipeline or from the io_rx GPIO pins.  To stream GPIO, one must first
   * set the GPIO data direction register to have io_rx[15] and/or io_rx[14]
   * configured as inputs.  The GPIO pins will be sampled at the time the
   * remainder of the DSP sample is strobed into the RX sample FIFO.  There
   * will be a decimation-dependent fixed time offset between the GPIO
   * sample stream and the associated RF samples.
   *
   *    3                   2                   1                       
   *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   * +-------+-------+-------+-------+-------+-------+-------+-------+
   * |                           MBZ                             |Q|I|
   * +-------+-------+-------+-------+-------+-------+-------+-------+
   *
   * I         0=LSB comes from DSP pipeline (default)
   *           1=LSB comes from io_rx[15]
   * 
   * Q         0=LSB comes from DSP pipeline (default)
   *           1=LSB comes from io_rx[14]
   */
  volatile uint32_t gpio_stream_enable;

} dsp_rx_regs_t;
  
#define dsp_rx_regs ((dsp_rx_regs_t *) DSP_RX_BASE)

#define MK_RX_CMD(num_lines, lines_per_frame, now, chain) \
  (((num_lines) << 9) | ((lines_per_frame) & 0x1ff) \
   | (((now) & 0x1) << 31) | (((chain) & 0x1) << 30))

/* 
 * --- ethernet tx protocol engine regs (write only) ---
 *
 * These registers control the transmit portion of the ethernet
 * protocol engine (out of USRP2).  The protocol engine handles fifo
 * status and sequence number insertion in outgoing packets, and
 * automagically generates status packets when required to inform the
 * host of changes in fifo availability.
 *
 * All outgoing packets have their fifo_status field set to the number
 * of 32-bit lines of fifo available in the ethernet Rx fifo (see
 * usrp2_eth_packet.h).  Seqno's are set if FIXME, else 0.
 *
 * FIXME clean this up once we know how it's supposed to behave.
 */

typedef struct {
  volatile uint32_t  flags;	     // not yet fully defined (channel?)
  volatile uint32_t  mac_dst0123;    // 4 bytes of destination mac addr
  volatile uint32_t  mac_dst45src01; // 2 bytes of dest mac addr; 2 bytes of src mac addr
  volatile uint32_t  mac_src2345;    // 4 bytes of destination mac addr
  volatile uint32_t  seqno;	     // Write to init seqno.  It autoincs on match
} tx_proto_engine_regs_t;

#define tx_proto_engine ((tx_proto_engine_regs_t *) TX_PROTOCOL_ENGINE_BASE)

/*
 * --- ethernet rx protocol engine regs (write only) ---
 *
 * These registers control the receive portion of the ethernet
 * protocol engine (into USRP2).  The protocol engine offloads common
 * packet inspection operations so that firmware has less to do on
 * "fast path" packets.
 *
 * The registers define conditions which must be matched for a packet
 * to be considered a "fast path" packet.  If a received packet
 * matches the src and dst mac address, ethertype, flags field, and
 * expected seqno number it is considered a "fast path" packet, and
 * the expected seqno is updated.  If the packet fails to satisfy any
 * of the above conditions it's a "slow path" packet, and the
 * corresponding SLOWPATH flag will be set buffer_status register.
 */

typedef struct {
  volatile uint32_t  flags;	     // not yet fully defined (channel?)
  volatile uint32_t  mac_dst0123;    // 4 bytes of destination mac addr
  volatile uint32_t  mac_dst45src01; // 2 bytes of dest mac addr; 2 bytes of src mac addr
  volatile uint32_t  mac_src2345;    // 4 bytes of destination mac addr
  volatile uint32_t  ethertype_pad;  // ethertype in high 16-bits
} rx_proto_engine_regs_t;

#define rx_proto_engine ((rx_proto_engine_regs_t *) RX_PROTOCOL_ENGINE_BASE)



///////////////////////////////////////////////////
// Simple Programmable Interrupt Controller, Slave 8

#define PIC_BASE  0xD800

// Interrupt request lines
// Bit numbers (LSB == 0) that correpond to interrupts into PIC

#define	IRQ_BUFFER	0	// buffer manager
#define	IRQ_TIMER	1
#define	IRQ_SPI		2
#define	IRQ_I2C		3
#define	IRQ_PHY		4	// ethernet PHY
#define	IRQ_UNDERRUN	5
#define	IRQ_OVERRUN	6
#define	IRQ_PPS		7	// pulse per second
#define	IRQ_UART_RX	8
#define	IRQ_UART_TX	9
#define	IRQ_SERDES	10
#define	IRQ_CLKSTATUS	11

#define IRQ_TO_MASK(x) (1 << (x))

#define PIC_BUFFER_INT    IRQ_TO_MASK(IRQ_BUFFER)
#define PIC_TIMER_INT     IRQ_TO_MASK(IRQ_TIMER)
#define PIC_SPI_INT       IRQ_TO_MASK(IRQ_SPI)
#define PIC_I2C_INT       IRQ_TO_MASK(IRQ_I2C)
#define PIC_PHY_INT       IRQ_TO_MASK(IRQ_PHY)
#define PIC_UNDERRUN_INT  IRQ_TO_MASK(IRQ_UNDERRUN)
#define PIC_OVERRUN_INT   IRQ_TO_MASK(IRQ_OVERRUN)
#define PIC_PPS_INT   	  IRQ_TO_MASK(IRQ_PPS)
#define PIC_UART_RX_INT   IRQ_TO_MASK(IRQ_UART_RX)
#define PIC_UART_TX_INT   IRQ_TO_MASK(IRQ_UART_TX)
#define PIC_SERDES        IRQ_TO_MASK(IRQ_SERDES)
#define PIC_CLKSTATUS     IRQ_TO_MASK(IRQ_CLKSTATUS)

typedef struct {
  volatile uint32_t edge_enable; // mask: 1 -> edge triggered, 0 -> level
  volatile uint32_t polarity;	 // mask: 1 -> rising edge
  volatile uint32_t mask;	 // mask: 1 -> disabled
  volatile uint32_t pending;	 // mask: 1 -> pending; write 1's to clear pending ints
} pic_regs_t;

#define pic_regs ((pic_regs_t *) PIC_BASE)

///////////////////////////////////////////////////
// Timer, Slave 9

#define TIMER_BASE  0xDC00

typedef struct {
  volatile uint32_t time;	// R: current, W: set time to interrupt
} timer_regs_t;

#define timer_regs ((timer_regs_t *) TIMER_BASE)

///////////////////////////////////////////////////
// UART, Slave 10

#define UART_BASE  0xE000

typedef struct {
  //  All elements are 8 bits except for clkdiv (16), but we use uint32 to make 
  //    the hardware for decoding easier
  volatile uint32_t clkdiv;  // Set to 50e6 divided by baud rate (no x16 factor)
  volatile uint32_t txlevel; // Number of spaces in the FIFO for writes
  volatile uint32_t rxlevel; // Number of available elements in the FIFO for reads
  volatile uint32_t txchar;  // Write characters to be sent here
  volatile uint32_t rxchar;  // Read received characters here
} uart_regs_t;

#define uart_regs ((uart_regs_t *) UART_BASE)

///////////////////////////////////////////////////
// ATR Controller, Slave 11

#define ATR_BASE  0xE400

typedef struct {
  volatile uint32_t	v[16];
} atr_regs_t;

#define	ATR_IDLE	0x0	// indicies into v
#define ATR_TX		0x1
#define	ATR_RX		0x2
#define	ATR_FULL	0x3

#define atr_regs ((atr_regs_t *) ATR_BASE)

///////////////////////////////////////////////////
// Time Sync Controller, Slave 12
#define TIMESYNC_BASE  0xE800

typedef struct {
  /*!
   * \brief Time sync configuration.
   *
   * <pre>
   *
   *    3                   2                   1                       
   *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   * +-----------------------------------------------------+-+-+-+-+-+
   * |                                                     |T|G|X|I|S|
   * +-----------------------------------------------------+-+-+-+-+-+
   *
   * S - Tick source (0 = free run, 1 = pps, default=0)
   * I - Tick interrupt enable (not implemented)
   * X - Use external sync source (default=1)
   * G - PPS edge selection (0=negedge, 1=posedge, default=0)
   * T - Trigger sync every pps edge (default=0)
   *
   * </pre>
   */
  volatile uint32_t tick_control;
  volatile uint32_t tick_interval;
  volatile uint32_t delta_time;
  volatile uint32_t sync_on_next_pps;
} timesync_regs_t;

#define timesync_regs ((timesync_regs_t *) TIMESYNC_BASE)

#define TSC_SOURCE_PPS        (1 << 0)
//#define TSC_SOURCE_FREE_RUN (0 << 0)
#define TSC_IENABLE           (1 << 1)
#define TSC_EXTSYNC           (1 << 2)
#define TSC_PPSEDGE_POS       (1 << 3)
//#define TSC_PPSEDGE_NEG     (0 << 3)
#define TSC_TRIGGER_EVERYPPS  (1 << 4)
//#define TSC_TRIGGER_ONCE    (0 << 4)

///////////////////////////////////////////////////
// SD Card SPI interface, Slave 13
//   All regs are 8 bits wide, but are accessed as if they are 32 bits

#define SDSPI_BASE  0xEC00

typedef struct {
  volatile uint32_t status;  // Write a 1 or 0 for controlling CS
  volatile uint32_t clkdiv;
  volatile uint32_t send_dat;
  volatile uint32_t receive_dat;
} sdspi_regs_t;

#define sdspi_regs ((sdspi_regs_t *) SDSPI_BASE)

///////////////////////////////////////////////////
// External RAM interface, Slave 14
//   Pages are 1K.  Page is 10 bits, set by a control register
//    output_regs->ram_page

#define EXTRAM_BASE 0xF000
#define extram ((volatile uint32_t *) EXTRAM_BASE)


///////////////////////////////////////////////////

#endif

