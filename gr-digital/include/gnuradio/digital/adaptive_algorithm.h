/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_ADAPTIVE_ALGORITHM_H
#define INCLUDED_DIGITAL_ADAPTIVE_ALGORITHM_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/math.h>
#include <algorithm>
#include <cmath>
#include <vector>

namespace gr {
namespace digital {
enum class adaptive_algorithm_t { LMS, NLMS, CMA };

class adaptive_algorithm;
typedef std::shared_ptr<adaptive_algorithm> adaptive_algorithm_sptr;

class DIGITAL_API adaptive_algorithm
    : public std::enable_shared_from_this<adaptive_algorithm>
{
protected:
    const adaptive_algorithm_t d_algorithm_type;
    const constellation_sptr d_constellation;

public:
    virtual ~adaptive_algorithm() {}

    adaptive_algorithm(adaptive_algorithm_t alg_type, constellation_sptr cons)
        : d_algorithm_type(alg_type), d_constellation(cons)
    {
    }

    adaptive_algorithm_sptr base() { return shared_from_this(); }

    virtual void initialize_taps(std::vector<gr_complex>& taps)
    {
        std::fill(taps.begin(), taps.end(), gr_complex(0.0, 0.0));
        taps[0] = gr_complex(1.0, 0.0); // default weights, overridden by derived classes
    }

    virtual gr_complex error_dd(gr_complex& wu, gr_complex& decision) const
    {
        // The `map_to_points` function will treat `decision` as an array pointer.
        // This call is "safe" because `map_to_points` is limited by the
        // dimensionality of the constellation. This class calls the
        // `constellation` class default constructor, which initializes the
        // dimensionality value to `1`. Thus, Only the single `gr_complex` value
        // will be dereferenced.
        d_constellation->map_to_points(d_constellation->decision_maker(&wu), &decision);
        return decision - wu;
    }

    virtual gr_complex error_tr(const gr_complex& wu, const gr_complex& d_n) const
    {
        return d_n - wu;
    }

    virtual gr_complex update_tap(const gr_complex tap,
                                  const gr_complex& in,
                                  const gr_complex error,
                                  const gr_complex decision) = 0;

    virtual void update_taps(gr_complex* taps,
                             const gr_complex* in,
                             const gr_complex error,
                             const gr_complex decision,
                             unsigned int num_taps)
    {
        // default tap update until update_taps is implemented for all the alg types
        // Performance can be improved in the derived algorithms by having a volk-ified
        //  update_taps method
        for (unsigned i = 0; i < num_taps; i++) {
            taps[i] = update_tap(taps[i], in[i], error, decision);
        }
    }
};

} // namespace digital
} // namespace gr
#endif