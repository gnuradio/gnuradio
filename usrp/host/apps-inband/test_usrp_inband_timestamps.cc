/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <mb_mblock.h>
#include <mb_runtime.h>
#include <mb_protocol_class.h>
#include <mb_exception.h>
#include <mb_msg_queue.h>
#include <mb_message.h>
#include <mb_msg_accepter.h>
#include <mb_class_registry.h>
#include <pmt.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>

#include <ui_nco.h>
#include <symbols_usrp_server_cs.h>
#include <symbols_usrp_channel.h>
#include <symbols_usrp_low_level_cs.h>
#include <symbols_usrp_tx.h>
#include <symbols_usrp_rx.h>

#define NBPING  10

static bool verbose = true;
bool bskip = false;
long bstep = 10000;
long bcurr = 0;
long incr = 0x500;
long ptime = 0x000;

class test_usrp_inband_timestamps : public mb_mblock
{
  mb_port_sptr 	d_tx;
  mb_port_sptr 	d_rx;
  mb_port_sptr 	d_cs;
  pmt_t		d_tx_chan;	// returned tx channel handle
  pmt_t		d_rx_chan;	// returned tx channel handle

  struct timeval times[NBPING];

  enum state_t {
    INIT,
    OPENING_USRP,
    ALLOCATING_CHANNEL,
    TRANSMITTING,
    CLOSING_CHANNEL,
    CLOSING_USRP,
  };

  state_t	d_state;
  long		d_nsamples_to_send;
  long		d_nsamples_xmitted;
  long		d_nframes_xmitted;
  long		d_samples_per_frame;
  bool		d_done_sending;

  // for generating sine wave output
  ui_nco<float,float>	d_nco;
  double	        d_amplitude;

