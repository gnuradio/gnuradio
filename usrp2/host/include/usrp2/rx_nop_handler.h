/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_RX_NOP_HANDLER_H
#define INCLUDED_RX_NOP_HANDLER_H

#include <usrp2/rx_sample_handler.h>
#include <boost/shared_ptr.hpp>

/*!
 * \ingroup usrp2
 *
 * Base class for receive handlers that must copy into potentially limited
 * range destination buffers.
 *
 * Maintains counters for number of items copied, times invoked, and test
 * for whether maximum has been reached.
 *
 * Derived classes should override the () operator, but call this
 * parent class method at some point at the start of their own operations.
 */ 

namespace usrp2 {

  class rx_nop_handler : public rx_sample_handler
  {
    uint64_t	d_max_samples;
    uint64_t    d_max_quantum;
    uint64_t	d_nsamples;
    uint64_t	d_nframes;
    
  protected:
    bool        d_err;
    
  public:
    
    // Shared pointer to an instance of this class
    typedef boost::shared_ptr<rx_nop_handler> sptr;

    /*!
     * Constructor
     *
     * \param max_samples  Maximum number of samples to copy. Use zero for no maximum.
     * \param max_quantum  Maximum number of samples required to accept in one call.
     *                     Use 0 to indicate no maximum.
     */
    rx_nop_handler(uint64_t max_samples, uint64_t max_quantum=0)
      : d_max_samples(max_samples), d_max_quantum(max_quantum), 
        d_nsamples(0), d_nframes(0), d_err(false) {}
      
    /*!
     * Destructor.  Derived classes must implement their own, non-inline destructor.
     */
    virtual ~rx_nop_handler();
      
    /*!
     * \brief Returns number of frames this copier was called with
     */
    uint64_t nframes() const { return d_nframes; }

    /*!
     * \brief Returns actual number of samples copied
     */
    uint64_t nsamples() const { return d_nsamples; }

    /*!
     * \brief Returns maximum number of samples that will be copied
     */
    uint64_t max_samples() const { return d_max_samples; }

    /*!
     * Returns true if an error has occurred. Derived classes must set d_err to true
     * when an error occurs in the () operator
     */
    bool has_errored_p() const { return d_err; }

    /*!
     * \brief Returns true if this instance has reached the maximum number of samples
     */
    bool has_finished_p() const 
    { return d_max_samples == 0 ? false : d_nsamples >= d_max_samples-d_max_quantum; }
      

    /*!
     * Function operator invoked by USRP2 RX API. Derived classes must override this method
     * but then invoke it at the start of their processing.  This operator will always be
     * called at least once.
     *
     * \param items points to the first 32-bit word of uninterpreted sample data in the frame.
     * \param nitems is the number of entries in the frame in units of uint32_t's.
     * \param metadata is the additional per frame data provided by the USRP2 FPGA.
     *
     * \p items points to the raw sample data received off of the ethernet.  The data is
     * packed into big-endian 32-bit unsigned ints for transport, but the actual format
     * of the data is dependent on the current configuration of the USRP2.  The most common
     * format is 16-bit I & Q, with I in the top of the 32-bit word.
     *
     * \returns true if the object wants to be called again with new data;
     * false if no additional data is wanted.
     */
    virtual bool operator()(const uint32_t *items, size_t nitems, const rx_metadata *metadata)
    {
      // printf("W0: %08x  TS: %08x\n", metadata->word0, metadata->timestamp);
      // printf("I0: %08x\n", items[0]);
      
      d_nsamples += nitems;
      d_nframes++;
      
      return !has_finished_p();
    }
  };
  
} /* namespace usrp2 */

#endif /* INCLUDED_RX_NOP_HANDLER */
