/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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
#include <mblock/mblock.h>
#include <mblock/protocol_class.h>
#include <mblock/message.h>
#include <mblock/class_registry.h>
#include <iostream>
#include <sstream>
#include <bitset>

static pmt_t s_in = pmt_intern("in");
static pmt_t s_out = pmt_intern("out");
static pmt_t s_data = pmt_intern("data");
static pmt_t s_start = pmt_intern("start");
static pmt_t s_send_batch = pmt_intern("send-batch");
static pmt_t s_long0 = pmt_from_long(0);

static std::string
str(long x)
{
  std::ostringstream s;
  s << x;
  return s.str();
}

/*!
 * \brief mblock used for QA.
 *
 * Messages arriving on "in" consist of a pair containing a (long)
 * message number in the car, and a (long) bitmap in the cdr.  For
 * each message received on "in", a new message is sent on "out".  The
 * new message is the same format as the input, but the bitmap in
 * the cdr has a "1" or'd into it that corresponds to the bit number
 * specified in the constructor.
 *
 * The bitmap can be used by the ultimate receiver to confirm
 * traversal of a set of blocks, if the blocks are assigned unique bit
 * numbers.
 */
class qa_bitset : public mb_mblock
{
  mb_port_sptr	d_in;
  mb_port_sptr	d_out;
  int		d_bitno;

public:
  qa_bitset(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  void handle_message(mb_message_sptr msg);
};

qa_bitset::qa_bitset(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  d_bitno = pmt_to_long(user_arg);	// The bit we are to set

  d_in  = define_port("in", "qa-bitset", false, mb_port::EXTERNAL);
  d_out = define_port("out", "qa-bitset", true, mb_port::EXTERNAL);
}

void
qa_bitset::handle_message(mb_message_sptr msg)
{
  if (pmt_eq(msg->port_id(), s_in) && pmt_eq(msg->signal(), s_data)){
    d_out->send(s_data,
		pmt_cons(pmt_car(msg->data()),
			 pmt_from_long((1L << d_bitno) | pmt_to_long(pmt_cdr(msg->data())))));
  }
}

REGISTER_MBLOCK_CLASS(qa_bitset);

// ------------------------------------------------------------------------

/*!
 * \brief mblock used for QA.  Compose two qa_bitset mblocks.
 */
class qa_bitset2 : public mb_mblock
{
  mb_port_sptr	d_in;
  mb_port_sptr	d_out;

public:
  qa_bitset2(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
};

qa_bitset2::qa_bitset2(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  long bitno = pmt_to_long(user_arg);	// The bit we are to set

  d_in  = define_port("in", "qa-bitset", false, mb_port::RELAY);
  d_out = define_port("out", "qa-bitset", true, mb_port::RELAY);

  define_component("bs0", "qa_bitset", pmt_from_long(bitno));
  define_component("bs1", "qa_bitset", pmt_from_long(bitno + 1));
  connect("self", "in", "bs0", "in");
  connect("bs0", "out", "bs1", "in");
  connect("bs1", "out", "self", "out");
}

REGISTER_MBLOCK_CLASS(qa_bitset2);

// ------------------------------------------------------------------------

/*!
 * \brief mblock used for QA.  Compose two qa_bitset2 mblocks.
 */
class qa_bitset4 : public mb_mblock
{
  mb_port_sptr	d_in;
  mb_port_sptr	d_out;

public:
  qa_bitset4(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
};

qa_bitset4::qa_bitset4(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  long bitno = pmt_to_long(user_arg);	// The bit we are to set

  d_in  = define_port("in", "qa-bitset", false, mb_port::RELAY);
  d_out = define_port("out", "qa-bitset", true, mb_port::RELAY);

  define_component("bs0", "qa_bitset2", pmt_from_long(bitno));
  define_component("bs1", "qa_bitset2", pmt_from_long(bitno + 2));
  connect("self", "in", "bs0", "in");
  connect("bs0", "out", "bs1", "in");
  connect("bs1", "out", "self", "out");
}

REGISTER_MBLOCK_CLASS(qa_bitset4);

// ------------------------------------------------------------------------

/*!
 * \brief mblock used for QA.  Compose two qa_bitset4 mblocks.
 */
class qa_bitset8 : public mb_mblock
{
  mb_port_sptr	d_in;
  mb_port_sptr	d_out;

public:
  qa_bitset8(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
};

qa_bitset8::qa_bitset8(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  long bitno = pmt_to_long(user_arg);	// The bit we are to set

  d_in  = define_port("in", "qa-bitset", false, mb_port::RELAY);
  d_out = define_port("out", "qa-bitset", true, mb_port::RELAY);

  define_component("bs0", "qa_bitset4", pmt_from_long(bitno));
  define_component("bs1", "qa_bitset4", pmt_from_long(bitno + 4));
  connect("self", "in", "bs0", "in");
  connect("bs0", "out", "bs1", "in");
  connect("bs1", "out", "self", "out");
}

REGISTER_MBLOCK_CLASS(qa_bitset8);

// ------------------------------------------------------------------------

/*!
 * \brief mblock used for QA.  Compose two qa_bitset8 mblocks.
 */
class qa_bitset16 : public mb_mblock
{
  mb_port_sptr	d_in;
  mb_port_sptr	d_out;

public:
  qa_bitset16(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
};

qa_bitset16::qa_bitset16(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  long bitno = pmt_to_long(user_arg);	// The bit we are to set

  d_in  = define_port("in", "qa-bitset", false, mb_port::RELAY);
  d_out = define_port("out", "qa-bitset", true, mb_port::RELAY);

  define_component("bs0", "qa_bitset8", pmt_from_long(bitno));
  define_component("bs1", "qa_bitset8", pmt_from_long(bitno + 8));
  connect("self", "in", "bs0", "in");
  connect("bs0", "out", "bs1", "in");
  connect("bs1", "out", "self", "out");
}

REGISTER_MBLOCK_CLASS(qa_bitset16);

// ------------------------------------------------------------------------

/*!
 * \brief mblock used for QA.  Compose two qa_bitset16 mblocks.
 */
class qa_bitset32 : public mb_mblock
{
  mb_port_sptr	d_in;
  mb_port_sptr	d_out;

public:
  qa_bitset32(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
};

qa_bitset32::qa_bitset32(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  long bitno = pmt_to_long(user_arg);	// The bit we are to set

  d_in  = define_port("in", "qa-bitset", false, mb_port::RELAY);
  d_out = define_port("out", "qa-bitset", true, mb_port::RELAY);

  define_component("bs0", "qa_bitset16", pmt_from_long(bitno));
  define_component("bs1", "qa_bitset16", pmt_from_long(bitno + 16));
  connect("self", "in", "bs0", "in");
  connect("bs0", "out", "bs1", "in");
  connect("bs1", "out", "self", "out");
}

REGISTER_MBLOCK_CLASS(qa_bitset32);

// ------------------------------------------------------------------------

class qa_bitset_src : public mb_mblock
{
  mb_port_sptr	d_cs_top;
  mb_port_sptr	d_cs;
  
