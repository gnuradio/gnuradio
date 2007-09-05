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
#include <mb_runtime_nop.h>		// QA only
#include <mb_protocol_class.h>
#include <mb_exception.h>
#include <mb_msg_queue.h>
#include <mb_message.h>
#include <mb_mblock_impl.h>
#include <mb_msg_accepter.h>
#include <mb_class_registry.h>
#include <pmt.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

// Signal set for the USRP server
static pmt_t s_cmd_allocate_channel = pmt_intern("cmd-allocate-channel");
static pmt_t s_cmd_close = pmt_intern("cmd-close");
static pmt_t s_cmd_deallocate_channel = pmt_intern("cmd-deallocate-channel");
static pmt_t s_cmd_open = pmt_intern("cmd-open");
static pmt_t s_cmd_start_recv_raw_samples = pmt_intern("cmd-start-recv-raw-samples");
static pmt_t s_cmd_stop_recv_raw_samples = pmt_intern("cmd-stop-recv-raw-samples");
static pmt_t s_cmd_to_control_channel = pmt_intern("cmd-to-control-channel");
static pmt_t s_cmd_xmit_raw_frame  = pmt_intern("cmd-xmit-raw-frame");
static pmt_t s_cmd_max_capacity  = pmt_intern("cmd-max-capacity");
static pmt_t s_cmd_ntx_chan  = pmt_intern("cmd-ntx-chan");
static pmt_t s_cmd_nrx_chan  = pmt_intern("cmd-nrx-chan");
static pmt_t s_cmd_current_capacity_allocation  = pmt_intern("cmd-current-capacity-allocation");
static pmt_t s_response_allocate_channel = pmt_intern("response-allocate-channel");
static pmt_t s_response_close = pmt_intern("response-close");
static pmt_t s_response_deallocate_channel = pmt_intern("response-deallocate-channel");
static pmt_t s_response_from_control_channel = pmt_intern("response-from-control-channel");
static pmt_t s_response_open = pmt_intern("response-open");
static pmt_t s_response_recv_raw_samples = pmt_intern("response-recv-raw-samples");
static pmt_t s_response_xmit_raw_frame = pmt_intern("response-xmit-raw-frame");
static pmt_t s_response_max_capacity = pmt_intern("response-max-capacity");
static pmt_t s_response_ntx_chan = pmt_intern("response-ntx-chan");
static pmt_t s_response_nrx_chan = pmt_intern("response-nrx-chan");
static pmt_t s_response_current_capacity_allocation  = pmt_intern("response-current-capacity-allocation");

static bool verbose = false;

class test_usrp_rx : public mb_mblock
{
  mb_port_sptr 	d_rx;
  mb_port_sptr 	d_cs;
  pmt_t		d_rx_chan;	// returned tx channel handle

  bool d_disk_write;

  enum state_t {
    INIT,
    OPENING_USRP,
    ALLOCATING_CHANNEL,
    RECEIVING,
    CLOSING_CHANNEL,
    CLOSING_USRP,
  };

  state_t	d_state;

  std::ofstream d_ofile;

