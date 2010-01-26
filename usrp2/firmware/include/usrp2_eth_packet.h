/* -*- c++ -*- */
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

#ifndef INCLUDED_USRP2_ETH_PACKET_H
#define INCLUDED_USRP2_ETH_PACKET_H

#include "usrp2_cdefs.h"
#include "network.h"

__U2_BEGIN_DECLS

#define U2_DATA_ETHERTYPE 		0xBEEF	// used in our data frames
#define U2_CTRL_ETHERTYPE 		0xBEF0	// used in our control frames
#define	MAC_CTRL_ETHERTYPE	0x8808	// used in PAUSE frames
#define	ETHERTYPE_IPV4      0x0800	// used in ip packets

/*
 * All these data structures are BIG-ENDIAN on the wire
 */

// FIXME gcc specific.  Really ought to come from compiler.h
#define _AL4   __attribute__((aligned (4)))

/*!
 * \brief The classic 14-byte ethernet header
 */
typedef struct {
  eth_mac_addr_t dst;
  eth_mac_addr_t src;
  uint16_t 	ethertype;
} __attribute__((packed)) u2_eth_hdr_t;

/*!
 * \brief consolidated packet: ethernet header
 */
typedef struct {
  u2_eth_hdr_t		ehdr;
} u2_eth_packet_t;

/*!
 * \brief consolidated packet: padding + ethernet header
 */
typedef struct {
  uint16_t          padding;
  u2_eth_hdr_t		ehdr;
} u2_eth_packet_pad_before_t;

/*
 * Opcodes for control channel
 *
 * Reply opcodes are the same as the request opcode with the OP_REPLY_BIT set (0x80).
 */
#define OP_REPLY_BIT	          0x80

#define	OP_EOP			     0	// marks last subpacket in packet

#define OP_ID			     1
#define	OP_ID_REPLY		     (OP_ID | OP_REPLY_BIT)
#define	OP_BURN_MAC_ADDR	     2
#define OP_BURN_MAC_ADDR_REPLY	     (OP_BURN_MAC_ADDR | OP_REPLY_BIT)
#define	OP_CONFIG_RX_V2	             4
#define	OP_CONFIG_RX_REPLY_V2        (OP_CONFIG_RX_V2 | OP_REPLY_BIT)
#define	OP_CONFIG_TX_V2	             5
#define	OP_CONFIG_TX_REPLY_V2        (OP_CONFIG_TX_V2 | OP_REPLY_BIT)
#define	OP_START_RX_STREAMING        6
#define	OP_START_RX_STREAMING_REPLY  (OP_START_RX_STREAMING | OP_REPLY_BIT)
#define	OP_STOP_RX	             7
#define	OP_STOP_RX_REPLY	     (OP_STOP_RX | OP_REPLY_BIT)
#define	OP_CONFIG_CLOCK              8
#define OP_CONFIG_CLOCK_REPLY        (OP_CONFIG_CLOCK | OP_REPLY_BIT)
#define	OP_DBOARD_INFO		     9
#define	OP_DBOARD_INFO_REPLY	     (OP_DBOARD_INFO | OP_REPLY_BIT)
#define OP_PEEK                      11
#define OP_PEEK_REPLY                (OP_PEEK | OP_REPLY_BIT)
#define OP_POKE                      12
#define OP_POKE_REPLY                (OP_POKE | OP_REPLY_BIT)
#define OP_SET_TX_LO_OFFSET          13
#define OP_SET_TX_LO_OFFSET_REPLY    (OP_SET_TX_LO_OFFSET | OP_REPLY_BIT)
#define OP_SET_RX_LO_OFFSET          14
#define OP_SET_RX_LO_OFFSET_REPLY    (OP_SET_RX_LO_OFFSET | OP_REPLY_BIT)
#define OP_RESET_DB                  15
#define OP_RESET_DB_REPLY            (OP_RESET_DB | OP_REPLY_BIT)
#define OP_GPIO_SET_DDR              17
#define OP_GPIO_SET_DDR_REPLY        (OP_GPIO_SET_DDR | OP_REPLY_BIT)
#define OP_GPIO_SET_SELS             18
#define OP_GPIO_SET_SELS_REPLY       (OP_GPIO_SET_SELS | OP_REPLY_BIT)
#define OP_GPIO_READ                 19
#define OP_GPIO_READ_REPLY           (OP_GPIO_READ | OP_REPLY_BIT)
#define OP_GPIO_WRITE                20
#define OP_GPIO_WRITE_REPLY          (OP_GPIO_WRITE | OP_REPLY_BIT)
#define OP_GPIO_STREAM               21
#define OP_GPIO_STREAM_REPLY         (OP_GPIO_STREAM | OP_REPLY_BIT)
#define OP_SET_TIME                  22
#define OP_SET_TIME_REPLY            (OP_SET_TIME | OP_REPLY_BIT)

