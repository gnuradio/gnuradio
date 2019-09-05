/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gnuradio/atsc/GrAtscFieldSyncCorrelator.h>
#include <gnuradio/atsc/create_atsci_fs_correlator.h>
#include <gnuradio/atsc/fs_correlator_impl.h>

// typedefs for fundamental i/o types

typedef float iType;
typedef float oType;

static const int NUMBER_OF_OUTPUTS = 2; // # of output streams


GrAtscFieldSyncCorrelator::GrAtscFieldSyncCorrelator()
    : VrHistoryProc<iType, oType>(NUMBER_OF_OUTPUTS)
{
    // 1 + number of extra input elements at which we look.  This is
    // used by the superclass's forecast routine to get us the correct
    // range on our inputs.
    // We're one-to-one input-to-output so set it to 1.
    history = 1;

    d_fsc = create_atsci_fs_correlator();
}

GrAtscFieldSyncCorrelator::~GrAtscFieldSyncCorrelator()
{
    // Anything that isn't automatically cleaned up...

    delete d_fsc;
}

/*
 * This is the real work horse.  In general this interface can handle
 * multiple streams of input and output, but we almost always
 * use a single input and output stream.
 */

int GrAtscFieldSyncCorrelator::work(VrSampleRange output,
                                    void* ao[],
                                    VrSampleRange inputs[],
                                    void* ai[])
{
    // If we have state that persists across invocations (e.g., we have
    // instance variables that we modify), we must use the sync method
    // to indicate to the scheduler that our output must be computed in
    // order.  This doesn't keep other things from being run in
    // parallel, it just means that at any given time, there is only a
    // single thread working this code, and that the scheduler will
    // ensure that we are asked to produce output that is contiguous and
    // that will be presented to us in order of increasing time.

    // We have state, hence we must use sync.

    sync(output.index);

    // construct some nicer i/o pointers to work with.

    iType* in = ((iType**)ai)[0];
    oType* sample_out = ((oType**)ao)[0];
    oType* tag_out = ((oType**)ao)[1];


    // We must produce output.size units of output.

    for (unsigned int i = 0; i < output.size; i++) {
        d_fsc->filter(in[i], &sample_out[i], &tag_out[i]);
    }

    // Return the number of units we produced.
    // Note that for all intents and purposes, it is an error to
    // produce less than you are asked for.

    return output.size;
}