 public:
  test_usrp_inband_timestamps(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~test_usrp_inband_timestamps();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void open_usrp();
  void close_usrp();
  void allocate_channel();
  void send_packets();
  void enter_receiving();
  void enter_transmitting();
  void build_and_send_ping();
  void build_and_send_next_frame();
  void handle_xmit_response(pmt_t invocation_handle);
  void enter_closing_channel();
};

test_usrp_inband_timestamps::test_usrp_inband_timestamps(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
    d_tx_chan(PMT_NIL),
    d_rx_chan(PMT_NIL),
    d_state(INIT), d_nsamples_to_send((long) 40e6),
    d_nsamples_xmitted(0),
    d_nframes_xmitted(0),
    //d_samples_per_frame((long)(126)),
    d_samples_per_frame((long)(126 * 2)),	// non-full packet
    //d_samples_per_frame((long)(126 * 3.5)),	// non-full packet
    //d_samples_per_frame((long)(126 * 4)),	// full packet
    d_done_sending(false),
    d_amplitude(16384)
{ 
  if(verbose)
    std::cout << "[TEST_USRP_INBAND_TIMESTAMPS] Initializing...\n";
  
  d_tx = define_port("tx0", "usrp-tx", false, mb_port::INTERNAL);
  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);
  
  bool fake_usrp_p = false;

  // Test the TX side

  pmt_t usrp_dict = pmt_make_dict();

  if(fake_usrp_p) {
    pmt_dict_set(usrp_dict, 
                 pmt_intern("fake-usrp"),
		             PMT_T);
  }

  // Set TX and RX interpolations
  pmt_dict_set(usrp_dict,
               pmt_intern("interp-tx"),
               pmt_from_long(128));

  pmt_dict_set(usrp_dict,
               pmt_intern("decim-rx"),
               pmt_from_long(16));

  // Specify the RBF to use
  pmt_dict_set(usrp_dict,
               pmt_intern("rbf"),
               pmt_intern("inband_1rxhb_1tx.rbf"));

  define_component("server", "usrp_server", usrp_dict);

  connect("self", "tx0", "server", "tx0");
  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");

  // initialize NCO
  double freq = 100e3;
  int interp = 32;			    // 32 -> 4MS/s
  double sample_rate = 128e6 / interp;	
  d_nco.set_freq(2*M_PI * freq/sample_rate);

}

test_usrp_inband_timestamps::~test_usrp_inband_timestamps()
{
}

void
test_usrp_inband_timestamps::initial_transition()
{
  open_usrp();
}

void
test_usrp_inband_timestamps::handle_message(mb_message_sptr msg)
{
  pmt_t	event = msg->signal();
  pmt_t data = msg->data();
  pmt_t port_id = msg->port_id();

  pmt_t handle = PMT_F;
  pmt_t status = PMT_F;
  std::string error_msg;
  
  //std::cout << msg << std::endl;

  switch(d_state){
  case OPENING_USRP:
    if (pmt_eq(event, s_response_open)){
      status = pmt_nth(1, data);
      if (pmt_eq(status, PMT_T)){
        allocate_channel();
        return;
      }
      else {
        error_msg = "failed to open usrp:";
        goto bail;
      }
    }
    goto unhandled;
    
  case ALLOCATING_CHANNEL:
    if (pmt_eq(event, s_response_allocate_channel)){

      if(pmt_eq(d_tx->port_symbol(), port_id)) {
        status = pmt_nth(1, data);
        d_tx_chan = pmt_nth(2, data);

        if (pmt_eq(status, PMT_T)){

          if(verbose)
            std::cout << "[TEST_USRP_INBAND_TIMESTAMPS] Received allocation for TX\n";

          if(!pmt_eqv(d_rx_chan, PMT_NIL)) {
            enter_receiving();
            enter_transmitting();
          }
          return;
        }
        else {
          error_msg = "failed to allocate channel:";
          goto bail;
        }
      }
      
      if(pmt_eq(d_rx->port_symbol(), port_id)) {
        status = pmt_nth(1, data);
        d_rx_chan = pmt_nth(2, data);

        if (pmt_eq(status, PMT_T)){

          if(verbose)
            std::cout << "[TEST_USRP_INBAND_TIMESTAMPS] Received allocation for TX\n";
          
          if(!pmt_eqv(d_tx_chan, PMT_NIL)) {
            enter_receiving();
            enter_transmitting();
          }
          return;
        }
        else {
          error_msg = "failed to allocate channel:";
          goto bail;
        }
      }
    }
    goto unhandled;

  case TRANSMITTING:
    if (pmt_eq(event, s_response_xmit_raw_frame)){
      handle = pmt_nth(0, data);
      status = pmt_nth(1, data);

      if (pmt_eq(status, PMT_T)){
        handle_xmit_response(handle);
        return;
      }
      else {
        error_msg = "bad response-xmit-raw-frame:";
        goto bail;
      }
    }

    if (pmt_eq(event, s_response_from_control_channel)) {
      std::cout << "ping response!\n";
    }
    goto unhandled;

  case CLOSING_CHANNEL:
    if (pmt_eq(event, s_response_deallocate_channel)){
      status = pmt_nth(1, data);

      if (pmt_eq(status, PMT_T)){
        close_usrp();
        return;
      }
      else {
        error_msg = "failed to deallocate channel:";
        goto bail;
      }
    }
    goto unhandled;

  case CLOSING_USRP:
    if (pmt_eq(event, s_response_close)){
      status = pmt_nth(1, data);

      if (pmt_eq(status, PMT_T)){
        shutdown_all(PMT_T);
        return;
      }
      else {
        error_msg = "failed to close USRP:";
        goto bail;
      }
    }
    goto unhandled;

  default:
    goto unhandled;
  }
  return;

 bail:
  std::cerr << error_msg << data
	    << "status = " << status << std::endl;
  shutdown_all(PMT_F);
  return;

 unhandled:
  if(verbose && 0)
    std::cout << "test_usrp_inband_tx: unhandled msg: " << msg
              << "in state "<< d_state << std::endl;
}


void
test_usrp_inband_timestamps::open_usrp()
{
  pmt_t which_usrp = pmt_from_long(0);

  d_cs->send(s_cmd_open, pmt_list2(PMT_NIL, which_usrp));
  d_state = OPENING_USRP;
}

void
test_usrp_inband_timestamps::close_usrp()
{
  d_cs->send(s_cmd_close, pmt_list1(PMT_NIL));
  d_state = CLOSING_USRP;

  if(verbose)
    std::cout << "[TEST_USRP_INBAND_TIMESTAMPS] Closing USRP\n";
}

void
test_usrp_inband_timestamps::allocate_channel()
{
  long capacity = (long) 16e6;
  d_tx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(capacity)));
  d_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(capacity)));
  d_state = ALLOCATING_CHANNEL;
}

void
test_usrp_inband_timestamps::enter_receiving()
{
  d_rx->send(s_cmd_start_recv_raw_samples,
             pmt_list2(PMT_F,
                       d_rx_chan));
}

