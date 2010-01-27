/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "u2_init.h"
#include "memory_map.h"
#include "spi.h"
#include "hal_io.h"
#include "buffer_pool.h"
#include "pic.h"
#include <stdbool.h>
#include "ethernet.h"
#include "nonstdio.h"
#include "usrp2_eth_packet.h"
#include "usrp2_ipv4_packet.h"
#include "usrp2_udp_packet.h"
#include "dbsm.h"
#include "app_common_v2.h"
#include "memcpy_wa.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "clocks.h"
#include <vrt/bits.h>

#define FW_SETS_SEQNO	1	// define to 0 or 1 (FIXME must be 1 for now)

#if (FW_SETS_SEQNO)
static int fw_seqno;	// used when f/w is filling in sequence numbers
#endif


/*
 * Full duplex Tx and Rx between ethernet and DSP pipelines
 *
 * Buffer 1 is used by the cpu to send frames to the host.
 * Buffers 2 and 3 are used to double-buffer the DSP Rx to eth flow
 * Buffers 4 and 5 are used to double-buffer the eth to DSP Tx  eth flow
 */
//#define CPU_RX_BUF	0	// eth -> cpu

#define	DSP_RX_BUF_0	2	// dsp rx -> eth (double buffer)
#define	DSP_RX_BUF_1	3	// dsp rx -> eth
#define	DSP_TX_BUF_0	4	// eth -> dsp tx (double buffer)
#define	DSP_TX_BUF_1	5	// eth -> dsp tx

/*
 * ================================================================
 *   configure DSP TX double buffering state machine (eth -> dsp)
 * ================================================================
 */

// DSP Tx reads ethernet header words
#define DSP_TX_FIRST_LINE ((sizeof(u2_eth_ip_udp_t))/4)

// Receive from ethernet
buf_cmd_args_t dsp_tx_recv_args = {
  PORT_ETH,
  0,
  BP_LAST_LINE
};

// send to DSP Tx
buf_cmd_args_t dsp_tx_send_args = {
  PORT_DSP,
  DSP_TX_FIRST_LINE,	// starts just past transport header
  0			// filled in from last_line register
};

dbsm_t dsp_tx_sm;	// the state machine

/*
 * ================================================================
 *   configure DSP RX double buffering state machine (dsp -> eth)
 * ================================================================
 */

// DSP Rx writes ethernet header words
#define DSP_RX_FIRST_LINE 1 //1 = control stuff to udp sm

// receive from DSP
buf_cmd_args_t dsp_rx_recv_args = {
  PORT_DSP,
  DSP_RX_FIRST_LINE,
  BP_LAST_LINE
};

// send to ETH
buf_cmd_args_t dsp_rx_send_args = {
  PORT_ETH,
  0,		// starts with ethernet header in line 0
  0,		// filled in from list_line register
};

dbsm_t dsp_rx_sm;	// the state machine


// The mac address of the host we're sending to.
eth_mac_addr_t host_mac_addr;

#define TIME_NOW ((uint32_t)(~0))

// variables for streaming mode

static bool         streaming_p = false;
static unsigned int streaming_items_per_frame = 0;
static uint32_t     time_secs = TIME_NOW;
static uint32_t     time_ticks = TIME_NOW;
static int          streaming_frame_count = 0;
#define FRAMES_PER_CMD	2

bool is_streaming(void){ return streaming_p; }


// ----------------------------------------------------------------

#define VRT_HEADER_WORDS 5
#define VRT_TRAILER_WORDS 1

void
restart_streaming(void)
{
  // setup RX DSP regs
  sr_rx_ctrl->nsamples_per_pkt = streaming_items_per_frame;
  sr_rx_ctrl->nchannels = 1;
  sr_rx_ctrl->clear_overrun = 1;			// reset
  sr_rx_ctrl->vrt_header = (0
     | VRTH_PT_IF_DATA_WITH_SID
     | VRTH_HAS_TRAILER
     | VRTH_TSI_OTHER
     | VRTH_TSF_SAMPLE_CNT
     | (VRT_HEADER_WORDS+streaming_items_per_frame+VRT_TRAILER_WORDS));
  sr_rx_ctrl->vrt_stream_id = 0;
  sr_rx_ctrl->vrt_trailer = 0;

  streaming_p = true;
  streaming_frame_count = FRAMES_PER_CMD;

  sr_rx_ctrl->cmd =
    MK_RX_CMD(FRAMES_PER_CMD * streaming_items_per_frame,
    (time_ticks==TIME_NOW)?1:0, 1);  // conditionally set "now" bit, set "chain" bit

  // kick off the state machine
  dbsm_start(&dsp_rx_sm);

  sr_rx_ctrl->time_secs = time_secs;
  sr_rx_ctrl->time_ticks = time_ticks;		// enqueue first of two commands

  // make sure this one and the rest have the "now" and "chain" bits set.
  sr_rx_ctrl->cmd =
    MK_RX_CMD(FRAMES_PER_CMD * streaming_items_per_frame, 1, 1);

  sr_rx_ctrl->time_secs = 0;
  sr_rx_ctrl->time_ticks = 0;		// enqueue second command
}