/*
 * All subpackets are a multiple of 4 bytes long.
 * All subpackets start with an 8-bit opcode, an 8-bit len and an 8-bit rid.
 */
#define MAX_SUBPKT_LEN 252

/*!
 * \brief Generic request and reply packet
 *
 * Used by:
 *  OP_EOP, OP_BURN_MAC_ADDR_REPLY, OP_START_RX_STREAMING_REPLY,
 *  OP_STOP_RX_REPLY, OP_DBOARD_INFO, OP_SYNC_TO_PPS
 */
typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	ok;		// bool
} _AL4 op_generic_t;

/*!
 * \brief Set the ticks and secs on a usrp2
 */
typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	type;
  uint32_t	time_secs;
  uint32_t	time_ticks;
} _AL4 op_set_time_t;

typedef enum {
    OP_SET_TIME_TYPE_NOW,
    OP_SET_TIME_TYPE_PPS
} op_set_time_type_t;

/*!
 * \brief Reply info from a USRP2
 */
typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	mbz;
  eth_mac_addr_t	addr;
  uint16_t	hw_rev;
  uint8_t	fpga_md5sum[16];
  uint8_t	sw_md5sum[16];
} _AL4 op_id_reply_t;

typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	mbz;
  uint32_t	items_per_frame;  // # of 32-bit data items; MTU=1500: [9,371]
  uint32_t	time_secs;
  uint32_t	time_ticks;
} _AL4 op_start_rx_streaming_t;

typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  eth_mac_addr_t	addr;
} _AL4 op_burn_mac_addr_t;

/*!
 * \brief Configure receiver
 */
typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	mbz;
  // bitmask indicating which of the following fields are valid
  uint16_t	valid;
  uint16_t	gain;		// fxpt_db (Q9.7)
  uint32_t	freq_hi;	// high 32-bits of 64-bit fxpt_freq (Q44.20)
  uint32_t	freq_lo;	// low  32-bits of 64-bit fxpt_freq (Q44.20)
  uint32_t	decim;		// desired decimation factor (NOT -1)
  uint32_t	scale_iq;	// (scale_i << 16) | scale_q [16.0 format]
} _AL4 op_config_rx_v2_t;

// bitmask for "valid" field.  If the bit is set, there's
// meaningful data in the corresonding field.

#define	CFGV_GAIN		0x0001	// gain field is valid
#define	CFGV_FREQ		0x0002	// target_freq field is valid
#define	CFGV_INTERP_DECIM	0x0004	// interp or decim is valid
#define	CFGV_SCALE_IQ		0x0008	// scale_iq is valid

/*!
 * \brief Reply to receiver configuration
 */
typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	mbz;

  uint16_t	ok;		// config was successful (bool)
  uint16_t	inverted;	// spectrum is inverted (bool)

  // RF frequency that corresponds to DC in the IF (fxpt_freq)
  uint32_t	baseband_freq_hi;
  uint32_t	baseband_freq_lo;
  // DDC frequency (fxpt_freq)
  uint32_t	ddc_freq_hi;
  uint32_t	ddc_freq_lo;
  // residual frequency (fxpt_freq)
  uint32_t	residual_freq_hi;
  uint32_t	residual_freq_lo;

} _AL4 op_config_rx_reply_v2_t;

/*!
 *  \brief Configure transmitter
 */
typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	mbz;

  // bitmask indicating which of the following fields are valid
  uint16_t	valid;
  uint16_t	gain;		// fxpt_db (Q9.7)
  uint32_t	freq_hi;	// high 32-bits of 64-bit fxpt_freq (Q44.20)
  uint32_t	freq_lo;	// low  32-bits of 64-bit fxpt_freq (Q44.20)
  uint32_t	interp;		// desired interpolation factor (NOT -1)
  uint32_t	scale_iq;	// (scale_i << 16) | scale_q [16.0 format]
} _AL4 op_config_tx_v2_t;

/*!
 * \brief Reply to configure transmitter
 */
typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	mbz;

  uint16_t	ok;		// config was successful (bool)
  uint16_t	inverted;	// spectrum is inverted (bool)

  // RF frequency that corresponds to DC in the IF (fxpt_freq)
  uint32_t	baseband_freq_hi;
  uint32_t	baseband_freq_lo;
  // DUC frequency (fxpt_freq)
  uint32_t	duc_freq_hi;
  uint32_t	duc_freq_lo;
  // residual frequency (fxpt_freq)
  uint32_t	residual_freq_hi;
  uint32_t	residual_freq_lo;

} _AL4 op_config_tx_reply_v2_t;

/*!
 * \brief Configure clocking, etc (uses generic reply)
 */
typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	flags;
} op_config_clock_t;

/*!
 * \brief High-level information about daughterboards
 */
typedef struct {
  int32_t	dbid;		//< d'board ID (-1 none, -2 invalid eeprom)
  uint32_t	freq_min_hi;	//< high 32-bits of 64-bit fxpt_freq (Q44.20)
  uint32_t	freq_min_lo;	//< low  32-bits of 64-bit fxpt_freq (Q44.20)
  uint32_t	freq_max_hi;	//< high 32-bits of 64-bit fxpt_freq (Q44.20)
  uint32_t	freq_max_lo;	//< low  32-bits of 64-bit fxpt_freq (Q44.20)
  uint16_t	gain_min;	//< min gain that can be set. fxpt_db (Q9.7)
  uint16_t	gain_max;	//< max gain that can be set. fxpt_db (Q9.7)
  uint16_t	gain_step_size;	//< fxpt_db (Q9.7)
} u2_db_info_t;


/*!
 * \brief Reply to d'board info request
 */
typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	ok;		// request was successful (bool)

  u2_db_info_t	tx_db_info;
  u2_db_info_t	rx_db_info;
} _AL4 op_dboard_info_reply_t;

/*!
 * \brief Read from Wishbone memory
 */
typedef struct {
  uint8_t       opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	mbz;
  uint32_t      addr;
  uint32_t      bytes;
} _AL4 op_peek_t;

/*!
 * \brief Write to Wishbone memory
 */
typedef struct {
  uint8_t       opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	mbz;
  uint32_t      addr;
  // Words follow here
} _AL4 op_poke_t;

/* 
 * Common structure for commands with a single frequency param 
 * (e.g., set_*_lo_offset, set_*_bw)
 */
typedef struct {
  uint8_t       opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	mbz;
  uint32_t	freq_hi;	//< high 32-bits of 64-bit fxpt_freq (Q44.20)
  uint32_t	freq_lo;	//< low  32-bits of 64-bit fxpt_freq (Q44.20)
} _AL4 op_freq_t;

/*
 * Structures for commands in GPIO system
 */
typedef struct {
  uint8_t       opcode;		// OP_GPIO_SET_DDR, OP_GPIO_WRITE, OP_GPIO_STREAM
  uint8_t       len;
  uint8_t       rid;
  uint8_t       bank;
  uint16_t      value;
  uint16_t      mask;
} _AL4 op_gpio_t;

typedef struct {
  uint8_t       opcode;		// OP_GPIO_SET_SELS
  uint8_t       len;
  uint8_t       rid;
  uint8_t       bank;
  uint8_t       sels[16];
} _AL4 op_gpio_set_sels_t;

typedef struct {
  uint8_t	opcode;		// OP_GPIO_READ_REPLY
  uint8_t	len;
  uint8_t	rid;
  uint8_t	ok;
  uint16_t      mbz;
  uint16_t	value;
} _AL4 op_gpio_read_reply_t;

/*
 * ================================================================
 *             union of all of subpacket types
 * ================================================================
 */
typedef union {

  op_generic_t			op_generic;
  op_id_reply_t			op_id_reply;
  op_start_rx_streaming_t	op_start_rx_streaming;
  op_burn_mac_addr_t		op_burn_mac_addr;
  op_config_rx_v2_t		op_config_rx_v2;
  op_config_rx_reply_v2_t	op_config_rx_reply_v2;
  op_config_tx_v2_t		op_config_tx_v2;
  op_config_tx_reply_v2_t	op_config_tx_reply_v2;
  op_config_clock_t 		op_config_clock;
  op_peek_t                     op_peek;
  op_poke_t                     op_poke;
  op_freq_t                     op_freq;
  op_gpio_t                     op_gpio;
  op_gpio_set_sels_t            op_gpio_set_sels;
  op_gpio_read_reply_t          op_gpio_read_reply;
  op_set_time_t                 op_set_time;

} u2_subpkt_t;


__U2_END_DECLS

#endif /* INCLUDED_USRP2_ETH_PACKET_H */
