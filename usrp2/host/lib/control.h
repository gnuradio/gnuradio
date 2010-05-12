/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2010 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CONTROL_H
#define INCLUDED_CONTROL_H

#include <gruel/thread.h>
#include <usrp2_eth_packet.h>

namespace usrp2 {

  struct op_generic_cmd {
    u2_eth_packet_t h;
    op_generic_t    op;
    op_generic_t    eop;
  };

  /*!
   * OP_CONFIG_RX_V2 command packet
   */
  struct op_config_rx_v2_cmd
  {
    u2_eth_packet_t   h;
    op_config_rx_v2_t op;
    op_generic_t      eop;
  };

  struct op_start_rx_streaming_cmd
  {
    u2_eth_packet_t	    h;
    op_start_rx_streaming_t op;
    op_generic_t	    eop;
  };

  struct op_sync_and_start_rx_streaming_cmd
  {
    u2_eth_packet_t	    h;
    op_generic_t            sync_op;
    op_start_rx_streaming_t rx_op;
    op_generic_t	    eop;
  };

  struct op_stop_rx_cmd {
    u2_eth_packet_t h;
    op_generic_t    op;
    op_generic_t    eop;
  };

  struct op_config_tx_v2_cmd
  {
    u2_eth_packet_t   h;
    op_config_tx_v2_t op;
    op_generic_t      eop;
  };

  struct op_config_mimo_cmd
  {
    u2_eth_packet_t   h;
    op_config_mimo_t  op;
    op_generic_t      eop;
  };

  struct op_burn_mac_addr_cmd
  {
    u2_eth_packet_t    h;
    op_burn_mac_addr_t op;
    op_generic_t       eop;
  };

  struct op_dboard_info_cmd {
    u2_eth_packet_t h;
    op_generic_t    op;
    op_generic_t    eop;
  };

  struct op_peek_cmd {
    u2_eth_packet_t h;
    op_peek_t       op;
    op_generic_t    eop;
  };

  struct op_poke_cmd {
    u2_eth_packet_t h;
    op_poke_t       op;
    // words to write go here
    // eop must be dynamically written here
  };

  struct op_freq_cmd {
    u2_eth_packet_t h;
    op_freq_t       op;
    op_generic_t    eop;
  };

  struct op_gpio_cmd {
    u2_eth_packet_t h;
    op_gpio_t       op;
    op_generic_t    eop;
  };

  struct op_gpio_set_sels_cmd {
    u2_eth_packet_t h;
    op_gpio_set_sels_t op;
    op_generic_t    eop;
  };

  /*!
   * Control mechanism to allow API calls to block waiting for reply packets
   */
  class pending_reply
  {
  private:
    unsigned int    d_rid;
    void           *d_buffer;
    size_t	    d_len;

    // d_mutex is used with d_cond and also protects d_complete
    gruel::mutex      d_mutex;
    gruel::condition_variable d_cond;
    bool	    d_complete;

  public:
    /*!
     * Construct a pending reply from the reply ID, response packet
     * buffer, and buffer length.
     */
    pending_reply(unsigned int rid, void *buffer, size_t len);

    /*!
     * Destructor. Signals creating thread.
     */
    ~pending_reply();

    /*!
     * Block, waiting for reply packet.
     * Returns: 1 = ok, reply packet in buffer
     *          0 = timeout
     */
    int wait_for_completion(double secs);

    /*!
     * Allows creating thread to resume after copying reply into buffer
     */
    void notify_completion();

    /*!
     * Retrieve pending reply ID
     */
    unsigned int rid() const { return d_rid; }

    /*!
     * Retrieve destination buffer address
     */
    void *buffer() const { return d_buffer; }

    /*!
     * Retrieve destination buffer length
     */
    size_t len() const { return d_len; }
  };

} // namespace usrp2

#endif /* INCLUDED_CONTROL_H */
