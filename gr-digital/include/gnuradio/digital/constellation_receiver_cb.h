/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_H
#define INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/control_loop.h>
#include <gnuradio/digital/api.h>
#include <gnuradio/digital/constellation.h>

namespace gr {
namespace digital {

/*!
 * \brief This block makes hard decisions about the received
 * symbols (using a constellation object) and also fine tunes
 * phase synchronization.
 *
 * \details
 *
 * The phase and frequency synchronization are based on a Costas
 * loop that finds the error of the incoming signal point compared
 * to its nearest constellation point. The frequency and phase of
 * the NCO are updated according to this error.
 *
 * Message Ports:
 *
 * set_constellation (input):
 *    Receives a PMT any containing a new gr::digital::constellation object.
 *    The PMT is cast back to a gr::digital::constellation_sptr
 *    and passes this to set_constellation.
 *
 * rotate_phase (input):
 *    Receives a PMT double to update the phase.
 *    The phase value passed in the message is added to the
 *    current phase of the receiver.
 */
class DIGITAL_API constellation_receiver_cb : virtual public block,
                                              virtual public blocks::control_loop
{
public:
    // gr::digital::constellation_receiver_cb::sptr
    typedef boost::shared_ptr<constellation_receiver_cb> sptr;

    /*!
     * \brief Constructs a constellation receiver that (phase/fine
     * freq) synchronizes and decodes constellation points specified
     * by a constellation object.
     *
     * \param constellation constellation of points for generic modulation
     * \param loop_bw	Loop  bandwidth of the Costas Loop (~ 2pi/100)
     * \param fmin          minimum normalized frequency value the loop can achieve
     * \param fmax          maximum normalized frequency value the loop can achieve
     */
    static sptr
    make(constellation_sptr constellation, float loop_bw, float fmin, float fmax);

    virtual void phase_error_tracking(float phase_error) = 0;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_H */