/*
 * 1's complement sum for IP and UDP headers
 *
 * init chksum to zero to start.
 */
static unsigned int
CHKSUM(unsigned int x, unsigned int *chksum)
{
  *chksum += x;
  *chksum = (*chksum & 0xffff) + (*chksum>>16);
  *chksum = (*chksum & 0xffff) + (*chksum>>16);
  return x;
}

void
start_rx_streaming_cmd(op_start_rx_streaming_t *p)
{
  /*
   * Construct  ethernet header and preload into two buffers
   */
  struct {
    uint32_t ctrl_word;
  } mem _AL4;

  memset(&mem, 0, sizeof(mem));
  p->items_per_frame = (1500 - sizeof(u2_eth_ip_udp_t))/sizeof(uint32_t) - (VRT_HEADER_WORDS + VRT_TRAILER_WORDS); //FIXME
  mem.ctrl_word = (VRT_HEADER_WORDS+p->items_per_frame+VRT_TRAILER_WORDS)*sizeof(uint32_t) | 1 << 16;

  memcpy_wa(buffer_ram(DSP_RX_BUF_0), &mem, sizeof(mem));
  memcpy_wa(buffer_ram(DSP_RX_BUF_1), &mem, sizeof(mem));

  //setup ethernet header machine
  sr_udp_sm->eth_hdr.mac_dst_0_1 = (host_dst_mac_addr.addr[0] << 8) | host_dst_mac_addr.addr[1];
  sr_udp_sm->eth_hdr.mac_dst_2_3 = (host_dst_mac_addr.addr[2] << 8) | host_dst_mac_addr.addr[3];
  sr_udp_sm->eth_hdr.mac_dst_4_5 = (host_dst_mac_addr.addr[4] << 8) | host_dst_mac_addr.addr[5];
  sr_udp_sm->eth_hdr.mac_src_0_1 = (host_src_mac_addr.addr[0] << 8) | host_src_mac_addr.addr[1];
  sr_udp_sm->eth_hdr.mac_src_2_3 = (host_src_mac_addr.addr[2] << 8) | host_src_mac_addr.addr[3];
  sr_udp_sm->eth_hdr.mac_src_4_5 = (host_src_mac_addr.addr[4] << 8) | host_src_mac_addr.addr[5];
  sr_udp_sm->eth_hdr.ether_type = ETHERTYPE_IPV4;

  //setup ip header machine
  unsigned int chksum = 0;
  sr_udp_sm->ip_hdr.ver_ihl_tos = CHKSUM(0x4500, &chksum);    // IPV4,  5 words of header (20 bytes), TOS=0
  sr_udp_sm->ip_hdr.total_length = UDP_SM_INS_IP_LEN;             // Don't checksum this line in SW
  sr_udp_sm->ip_hdr.identification = CHKSUM(0x0000, &chksum);    // ID
  sr_udp_sm->ip_hdr.flags_frag_off = CHKSUM(0x4000, &chksum);    // don't fragment
  sr_udp_sm->ip_hdr.ttl_proto = CHKSUM(0x2011, &chksum);    // TTL=32, protocol = UDP (17 decimal)
  //sr_udp_sm->ip_hdr.checksum .... filled in below
  uint32_t src_ip_addr = host_src_ip_addr.s_addr;
  uint32_t dst_ip_addr = host_dst_ip_addr.s_addr;
  sr_udp_sm->ip_hdr.src_addr_high = CHKSUM(src_ip_addr >> 16, &chksum);    // IP src high
  sr_udp_sm->ip_hdr.src_addr_low = CHKSUM(src_ip_addr & 0xffff, &chksum); // IP src low
  sr_udp_sm->ip_hdr.dst_addr_high = CHKSUM(dst_ip_addr >> 16, &chksum);    // IP dst high
  sr_udp_sm->ip_hdr.dst_addr_low = CHKSUM(dst_ip_addr & 0xffff, &chksum); // IP dst low
  sr_udp_sm->ip_hdr.checksum = UDP_SM_INS_IP_HDR_CHKSUM | (chksum & 0xffff);

  //setup the udp header machine
  sr_udp_sm->udp_hdr.src_port = host_src_udp_port;
  sr_udp_sm->udp_hdr.dst_port = host_dst_udp_port;
  sr_udp_sm->udp_hdr.length = UDP_SM_INS_UDP_LEN;
  sr_udp_sm->udp_hdr.checksum = UDP_SM_LAST_WORD;		// zero UDP checksum

  if (FW_SETS_SEQNO)
    fw_seqno = 0;

  streaming_items_per_frame = p->items_per_frame;
  time_secs = p->time_secs;
  time_ticks = p->time_ticks;
  restart_streaming();
}

void
stop_rx_cmd(void)
{
  if (is_streaming()){
    streaming_p = false;

    // no samples, "now", not chained
    sr_rx_ctrl->cmd = MK_RX_CMD(0, 1, 0);

    sr_rx_ctrl->time_secs = 0;
    sr_rx_ctrl->time_ticks = 0;	// enqueue command
  }

}


