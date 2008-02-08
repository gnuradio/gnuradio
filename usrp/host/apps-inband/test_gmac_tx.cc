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
#include <iostream>

#include <ui_nco.h>
#include <gmac.h>
#include <gmac_symbols.h>

static bool verbose = true;

class test_gmac_tx : public mb_mblock
{
  mb_port_sptr 	d_tx;
  mb_port_sptr 	d_cs;
  pmt_t		d_tx_chan;	// returned tx channel handle

  enum state_t {
    INIT,
    TRANSMITTING,
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
  test_gmac_tx(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~test_gmac_tx();
  void handle_message(mb_message_sptr msg);

 protected:
  void open_usrp();
  void close_usrp();
  void allocate_channel();
  void send_packets();
  void enter_transmitting();
  void build_and_send_next_frame();
  void handle_xmit_response(pmt_t invocation_handle);
  void enter_closing_channel();
};

test_gmac_tx::test_gmac_tx(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
    d_state(INIT), d_nsamples_to_send((long) 40e6),
    d_nsamples_xmitted(0),
    d_nframes_xmitted(0),
    d_samples_per_frame((long)(126 * 4)),	// full packet
    d_done_sending(false),
    d_amplitude(16384)
{ 
  
  define_component("GMAC", "gmac", PMT_NIL);
  d_tx = define_port("tx0", "gmac-tx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "gmac-cs", false, mb_port::INTERNAL);

  connect("self", "tx0", "GMAC", "tx0");
  connect("self", "cs", "GMAC", "cs");

  // initialize NCO
  double freq = 100e3;
  int interp = 32;			    // 32 -> 4MS/s
  double sample_rate = 128e6 / interp;	
  d_nco.set_freq(2*M_PI * freq/sample_rate);

}

test_gmac_tx::~test_gmac_tx()
{
}

void
test_gmac_tx::handle_message(mb_message_sptr msg)
{
  pmt_t event = msg->signal();
  pmt_t data = msg->data();
  pmt_t port_id = msg->port_id();

  pmt_t handle = PMT_F;
  pmt_t status = PMT_F;
  pmt_t dict = PMT_NIL;
  std::string error_msg;

  // Dispatch based on state
  switch(d_state) {
    
    //------------------------------ INIT ---------------------------------//
    // When GMAC is done initializing, it will send a response
    case INIT:
      
      if(pmt_eq(event, s_response_gmac_initialized)) {
        handle = pmt_nth(0, data);
        status = pmt_nth(1, data);

        if(pmt_eq(status, PMT_T)) {
          enter_transmitting();
          return;
        }
        else {
          error_msg = "error initializing gmac:";
          goto bail;
        }
      }
      goto unhandled;

    //-------------------------- TRANSMITTING ----------------------------//
    // In the transmit state we count the number of underruns received and
    // ballpark the number with an expected count (something >1 for starters)
    case TRANSMITTING:
      
      // Check that the transmits are OK
      if (pmt_eq(event, s_response_tx_pkt)){
        handle = pmt_nth(0, data);
        status = pmt_nth(1, data);

        if (pmt_eq(status, PMT_T)){
          handle_xmit_response(handle);
          return;
        }
        else {
          error_msg = "bad response-tx-pkt:";
          goto bail;
        }
      }

      goto unhandled;

  }

 // An error occured, print it, and shutdown all m-blocks
 bail:
  std::cerr << error_msg << data
      	    << "status = " << status << std::endl;
  shutdown_all(PMT_F);
  return;

 // Received an unhandled message for a specific state
 unhandled:
  if(verbose && !pmt_eq(event, pmt_intern("%shutdown")))
    std::cout << "[TEST_GMAC_TX] unhandled msg: " << msg
              << "in state "<< d_state << std::endl;
}

void
test_gmac_tx::enter_transmitting()
{
  d_state = TRANSMITTING;
  d_nsamples_xmitted = 0;

  d_cs->send(s_cmd_carrier_sense_deadline,
             pmt_list2(PMT_NIL,
                       pmt_from_long(50000000)));

  build_and_send_next_frame();	// fire off 4 to start pipeline
  build_and_send_next_frame();
  build_and_send_next_frame();
  build_and_send_next_frame();
}

void
test_gmac_tx::build_and_send_next_frame()
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
  int16_t *samples = pmt_s16vector_writeable_elements(uvec, ignore);

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
    
  // Per packet properties
  pmt_t tx_properties = pmt_make_dict();

  if(d_nframes_xmitted > 25000) {
    pmt_dict_set(tx_properties,
                 pmt_intern("carrier-sense"),
                 PMT_F);
  }

  if(d_nframes_xmitted > 35000) {
    pmt_dict_set(tx_properties,
                 pmt_intern("carrier-sense"),
                 PMT_NIL);
  }

  if(d_nframes_xmitted == 45000) {
    d_cs->send(s_cmd_carrier_sense_threshold, 
               pmt_list2(PMT_NIL,
                         pmt_from_long(100)));
  }
  
  if(d_nframes_xmitted == 60000) {
    d_cs->send(s_cmd_carrier_sense_threshold, 
               pmt_list2(PMT_NIL,
                         pmt_from_long(25)));
  }
  
  if(d_nframes_xmitted == 75000) {
    d_cs->send(s_cmd_carrier_sense_disable, 
               pmt_list1(PMT_NIL));
  }
  
  if(d_nframes_xmitted > 90000 && d_nframes_xmitted < 110000) {
    pmt_dict_set(tx_properties,
                 pmt_intern("carrier-sense"),
                 PMT_T);
  }
  
  if(d_nframes_xmitted > 110000) {

    if(d_nframes_xmitted % 100 == 0)
      pmt_dict_set(tx_properties,
                   pmt_intern("carrier-sense"),
                   PMT_T);
}

  pmt_t timestamp = pmt_from_long(0xffffffff);	// NOW
  d_tx->send(s_cmd_tx_pkt,
	     pmt_list4(PMT_NIL,   // invocation-handle
           PMT_NIL,         // destination
		       uvec,				    // the samples
           tx_properties)); // per pkt properties

  d_nsamples_xmitted += nsamples_this_frame;
  d_nframes_xmitted++;

  if(verbose && 0)
    std::cout << "[TEST_GMAC_TX] Transmitted frame\n";
}


void
test_gmac_tx::handle_xmit_response(pmt_t handle)
{
  if (d_done_sending &&
      pmt_to_long(handle) == (d_nframes_xmitted - 1)){
    // We're done sending and have received all responses
  }

  build_and_send_next_frame();
}

REGISTER_MBLOCK_CLASS(test_gmac_tx);


// ----------------------------------------------------------------

int
main (int argc, char **argv)
{
  // handle any command line args here

  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  rt->run("test_gmac_tx", "test_gmac_tx", PMT_F, &result);
}