  mb_port_sptr	d_out;

  long		d_msg_number;		// starting message number
  long		d_nmsgs_to_send;	// # of messages to send
  long		d_batch_size;		// # of messages to send per batch
  
public:
  qa_bitset_src(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  void handle_message(mb_message_sptr msg);

protected:
  void send_one();
  void send_batch();
};

qa_bitset_src::qa_bitset_src(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  d_msg_number    = pmt_to_long(pmt_nth(0, user_arg));
  d_nmsgs_to_send = pmt_to_long(pmt_nth(1, user_arg));
  d_batch_size    = pmt_to_long(pmt_nth(2, user_arg));

  d_cs_top = define_port("cs_top", "qa-bitset-cs", true, mb_port::EXTERNAL);
  d_cs = define_port("cs", "qa-bitset-cs", true, mb_port::EXTERNAL);

  d_out = define_port("out", "qa-bitset", true, mb_port::EXTERNAL);
}

void
qa_bitset_src::handle_message(mb_message_sptr msg)
{
  if ((pmt_eq(msg->port_id(), d_cs_top->port_symbol())
       || pmt_eq(msg->port_id(), d_cs->port_symbol()))
      && pmt_eq(msg->signal(), s_send_batch)){
    send_batch();
  }
}

void
qa_bitset_src::send_batch()
{
  for (int i = 0; i < d_batch_size; i++)
    send_one();
}

void
qa_bitset_src::send_one()
{
  if (d_nmsgs_to_send > 0){
    pmt_t msg_number = pmt_from_long(d_msg_number++);
    d_out->send(s_data, pmt_cons(msg_number, s_long0));
  }
  if (--d_nmsgs_to_send <= 0)
    exit();
}

REGISTER_MBLOCK_CLASS(qa_bitset_src);

// ------------------------------------------------------------------------

class qa_bitset_sink : public mb_mblock
{
  // Maximum number of messages we can track
  static const size_t MAX_MSGS = 1 * 1024 * 1024; 
  