void
test_usrp_inband_timestamps::enter_transmitting()
{
  d_state = TRANSMITTING;
  d_nsamples_xmitted = 0;

  if(verbose)
    std::cout << "[TEST_USRP_INBAND_TIMESTAMPS] Beginning transmission\n";

  sleep(1);

  build_and_send_next_frame();
  build_and_send_next_frame();
  build_and_send_next_frame();
  build_and_send_next_frame();

}

void
test_usrp_inband_timestamps::build_and_send_ping()
{
  
  d_tx->send(s_cmd_to_control_channel,
             pmt_list2(PMT_NIL, pmt_list1(pmt_list2(s_op_ping_fixed,
                                                    pmt_list2(pmt_from_long(0),
                                                              pmt_from_long(0))))));
  if(verbose && 0)
    std::cout << "[TEST_USRP_INBAND_TIMESTAMPS] Ping sent" << std::endl;
}

void
test_usrp_inband_timestamps::build_and_send_next_frame()
{
  // allocate the uniform vector for the samples
  // FIXME perhaps hold on to this between calls

#if 0
  long nsamples_this_frame =
    std::min(d_nsamples_to_send - d_nsamples_xmitted,
	     d_samples_per_frame);
#else
  long nsamples_this_frame = d_samples_per_frame;
#endif

  if (nsamples_this_frame == 0){
    d_done_sending = true;
    return;
  }
    

  size_t nshorts = 2 * nsamples_this_frame;	// 16-bit I & Q
  pmt_t uvec = pmt_make_s16vector(nshorts, 0);
  size_t ignore;
  int16_t *samples = pmt_s16vector_writable_elements(uvec, ignore);

  // fill in the complex sinusoid

  for (int i = 0; i < nsamples_this_frame; i++){

    if (1){
      gr_complex s;
      d_nco.sincos(&s, 1, d_amplitude);
      // write 16-bit i & q
      samples[2*i] =   (int16_t) s.real();
      samples[2*i+1] = (int16_t) s.imag();
    }
    else {
      gr_complex s(d_amplitude, d_amplitude);

      // write 16-bit i & q
      samples[2*i] =   (int16_t) s.real();
      samples[2*i+1] = (int16_t) s.imag();
    }
  }

  pmt_t timestamp;

  if(bskip) {
    timestamp = pmt_from_long(0x0);	// throw away  
    bcurr++;
    if(bcurr == bstep) {
      bskip = false;
      bcurr = 0;
    }
  } else {
    timestamp = pmt_from_long(0xffffffff);	// NOW
    timestamp = pmt_from_long(ptime);
    ptime += incr;
    bcurr++;
    if(bcurr == bstep) {
      //bskip = true;
      bcurr = 0;
    }
  }

  std::cout << bskip << " -- " << bcurr << std::endl;

  d_tx->send(s_cmd_xmit_raw_frame,
	     pmt_list4(pmt_from_long(d_nframes_xmitted),  // invocation-handle
		       d_tx_chan,			  // channel
		       uvec,				  // the samples
		       timestamp));

  d_nsamples_xmitted += nsamples_this_frame;
  d_nframes_xmitted++;

  if(verbose && 0)
    std::cout << "[TEST_USRP_INBAND_TIMESTAMPS] Transmitted frame\n";
  
  //build_and_send_next_frame();
}


void
test_usrp_inband_timestamps::handle_xmit_response(pmt_t handle)
{
  if (d_done_sending &&
      pmt_to_long(handle) == (d_nframes_xmitted - 1)){
    // We're done sending and have received all responses
    enter_closing_channel();
  }

  build_and_send_next_frame();
  //build_and_send_ping();
}

void
test_usrp_inband_timestamps::enter_closing_channel()
{
  d_state = CLOSING_CHANNEL;
  
  d_tx->send(s_cmd_deallocate_channel, pmt_list2(PMT_NIL, d_tx_chan));

  if(verbose)
    std::cout << "[TEST_USRP_INBAND_TIMESTAMPS] Closing channel\n";
}

REGISTER_MBLOCK_CLASS(test_usrp_inband_timestamps);


// ----------------------------------------------------------------

int
main (int argc, char **argv)
{
  // handle any command line args here

  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  rt->run("top", "test_usrp_inband_timestamps", PMT_F, &result);
}
