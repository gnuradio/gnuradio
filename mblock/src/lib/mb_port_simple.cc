/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#include <mb_port_simple.h>
#include <mb_msg_accepter.h>
#include <mb_exception.h>
#include <mb_mblock.h>
#include <mb_mblock_impl.h>
#include <assert.h>
#include <mbi_runtime_lock.h>


mb_port_simple::mb_port_simple(mb_mblock *mblock,
			       const std::string &port_name,
			       const std::string &protocol_class_name,
			       bool conjugated,
			       mb_port::port_type_t port_type)
  : mb_port(mblock, port_name, protocol_class_name, conjugated, port_type),
    d_cache_valid(false)
{
}

mb_port_simple::~mb_port_simple()
{
  // nop
}

void
mb_port_simple::send(pmt_t signal, pmt_t data, pmt_t metadata, mb_pri_t priority)
{
  if (port_type() == mb_port::RELAY)  // Can't send directly to a RELAY port
    throw mbe_invalid_port_type(mblock(), mblock()->instance_name(), port_name());

  mb_msg_accepter_sptr  accepter = find_accepter(this);
  if (accepter)
    (*accepter)(signal, data, metadata, priority);
}


mb_msg_accepter_sptr
mb_port_simple::find_accepter(mb_port_simple *start)
{
  mb_port_simple	*p = start;
  mb_port_simple	*pp = 0;
  mb_mblock 		*context = 0;
  mb_endpoint 		peer_ep;
  mb_msg_accepter_sptr	r;

  if (start->d_cache_valid)
    return start->d_cached_accepter;

  mbi_runtime_lock	l(p->mblock());

  // Set up initial context.

  switch(p->port_type()){
  case mb_port::INTERNAL:	// binding is in our name space
    context = p->mblock();
    break;

  case mb_port::EXTERNAL:	// binding is in parent's name space
    context = p->mblock()->parent();
    if (!context)			// can't be bound if there's no parent
      return mb_msg_accepter_sptr();	// not bound
    break;

  default:
    throw std::logic_error("Can't happen: mb_port_simple::find_accepter [1]");
  }


 traverse:

  if (!context->impl()->lookup_other_endpoint(p, &peer_ep))
    return mb_msg_accepter_sptr();	// not bound
  
  pp = dynamic_cast<mb_port_simple *>(peer_ep.port().get());	// peer port
  assert(pp);

  switch (pp->port_type()){	
  case mb_port::INTERNAL:	// Terminate here.
  case mb_port::EXTERNAL:
    r = pp->make_accepter();

    // cache the result

    start->d_cached_accepter = r;
    start->d_cache_valid = true;
    return r;

  case mb_port::RELAY:		// Traverse to other side of relay port.
    if (peer_ep.inside_of_relay_port_p()){
      // We're on inside of relay port, headed out.
      p = pp;
      context = p->mblock()->parent();

      // Corner case: we're attempting to traverse a relay port on the border
      // of the top block...
      if (!context)
	return mb_msg_accepter_sptr();	// not bound

      goto traverse;
    }
    else {
      // We're on the outside of relay port, headed in.
      p = pp;
      context = p->mblock();
      goto traverse;
    }
    break;

  default:
    throw std::logic_error("Can't happen: mb_port_simple::find_accepter [2]");
  }
}


mb_msg_accepter_sptr
mb_port_simple::make_accepter()
{
  return d_mblock->impl()->make_accepter(port_symbol());
}

void
mb_port_simple::invalidate_cache()
{
  d_cache_valid = false;
  d_cached_accepter.reset();
}