  mb_port_sptr	d_cs0;
  mb_port_sptr	d_cs1;
  mb_port_sptr	d_cs2;
  mb_port_sptr	d_cs3;
  
  mb_port_sptr	d_in0;
  mb_port_sptr	d_in1;
  mb_port_sptr	d_in2;
  mb_port_sptr	d_in3;

  long			d_nmsgs_to_recv; // # of messages to receive
  long			d_batch_size;	 // # of messages to receive per batch
  uint32_t		d_expected_mask;

  std::bitset<MAX_MSGS>	d_bitset;
  long			d_nrecvd;
  
public:
  qa_bitset_sink(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  void handle_message(mb_message_sptr msg);

protected:
  void receive_one(mb_message_sptr msg);
};

qa_bitset_sink::qa_bitset_sink(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg),
    d_nrecvd(0)
{
  d_nmsgs_to_recv = pmt_to_long(pmt_nth(0, user_arg));
  d_batch_size    = pmt_to_long(pmt_nth(1, user_arg));
  d_expected_mask = pmt_to_long(pmt_nth(2, user_arg));

  if (d_nmsgs_to_recv > (long) MAX_MSGS)
    throw std::out_of_range("qa_bitset_sink: nmsgs_to_recv is too big");

  if (d_batch_size < 1)
    throw std::out_of_range("qa_bitset_sink: batch_size must be >= 1");

  d_cs0 = define_port("cs0", "qa-bitset-cs", true, mb_port::EXTERNAL);
  d_cs1 = define_port("cs1", "qa-bitset-cs", true, mb_port::EXTERNAL);
  d_cs2 = define_port("cs2", "qa-bitset-cs", true, mb_port::EXTERNAL);
  d_cs3 = define_port("cs3", "qa-bitset-cs", true, mb_port::EXTERNAL);

  d_in0 = define_port("in0", "qa-bitset", false, mb_port::EXTERNAL);
  d_in1 = define_port("in1", "qa-bitset", false, mb_port::EXTERNAL);
  d_in2 = define_port("in2", "qa-bitset", false, mb_port::EXTERNAL);
  d_in3 = define_port("in3", "qa-bitset", false, mb_port::EXTERNAL);
}

void
qa_bitset_sink::handle_message(mb_message_sptr msg)
{
  if ((pmt_eq(msg->port_id(), d_in0->port_symbol())
       || pmt_eq(msg->port_id(), d_in1->port_symbol())
       || pmt_eq(msg->port_id(), d_in2->port_symbol())
       || pmt_eq(msg->port_id(), d_in3->port_symbol()))
      && pmt_eq(msg->signal(), s_data)){

    receive_one(msg);
  }
}

void
qa_bitset_sink::receive_one(mb_message_sptr msg)
{
  long msg_number = pmt_to_long(pmt_car(msg->data()));
  uint32_t mask = pmt_to_long(pmt_cdr(msg->data()));

  // std::cout << msg->data() << std::endl;

  d_nrecvd++;
  if (d_nrecvd % d_batch_size == d_batch_size - 1){
    d_cs0->send(s_send_batch);
    d_cs1->send(s_send_batch);
    d_cs2->send(s_send_batch);
    d_cs3->send(s_send_batch);
  }

  if (msg_number >= d_nmsgs_to_recv){
    std::cerr << "qa_bitset_sink::receive_one: msg_number too big ("
	      << msg_number << ")\n";
    shutdown_all(PMT_F);
    return;
  }
  if (mask != d_expected_mask){
    fprintf(stderr,
	    "qa_bitset_sink::receive_one: Wrong mask.  Expected 0x%08x, got 0x%08x\n",
	    d_expected_mask, mask);
    shutdown_all(PMT_F);
    return;
  }

  if (d_bitset.test((size_t) msg_number)){
    std::cerr << "qa_bitset_sink::receive_one: duplicate msg_number ("
	      << msg_number << ")\n";
    shutdown_all(PMT_F);
    return;
  }

  d_bitset.set((size_t) msg_number);
  if (d_nrecvd == d_nmsgs_to_recv)
    shutdown_all(PMT_T);		// we're done!
}

REGISTER_MBLOCK_CLASS(qa_bitset_sink);

// ------------------------------------------------------------------------

class qa_bitset_top : public mb_mblock
{
  static const int NPIPES = 4;

