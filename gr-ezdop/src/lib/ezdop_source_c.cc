/*
 * Copyright 2004,2006 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ezdop_source_c.h>
#include <gr_io_signature.h>

ezdop_source_c_sptr ezdop_make_source_c()
{
    return ezdop_source_c_sptr(new ezdop_source_c());
}

ezdop_source_c::ezdop_source_c() throw (std::runtime_error) :
    gr_sync_block("ezdop_source_c",
	          gr_make_io_signature(0, 0, 0),
                  gr_make_io_signature(1, 1, sizeof (gr_complex))
    )
{
    d_ezdop = new ezdop();

    if (!d_ezdop->init())
	throw std::runtime_error("unable to init ezdop hardware");
    if (!d_ezdop->reset())
	throw std::runtime_error("unable to reset ezdop hardware");
}

ezdop_source_c::~ezdop_source_c()
{
    assert(d_ezdop);
    d_ezdop->finish();
    delete d_ezdop;
}

bool ezdop_source_c::start()
{
    assert(d_ezdop);
    bool success = d_ezdop->stream();
    if (!success)
	fprintf(stderr, "ezdop_source_c::start(): unable to start streaming\n");
    return success;
}

bool ezdop_source_c::stop()
{
    assert(d_ezdop);
    bool success = d_ezdop->stop_streaming();
    if (!success)
	fprintf(stderr, "ezdop_source_c::start(): unable to stop streaming\n");
    return success;
}

int ezdop_source_c::rate()
{
    assert(d_ezdop);
    return d_ezdop->rate();
}

bool ezdop_source_c::set_rate(int rate)
{
    assert(d_ezdop);
    if (rate <= 0 || rate > 2000)
	return false;
    bool success = d_ezdop->set_rate(rate);    
    return success;
}

bool ezdop_source_c::rotate()
{
    assert(d_ezdop);
    bool success = d_ezdop->rotate();
    if (!success)
	fprintf(stderr, "ezdop_source_c::rotate(): unable to start rotating\n");
    return success;
}

bool ezdop_source_c::stop_rotating()
{
    assert(d_ezdop);
    bool success = d_ezdop->stop_rotating();
    if (!success)
	fprintf(stderr, "ezdop_source_c::stop_rotating(): unable to stop rotating\n");
    return success;
}

int ezdop_source_c::work(int noutput_items,
	gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items)
{
    assert(d_ezdop);
    float volume = 0.0; // Dummy for now

    gr_complex *out = (gr_complex *)output_items[0];
    return d_ezdop->read_iq(out, noutput_items, volume);
}
