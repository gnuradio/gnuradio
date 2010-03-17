/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2009,2010 Free Software Foundation, Inc.
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
#include "usrp2_bytesex.h"
#include "usrp2_mac_addr.h"
#include "usrp2_mimo_config.h"

__U2_BEGIN_DECLS

#define U2_ETHERTYPE 		0xBEEF	// used in our frames
#define	MAC_CTRL_ETHERTYPE	0x8808	// used in PAUSE frames

/*
 * All these data structures are BIG-ENDIAN on the wire
 */

// FIXME gcc specific.  Really ought to come from compiler.h
#define _AL4   __attribute__((aligned (4)))

/*
 * \brief The classic 14-byte ethernet header
 */
typedef struct {
  u2_mac_addr_t dst;
  u2_mac_addr_t src;
  uint16_t 	ethertype;
} __attribute__((packed)) u2_eth_hdr_t;

/*!
 * \brief USRP2 transport header
 *
 * This enables host->usrp2 flow control and dropped packet detection.
 */
typedef struct {
  uint16_t	flags;		// MBZ, may be used for channel in future
  uint16_t	fifo_status;	// free space in Rx fifo in 32-bit lines
  uint8_t	seqno;		// sequence number of this packet
  uint8_t	ack;		// sequence number of next packet expected
} __attribute__((packed)) u2_transport_hdr_t;


/*
 * The fixed payload header of a USRP2 ethernet packet...
 *
 * Basically there's 1 word of flags and routing info, and 1 word
 * of timestamp that specifies when the data was received, or
 * when it should be transmitted. The data samples follow immediately.
 *
 * Transmit packets (from host to U2)
 * 
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |  Chan   |                    mbz                        |I|S|E|
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                           Timestamp                           |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 * Received packets (from U2 to host)
 *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |  Chan   |                    mbz                              |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                           Timestamp                           |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *  mbz == must be zero
 */

typedef struct {
  uint32_t	word0;		// flags etc
  uint32_t	timestamp;	// time of rx or tx (100 MHz)
} u2_fixed_hdr_t;


#define U2P_CHAN_MASK		0x1f
#define	U2P_CHAN_SHIFT		27

#define U2P_TX_IMMEDIATE	0x00000004  // send samples NOW, else at timestamp
#define U2P_TX_START_OF_BURST	0x00000002  // this frame is the start of a burst
#define	U2P_TX_END_OF_BURST	0x00000001  // this frame is the end of a burst

#define	U2P_ALL_FLAGS		0x00000007

#define	CONTROL_CHAN		0x1f

static inline int
u2p_chan(u2_fixed_hdr_t *p)
{
  return (ntohl(p->word0) >> U2P_CHAN_SHIFT) & U2P_CHAN_MASK;
}

inline static uint32_t
u2p_word0(u2_fixed_hdr_t *p)
{
  return ntohl(p->word0);
}

inline static uint32_t
u2p_timestamp(u2_fixed_hdr_t *p)
{
  return ntohl(p->timestamp);
}

inline static void
u2p_set_word0(u2_fixed_hdr_t *p, int flags, int chan)
{
  p->word0 = htonl((flags & U2P_ALL_FLAGS)
		   | ((chan & U2P_CHAN_MASK) << U2P_CHAN_SHIFT));
}

inline static void
u2p_set_timestamp(u2_fixed_hdr_t *p, uint32_t ts)
{
  p->timestamp = htonl(ts);
}

/*!
 * \brief consolidated packet: ethernet header + transport header + fixed header
 */
typedef struct {
  u2_eth_hdr_t		ehdr;
  u2_transport_hdr_t	thdr;
  u2_fixed_hdr_t	fixed;
} u2_eth_packet_t;

/*
 * full load of samples:
 *   ethernet header + transport header + fixed header + maximum number of samples.
 *   sizeof(u2_eth_samples_t) == 1512
 *   (payload is 1498 bytes, two bytes shorter than 1500 byte MTU)
 *   (sample numbers are made even to force pairwise alignment in the interleaved case)
 */

#define U2_MAX_SAMPLES	370
#define	U2_MIN_SAMPLES   10

typedef struct {
  u2_eth_packet_t	hdrs;
  uint32_t		samples[U2_MAX_SAMPLES];
} u2_eth_samples_t;

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
#define	OP_READ_TIME	             3	// What time is it? (100 MHz counter)
#define	OP_READ_TIME_REPLY           (OP_READ_TIME | OP_REPLY_BIT)
#define	OP_CONFIG_RX_V2	             4
#define	OP_CONFIG_RX_REPLY_V2        (OP_CONFIG_RX_V2 | OP_REPLY_BIT)
#define	OP_CONFIG_TX_V2	             5
#define	OP_CONFIG_TX_REPLY_V2        (OP_CONFIG_TX_V2 | OP_REPLY_BIT)
#define	OP_START_RX_STREAMING        6
#define	OP_START_RX_STREAMING_REPLY  (OP_START_RX_STREAMING | OP_REPLY_BIT)
#define	OP_STOP_RX	             7
#define	OP_STOP_RX_REPLY	     (OP_STOP_RX | OP_REPLY_BIT)
#define	OP_CONFIG_MIMO	       	     8
#define OP_CONFIG_MIMO_REPLY	     (OP_CONFIG_MIMO | OP_REPLY_BIT)
#define	OP_DBOARD_INFO		     9
#define	OP_DBOARD_INFO_REPLY	     (OP_DBOARD_INFO | OP_REPLY_BIT)
#define	OP_SYNC_TO_PPS               10
#define	OP_SYNC_TO_PPS_REPLY	     (OP_SYNC_TO_PPS | OP_REPLY_BIT)
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
#define OP_SYNC_EVERY_PPS            16
#define OP_SYNC_EVERY_PPS_REPLY      (OP_SYNC_EVERY_PPS | OP_REPLY_BIT)
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
#define OP_RX_ANTENNA                22
#define OP_RX_ANTENNA_REPLY          (OP_RX_ANTENNA | OP_REPLY_BIT)
#define OP_TX_ANTENNA                23
#define OP_TX_ANTENNA_REPLY          (OP_RX_ANTENNA | OP_REPLY_BIT)

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
 * \brief Reply info from a USRP2
 */
typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	mbz;
  u2_mac_addr_t	addr;
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
} _AL4 op_start_rx_streaming_t;

typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  u2_mac_addr_t	addr;
} _AL4 op_burn_mac_addr_t;

typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	mbz;
  uint32_t	time;
} _AL4 op_read_time_reply_t;


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
 * \brief Configure MIMO clocking, etc (uses generic reply)
 */
typedef struct {
  uint8_t	opcode;
  uint8_t	len;
  uint8_t	rid;
  uint8_t	flags;	// from usrp_mimo_config.h
} op_config_mimo_t;


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
  op_read_time_reply_t		op_read_time_reply;
  op_config_rx_v2_t		op_config_rx_v2;
  op_config_rx_reply_v2_t	op_config_rx_reply_v2;
  op_config_tx_v2_t		op_config_tx_v2;
  op_config_tx_reply_v2_t	op_config_tx_reply_v2;
  op_config_mimo_t 		op_config_mimo;
  op_peek_t                     op_peek;
  op_poke_t                     op_poke;
  op_freq_t                     op_freq;
  op_gpio_t                     op_gpio;
  op_gpio_set_sels_t            op_gpio_set_sels;
  op_gpio_read_reply_t          op_gpio_read_reply;

} u2_subpkt_t;


__U2_END_DECLS

#endif /* INCLUDED_USRP2_ETH_PACKET_H */