  std::vector<mb_port_sptr>	d_cs;
  
  long			d_nmsgs; 	 // # of messages to send
  long			d_batch_size;	 // # of messages to receive per batch

public:
  qa_bitset_top(mb_runtime *runtime, const std::string &instance_name, pmt_t user_arg);
  void initial_transition();
};

qa_bitset_top::qa_bitset_top(mb_runtime *runtime,
			     const std::string &instance_name, pmt_t user_arg)
  : mb_mblock(runtime, instance_name, user_arg)
{
  d_nmsgs      = pmt_to_long(pmt_nth(0, user_arg));
  d_nmsgs = (d_nmsgs / NPIPES) * NPIPES;
  d_batch_size = pmt_to_long(pmt_nth(1, user_arg));

  /*
   * We build NPIPES sources which feed NPIPES pipelines, each of which
   * consists of 8-mblocks.  All pipelines feed into a single sink
   * which keeps track the results.
   */
  for (int i = 0; i < NPIPES; i++){
    d_cs.push_back(define_port("cs"+str(i), "qa-bitset-cs", false, mb_port::INTERNAL));
  
    // sources of test messages
    define_component("src"+str(i), "qa_bitset_src",
		     pmt_list3(pmt_from_long(i * d_nmsgs/NPIPES),
			       pmt_from_long(d_nmsgs/NPIPES),
			       pmt_from_long(d_batch_size)));

    // 8-mblock processing pipelines
    define_component("pipeline"+str(i), "qa_bitset8", pmt_from_long(0));
  }

  // sink for output of pipelines
  define_component("sink", "qa_bitset_sink",
		   pmt_list3(pmt_from_long(d_nmsgs),
			     pmt_from_long(d_batch_size * NPIPES),
			     pmt_from_long(0x000000ff)));

  for (int i = 0; i < NPIPES; i++){
    connect("self", "cs"+str(i), "src"+str(i), "cs_top");
    connect("src"+str(i), "out", "pipeline"+str(i), "in");
    connect("src"+str(i), "cs", "sink", "cs"+str(i));
    connect("pipeline"+str(i), "out", "sink", "in"+str(i));
  }
}

void
qa_bitset_top::initial_transition()
{
  for (int i = 0; i < NPIPES; i++){
    d_cs[i]->send(s_send_batch);	// prime the pump
    d_cs[i]->send(s_send_batch);
  }
}

REGISTER_MBLOCK_CLASS(qa_bitset_top);