 public:
  test_usrp_rx(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  ~test_usrp_rx();
  void initial_transition();
  void handle_message(mb_message_sptr msg);

 protected:
  void open_usrp();
  void close_usrp();
  void allocate_channel();
  void send_packets();
  void enter_receiving();
  void build_and_send_next_frame();
  void handle_response_recv_raw_samples(pmt_t invocation_handle);
  void enter_closing_channel();
};

test_usrp_rx::test_usrp_rx(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
  d_disk_write(false)
{ 
  
  d_rx = define_port("rx0", "usrp-rx", false, mb_port::INTERNAL);
  d_cs = define_port("cs", "usrp-server-cs", false, mb_port::INTERNAL);
  
  //bool fake_usrp_p = true;
  bool fake_usrp_p = false;
  
  d_disk_write = true;

  // Test the TX side

  // Pass a dictionary to usrp_server which specifies which interface to use, the stub or USRP
  pmt_t usrp_dict = pmt_make_dict();

  if(fake_usrp_p)
    pmt_dict_set(usrp_dict, 
                 pmt_intern("fake-usrp"),
		             PMT_T);
  
  // Specify the RBF to use
  pmt_dict_set(usrp_dict,
               pmt_intern("rbf"),
               pmt_intern("tmac6.rbf"));

  // Set TX and RX interpolations
  pmt_dict_set(usrp_dict,
               pmt_intern("interp-tx"),
               pmt_from_long(128));

  pmt_dict_set(usrp_dict,
               pmt_intern("interp-rx"),
               pmt_from_long(16));

  define_component("server", "usrp_server", usrp_dict);

  connect("self", "rx0", "server", "rx0");
  connect("self", "cs", "server", "cs");

  if(d_disk_write)
    d_ofile.open("pdump_rx.dat",std::ios::binary|std::ios::out);
}

test_usrp_rx::~test_usrp_rx()
{
  if(d_disk_write)
    d_ofile.close();
}

void
test_usrp_rx::initial_transition()
{
  open_usrp();
}

void
test_usrp_rx::handle_message(mb_message_sptr msg)
{
  pmt_t	event = msg->signal();
  pmt_t data = msg->data();

  pmt_t handle = PMT_F;
  pmt_t status = PMT_F;
  std::string error_msg;
  
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
      status = pmt_nth(1, data);
      d_rx_chan = pmt_nth(2, data);

      if (pmt_eq(status, PMT_T)){
        enter_receiving();
        return;
      }
      else {
        error_msg = "failed to allocate channel:";
        goto bail;
      }
    }
    goto unhandled;

  case RECEIVING:
    if (pmt_eq(event, s_response_recv_raw_samples)){
      status = pmt_nth(1, data);

      if (pmt_eq(status, PMT_T)){
        handle_response_recv_raw_samples(data);
        return;
      }
      else {
        error_msg = "bad response-xmit-raw-frame:";
        goto bail;
      }
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
  std::cout << "test_usrp_inband_rx: unhandled msg: " << msg
	    << "in state "<< d_state << std::endl;
}


void
test_usrp_rx::open_usrp()
{
  pmt_t which_usrp = pmt_from_long(0);

  d_cs->send(s_cmd_open, pmt_list2(PMT_NIL, which_usrp));
  d_state = OPENING_USRP;
}

void
test_usrp_rx::close_usrp()
{
  d_cs->send(s_cmd_close, pmt_list1(PMT_NIL));
  d_state = CLOSING_USRP;
}

void
test_usrp_rx::allocate_channel()
{
  long capacity = (long) 16e6;
  d_rx->send(s_cmd_allocate_channel, pmt_list2(PMT_T, pmt_from_long(capacity)));
  d_state = ALLOCATING_CHANNEL;
}

void
test_usrp_rx::enter_receiving()
{
  d_state = RECEIVING;

  d_rx->send(s_cmd_start_recv_raw_samples,
             pmt_list2(PMT_F,
                       d_rx_chan));
}

void
test_usrp_rx::handle_response_recv_raw_samples(pmt_t data)
{
  pmt_t invocation_handle = pmt_nth(0, data);
  pmt_t status = pmt_nth(1, data);
  pmt_t v_samples = pmt_nth(2, data);
  pmt_t timestamp = pmt_nth(3, data);
  pmt_t properties = pmt_nth(4, data);

  size_t n_bytes;
  
  const char *samples = (const char *) pmt_uniform_vector_elements(v_samples, n_bytes);

  if(d_disk_write)
    d_ofile.write(samples, n_bytes);

  if(verbose)
    std::cout << ".";

  if (pmt_is_dict(properties)) {
    // Read the RSSI
    if(pmt_t rssi = pmt_dict_ref(properties, 
                                 pmt_intern("rssi"), 
                                 PMT_NIL)) {
      if(!pmt_eqv(rssi, PMT_NIL)) 
        std::cout << "RSSI: " << rssi << std::endl;
    }
  }
  

}

void
test_usrp_rx::enter_closing_channel()
{
  d_state = CLOSING_CHANNEL;
  
  d_rx->send(s_cmd_deallocate_channel, pmt_list2(PMT_NIL, d_rx_chan));
}

REGISTER_MBLOCK_CLASS(test_usrp_rx);


// ----------------------------------------------------------------

int
main (int argc, char **argv)
{
  // handle any command line args here

  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  rt->run("top", "test_usrp_rx", PMT_F, &result);
}
