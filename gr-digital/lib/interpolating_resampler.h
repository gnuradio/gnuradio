/* -*- c++ -*- */
/*
 * Copyright (C) 2017 Free Software Foundation, Inc.
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
 * along with this file; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DIGITAL_INTERPOLATING_RESAMPLER_H
#define INCLUDED_DIGITAL_INTERPOLATING_RESAMPLER_H

#include <gnuradio/gr_complex.h>
#include <gnuradio/digital/interpolating_resampler_type.h>
#include <vector>
#include <gnuradio/filter/mmse_fir_interpolator_cc.h>
#include <gnuradio/filter/mmse_fir_interpolator_ff.h>
#include <gnuradio/filter/mmse_interp_differentiator_cc.h>
#include <gnuradio/filter/mmse_interp_differentiator_ff.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Base class for the composite interpolating resampler objects
     * used by the symbol_synchronizer_xx blocks.
     * \ingroup internal
     *
     * \details
     * This is the base class for the composite interpolating resampler
     * objects used by the symbol_synchronizer_xx blocks to provide a
     * user selectable interpolating resampler type.
     *
     * This base class provides the enumeration type for the available
     * types of interpolating resamplers.
     *
     * This base class also provides methods to to update, manage, and
     * store the sample phase state of the interpolating resampler.
     * The sample phase increments and phase state are assumed to be in
     * units of samples, and a complete sample phase cycle is one sample.
     */
    class interpolating_resampler
    {
      public:

        virtual ~interpolating_resampler() {}

        /*!
         * \brief Return the number of taps used in any single FIR filtering
         * operation
         */
        virtual unsigned int ntaps() const = 0;

        /*!
         * \brief Return the current unwrapped interpolator samples phase in
         * units of samples
         */
        virtual float phase()         { return d_phase; }

        /*!
         * \brief Return the integral part of the current unwrapped
         * interpolator sample phase in units of (whole) samples
         */
        virtual int   phase_n()       { return d_phase_n; }

        /*!
         * \brief Returns the fractional part of the current wrapped
         * interpolator sample phase in units of samples from [0.0, 1.0).
         */
        virtual float phase_wrapped() { return d_phase_wrapped; }

        /*!
         * \brief Return the fractional part of the previous wrapped
         * interpolator sample phase in units of samples from [0.0, 1.0).
         */
        virtual float prev_phase_wrapped() { return d_prev_phase_wrapped; }

        /*!
         * \brief Compute the next interpolator sample phase.
         * \param increment The sample phase increment to the next interpolation
         *                  point, in units of samples.
         * \param phase     The new interpolator sample phase, in units of
         *                  samples.
         * \param phase_n   The integral part of the new interpolator sample
         *                  phase, in units of samples.
         * \param phase_wrapped The new wrapped interpolator sample phase,
         *                      in units of samples.
         */
        virtual void next_phase(float increment,
                                float &phase,
                                int &phase_n,
                                float &phase_wrapped);

        /*!
         * \brief Advance the phase state to the next interpolator sample phase.
         * \param increment The sample phase increment to the next interpolation
         *                  point, in units of samples.
         */
        virtual void advance_phase(float increment);

        /*!
         * \brief Revert the phase state to the previous interpolator sample
         * phase.
         */
        virtual void revert_phase();

        /*!
         * \brief Reset the phase state to the specified interpolator sample
         * phase.
         * \param phase The interpolator sample phase to which to reset,
         *              in units of samples.
         */
        virtual void sync_reset(float phase);

      private:
        enum ir_type d_type;

      protected:
        interpolating_resampler(enum ir_type type, bool derivative = false);

        bool d_derivative;

        float d_phase;
        float d_phase_wrapped;
        int   d_phase_n;
        float d_prev_phase;
        float d_prev_phase_wrapped;
        int   d_prev_phase_n;
    };

    /*************************************************************************/

    /*!
     * \brief A complex input, complex output, composite interpolating resampler
     * object used by the symbol_synchronizer_cc block.
     * \ingroup internal
     *
     * \details
     * This is the complex input, complex output composite interpolating
     * resampler object used by the symbol_synchronizer_cc block to provide a
     * user selectable interpolating resampler type.
     *
     * This class abstracts away the underlying interpolating resampler
     * type implementation, so the the symbol_synchronizer_cc block need not
     * be bothered with the underlying implementation after the object is
     * instantiated.
     */
    class interpolating_resampler_ccf : public interpolating_resampler
    {
      public:
        /*!
         * \brief Create a complex input, complex output interpolating
         * resampler object.
         * \param type The underlying type implementation of the interpolating
         *             resampler.
         * \param derivative True if an interpolating differentitator is
         *                   requested to be initialized to obtain interpolated
         *                   derivative samples as well.
         * \param nfilts The number of polyphase filter bank arms.  Only needed
         *               for some types.
         * \param taps   Prototype filter for the polyphase filter bank. Only
         *               needed for some types.
         */
        static interpolating_resampler_ccf *make(enum ir_type type,
                                                 bool derivative = false,
                                                 int nfilts = 32,
                                                 const std::vector<float> &taps
                                                        = std::vector<float>());

        virtual ~interpolating_resampler_ccf() {};

        /*!
         * \brief Return an interpolated sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        virtual gr_complex interpolate(const gr_complex input[],
                                       float mu) const = 0;

        /*!
         * \brief Return an interpolated derivative sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        virtual gr_complex differentiate(const gr_complex input[],
                                         float mu) const = 0;

      protected:
        interpolating_resampler_ccf(enum ir_type type,
                                    bool derivative = false)
        : interpolating_resampler(type, derivative) {}
    };

    /*************************************************************************/

    /*!
     * \brief A float input, float output, composite interpolating resampler
     * object used by the symbol_synchronizer_ff block.
     * \ingroup internal
     *
     * \details
     * This is the float input, float output composite interpolating
     * resampler object used by the symbol_synchronizer_ff block to provide a
     * user selectable interpolating resampler type.
     *
     * This class abstracts away the underlying interpolating resampler
     * type implementation, so the the symbol_synchronizer_ff block need not
     * be bothered with the underlying implementation after the object is
     * instantiated.
     */
    class interpolating_resampler_fff : public interpolating_resampler
    {
      public:
        /*!
         * \brief Create a float input, float output interpolating
         * resampler object.
         * \param type The underlying type implementation of the interpolating
         *             resampler.
         * \param derivative True if an interpolating differentitator is
         *                   requested to be initialized to obtain interpolated
         *                   derivative samples as well.
         * \param nfilts The number of polyphase filter bank arms.  Only needed
         *               for some types.
         * \param taps   Prototype filter for the polyphase filter bank. Only
         *               needed for some types.
         */
        static interpolating_resampler_fff *make(enum ir_type type,
                                                 bool derivative = false,
                                                 int nfilts = 32,
                                                 const std::vector<float> &taps
                                                        = std::vector<float>());

        virtual ~interpolating_resampler_fff() {};

        /*!
         * \brief Return an interpolated sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        virtual float interpolate(const float input[], float mu) const = 0;

        /*!
         * \brief Return an interpolated derivative sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        virtual float differentiate(const float input[], float mu) const = 0;

      protected:
        interpolating_resampler_fff(enum ir_type type,
                                    bool derivative = false)
        : interpolating_resampler(type, derivative) {}
    };

    /*************************************************************************/

    /*!
     * \brief A complex input, complex output, interpolating resampler
     * object using the MMSE interpolator filter bank.
     * \ingroup internal
     *
     * \details
     * This is the complex input, complex output, interpolating resampler
     * object using the MMSE interpolator filter bank as its underlying
     * polyphase filterbank interpolator.
     */
    class interp_resampler_mmse_8tap_cc : public interpolating_resampler_ccf
    {
      public:
        /*!
         * \brief Create a complex input, complex output, polyphase filter bank
         * using the MMSE filter bank, interpolating resampler object.
         * \param derivative True if an interpolating differentitator is
         *                   requested to be initialized to obtain interpolated
         *                   derivative samples as well.
         */
        interp_resampler_mmse_8tap_cc(bool derivative = false);
        ~interp_resampler_mmse_8tap_cc();

        /*!
         * \brief Return the number of taps used in any single FIR filtering
         * operation
         */
        unsigned int ntaps() const;

        /*!
         * \brief Return an interpolated sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        gr_complex interpolate(const gr_complex input[], float mu) const;

        /*!
         * \brief Return an interpolated derivative sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        gr_complex differentiate(const gr_complex input[], float mu) const;

      private:
        filter::mmse_fir_interpolator_cc *d_interp;
        filter::mmse_interp_differentiator_cc *d_interp_diff;
    };

    /*!
     * \brief A float input, float output, interpolating resampler
     * object using the MMSE interpolator filter bank.
     * \ingroup internal
     *
     * \details
     * This is the float input, float output, interpolating resampler
     * object using the MMSE interpolator filter bank as its underlying
     * polyphase filterbank interpolator.
     */
    class interp_resampler_mmse_8tap_ff : public interpolating_resampler_fff
    {
      public:
        /*!
         * \brief Create a float input, float output, polyphase filter bank
         * using the MMSE filter bank, interpolating resampler object.
         * \param derivative True if an interpolating differentitator is
         *                   requested to be initialized to obtain interpolated
         *                   derivative samples as well.
         */
        interp_resampler_mmse_8tap_ff(bool derivative = false);
        ~interp_resampler_mmse_8tap_ff();

        /*!
         * \brief Return the number of taps used in any single FIR filtering
         * operation
         */
        unsigned int ntaps() const;

        /*!
         * \brief Return an interpolated sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        float interpolate(const float input[], float mu) const;

        /*!
         * \brief Return an interpolated derivative sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        float differentiate(const float input[], float mu) const;

      private:
        filter::mmse_fir_interpolator_ff *d_interp;
        filter::mmse_interp_differentiator_ff *d_interp_diff;
    };

    /*************************************************************************/

    /*!
     * \brief A complex input, complex output, interpolating resampler
     * object with a polyphase filter bank which uses the MMSE interpolator
     * filter arms.
     * \ingroup internal
     *
     * \details
     * This is the complex input, complex output, interpolating resampler
     * object with a polyphase filter bank which uses the MMSE interpolator
     * filter arms.  This class has the "advantage" that the number of arms
     * used can be reduced from 128 (default) to 64, 32, 16, 8, 4, or 2.
     */
    class interp_resampler_pfb_no_mf_cc : public interpolating_resampler_ccf
    {
      public:
        /*!
         * \brief Create a complex input, complex output, polyphase filter bank
         * using the MMSE filter bank, interpolating resampler object.
         * \param nfilts The number of polyphase filter bank arms.  Must be in
         *               {2, 4, 8, 16, 32, 64, 128 (default)}
         * \param derivative True if an interpolating differentitator is
         *                   requested to be initialized to obtain interpolated
         *                   derivative samples as well.
         */
        interp_resampler_pfb_no_mf_cc(bool derivative = false,
                                      int nfilts = 128);
        ~interp_resampler_pfb_no_mf_cc();

        /*!
         * \brief Return the number of taps used in any single FIR filtering
         * operation
         */
        unsigned int ntaps() const;

        /*!
         * \brief Return an interpolated sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        gr_complex interpolate(const gr_complex input[], float mu) const;

        /*!
         * \brief Return an interpolated derivative sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        gr_complex differentiate(const gr_complex input[], float mu) const;

      private:
        int d_nfilters;
        std::vector<filter::kernel::fir_filter_ccf*> d_filters;
        std::vector<filter::kernel::fir_filter_ccf*> d_diff_filters;
    };

    /*!
     * \brief A float input, float output, interpolating resampler
     * object with a polyphase filter bank which uses the MMSE interpolator
     * filter arms.
     * \ingroup internal
     *
     * \details
     * This is the float input, float output, interpolating resampler
     * object with a polyphase filter bank which uses the MMSE interpolator
     * filter arms.  This class has the "advantage" that the number of arms
     * used can be reduced from 128 (default) to 64, 32, 16, 8, 4, or 2.
     */
    class interp_resampler_pfb_no_mf_ff : public interpolating_resampler_fff
    {
      public:
        /*!
         * \brief Create a float input, float output, polyphase filter bank
         * using the MMSE filter bank, interpolating resampler object.
         * \param nfilts The number of polyphase filter bank arms.  Must be in
         *               {2, 4, 8, 16, 32, 64, 128 (default)}
         * \param derivative True if an interpolating differentitator is
         *                   requested to be initialized to obtain interpolated
         *                   derivative samples as well.
         */
        interp_resampler_pfb_no_mf_ff(bool derivative = false,
                                      int nfilts = 128);
        ~interp_resampler_pfb_no_mf_ff();

        /*!
         * \brief Return the number of taps used in any single FIR filtering
         * operation
         */
        unsigned int ntaps() const;

        /*!
         * \brief Return an interpolated sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        float interpolate(const float input[], float mu) const;

        /*!
         * \brief Return an interpolated derivative sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        float differentiate(const float input[], float mu) const;

      private:
        int d_nfilters;
        std::vector<filter::kernel::fir_filter_fff*> d_filters;
        std::vector<filter::kernel::fir_filter_fff*> d_diff_filters;
    };

    /*************************************************************************/

    /*!
     * \brief A complex input, complex output, interpolating resampler
     * object with a polyphase filter bank with a user provided prototype
     * matched filter.
     * \ingroup internal
     *
     * \details
     * This is the complex input, complex output, interpolating resampler
     * object with a polyphase filter bank with a user provided prototype
     * matched filter.  The prototype matched filter must be designed at a
     * rate of nfilts times the output rate.
     */
    class interp_resampler_pfb_mf_ccf : public interpolating_resampler_ccf
    {
      public:
        /*!
         * \brief Create a complex input, complex output, polyphase filter bank,
         * with matched filter, interpolating resampler object.
         * \param taps   Prototype filter for the polyphase filter bank.
         * \param nfilts The number of polyphase filter bank arms.
         * \param derivative True if an interpolating differentitator is
         *                   requested to be initialized to obtain interpolated
         *                   derivative samples as well.
         */
        interp_resampler_pfb_mf_ccf(const std::vector<float> &taps,
                                    int nfilts = 32,
                                    bool derivative = false);
        ~interp_resampler_pfb_mf_ccf();

        /*!
         * \brief Return the number of taps used in any single FIR filtering
         * operation
         */
        unsigned int ntaps() const;

        /*!
         * \brief Return an interpolated sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        gr_complex interpolate(const gr_complex input[], float mu) const;

        /*!
         * \brief Return an interpolated derivative sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        gr_complex differentiate(const gr_complex input[], float mu) const;

      private:
        int d_nfilters;
        const unsigned int d_taps_per_filter;
        std::vector<filter::kernel::fir_filter_ccf*> d_filters;
        std::vector<filter::kernel::fir_filter_ccf*> d_diff_filters;

        std::vector< std::vector<float> > d_taps;
        std::vector< std::vector<float> > d_diff_taps;
    };

    /*!
     * \brief A float input, float output, interpolating resampler
     * object with a polyphase filter bank with a user provided prototype
     * matched filter.
     * \ingroup internal
     *
     * \details
     * This is the float input, float output, interpolating resampler
     * object with a polyphase filter bank with a user provided prototype
     * matched filter.  The prototype matched filter must be designed at a
     * rate of nfilts times the output rate.
     */
    class interp_resampler_pfb_mf_fff : public interpolating_resampler_fff
    {
      public:
        /*!
         * \brief Create a float input, float output, polyphase filter bank,
         * with matched filter, interpolating resampler object.
         * \param taps   Prototype filter for the polyphase filter bank.
         * \param nfilts The number of polyphase filter bank arms.
         * \param derivative True if an interpolating differentitator is
         *                   requested to be initialized to obtain interpolated
         *                   derivative samples as well.
         */
        interp_resampler_pfb_mf_fff(const std::vector<float> &taps,
                                    int nfilts = 32,
                                    bool derivative = false);
        ~interp_resampler_pfb_mf_fff();

        /*!
         * \brief Return the number of taps used in any single FIR filtering
         * operation
         */
        unsigned int ntaps() const;

        /*!
         * \brief Return an interpolated sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        float interpolate(const float input[], float mu) const;

        /*!
         * \brief Return an interpolated derivative sample.
         * \param input Array of input samples of length ntaps().
         * \param mu Intersample phase in the range [0.0, 1.0] samples.
         */
        float differentiate(const float input[], float mu) const;

      private:
        int d_nfilters;
        const unsigned int d_taps_per_filter;
        std::vector<filter::kernel::fir_filter_fff*> d_filters;
        std::vector<filter::kernel::fir_filter_fff*> d_diff_filters;

        std::vector< std::vector<float> > d_taps;
        std::vector< std::vector<float> > d_diff_taps;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_INTERPOLATING_RESAMPLER_H */
