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

#ifndef INCLUDED_EZDOP_SOURCE_C_H
#define INCLUDED_EZDOP_SOURCE_C_H

#include <gr_sync_block.h>
#include <ezdop.h>
#include <stdexcept>

class ezdop_source_c;
typedef boost::shared_ptr<ezdop_source_c> ezdop_source_c_sptr;

ezdop_source_c_sptr ezdop_make_source_c();

class ezdop_source_c : public gr_sync_block {
private:
    friend ezdop_source_c_sptr ezdop_make_source_c();
    ezdop_source_c() throw (std::runtime_error);

    ezdop *d_ezdop;

public:
    virtual bool start();  // Called by runtime 
    virtual bool stop();   // Called by runtime

    int rate();
    bool set_rate(int rate); // Any integer divisor of 2000 Hz

    bool rotate();
    bool stop_rotating();

    ~ezdop_source_c();

    int work(int noutput_items,
	gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items);
};

#endif /* INCLUDED_EZDOP_SOURCE_C_H */