static void
setup_tx()
{
  sr_tx_ctrl->clear_state = 1;
  bp_clear_buf(DSP_TX_BUF_0);
  bp_clear_buf(DSP_TX_BUF_1);

  int tx_scale = 256;
  int interp = 32;

  // setup some defaults

  dsp_tx_regs->freq = 0;
  dsp_tx_regs->scale_iq = (tx_scale << 16) | tx_scale;
  dsp_tx_regs->interp_rate = interp;
}


#if (FW_SETS_SEQNO)
/*
 * Debugging ONLY.  This will be handled by the tx_protocol_engine.
 *
 * This is called when the DSP Rx chain has filled in a packet.
 * We set and increment the seqno, then return false, indicating
 * that we didn't handle the packet.  A bit of a kludge
 * but it should work.
 */
bool 
fw_sets_seqno_inspector(dbsm_t *sm, int buf_this)	// returns false
{
  // queue up another rx command when required
  if (streaming_p && --streaming_frame_count == 0){
    streaming_frame_count = FRAMES_PER_CMD;
    sr_rx_ctrl->time_secs = 0;
    sr_rx_ctrl->time_ticks = 0;
  }

  return false;		// we didn't handle the packet
}
#endif


inline static void
buffer_irq_handler(unsigned irq)
{
  uint32_t  status = buffer_pool_status->status;

  dbsm_process_status(&dsp_tx_sm, status);
  dbsm_process_status(&dsp_rx_sm, status);
}

int
main(void)
{
  u2_init();

  putstr("\nTxRx-NEWETH\n");
  print_mac_addr(ethernet_mac_addr()->addr);
  newline();

  ethernet_register_link_changed_callback(link_changed_callback);
  ethernet_init();

#if 0
  // make bit 15 of Tx gpio's be a s/w output
  hal_gpio_set_sel(GPIO_TX_BANK, 15, 's');
  hal_gpio_set_ddr(GPIO_TX_BANK, 0x8000, 0x8000);
#endif

  output_regs->debug_mux_ctrl = 1;
#if 0
  hal_gpio_set_sels(GPIO_TX_BANK, "1111111111111111");
  hal_gpio_set_sels(GPIO_RX_BANK, "1111111111111111");
  hal_gpio_set_ddr(GPIO_TX_BANK, 0xffff, 0xffff);
  hal_gpio_set_ddr(GPIO_RX_BANK, 0xffff, 0xffff);
#endif


  // initialize double buffering state machine for ethernet -> DSP Tx

  dbsm_init(&dsp_tx_sm, DSP_TX_BUF_0,
	    &dsp_tx_recv_args, &dsp_tx_send_args,
	    eth_pkt_inspector);


  // initialize double buffering state machine for DSP RX -> Ethernet

  if (FW_SETS_SEQNO){
    dbsm_init(&dsp_rx_sm, DSP_RX_BUF_0,
	      &dsp_rx_recv_args, &dsp_rx_send_args,
	      fw_sets_seqno_inspector);
  }
  else {
    dbsm_init(&dsp_rx_sm, DSP_RX_BUF_0,
	      &dsp_rx_recv_args, &dsp_rx_send_args,
	      dbsm_nop_inspector);
  }

  // tell app_common that this dbsm could be sending to the ethernet
  ac_could_be_sending_to_eth = &dsp_rx_sm;


  // program tx registers
  setup_tx();

  // kick off the state machine
  dbsm_start(&dsp_tx_sm);

  //int which = 0;

  while(1){
    // hal_gpio_write(GPIO_TX_BANK, which, 0x8000);
    // which ^= 0x8000;

    buffer_irq_handler(0);

    int pending = pic_regs->pending;		// poll for under or overrun

    if (pending & PIC_UNDERRUN_INT){
      dbsm_handle_tx_underrun(&dsp_tx_sm);
      pic_regs->pending = PIC_UNDERRUN_INT;	// clear interrupt
      putchar('U');
    }

    if (pending & PIC_OVERRUN_INT){
      dbsm_handle_rx_overrun(&dsp_rx_sm);
      pic_regs->pending = PIC_OVERRUN_INT;	// clear pending interrupt

      // FIXME Figure out how to handle this robustly.
      // Any buffers that are emptying should be allowed to drain...

      if (streaming_p){
	// restart_streaming();
	// FIXME report error
      }
      else {
	// FIXME report error
      }
      putchar('O');
    }
  }
}

//-------------------compile time checks--------------------------------
#define COMPILE_TIME_ASSERT(pred) switch(0){case 0:case pred:;}

void compile_time_checks(void){
    COMPILE_TIME_ASSERT(sizeof(u2_eth_hdr_t) == 14);
    COMPILE_TIME_ASSERT(sizeof(u2_ipv4_hdr_t) == 20);
    COMPILE_TIME_ASSERT(sizeof(u2_udp_hdr_t) == 8);
}
