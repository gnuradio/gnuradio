/* -*- c++ -*- */
/*
 * Copyright 2010-2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CONSTELLATION_H
#define	INCLUDED_DIGITAL_CONSTELLATION_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/metric_type.h>
#include <boost/enable_shared_from_this.hpp>
#include <boost/any.hpp>
#include <gnuradio/gr_complex.h>
#include <pmt/pmt.h>
#include <vector>

namespace gr {
  namespace digital {

    /************************************************************/
    /* constellation                                            */
    /*                                                          */
    /* Base class defining interface.                           */
    /************************************************************/

    class constellation;
    typedef boost::shared_ptr<constellation> constellation_sptr;

    /*!
     * \brief An abstracted constellation object
     * \ingroup symbol_coding_blk
     *
     * \details
     * The constellation objects hold the necessary information to pass
     * around constellation information for modulators and
     * demodulators. These objects contain the mapping between the bits
     * and the constellation points used to represent them as well as
     * methods for slicing the symbol space. Various implementations are
     * possible for efficiency and ease of use.
     *
     * Standard constellations (BPSK, QPSK, QAM, etc) can be inherited
     * from this class and overloaded to perform optimized slicing and
     * constellation mappings.
     */
    class DIGITAL_API constellation
      : public boost::enable_shared_from_this<constellation>
    {
    public:
      constellation(std::vector<gr_complex> constell,
		    std::vector<int> pre_diff_code,
		    unsigned int rotational_symmetry,
		    unsigned int dimensionality);
      constellation();
      virtual ~constellation();

      //! Returns the constellation points for a symbol value
      void map_to_points(unsigned int value, gr_complex *points);
      std::vector<gr_complex> map_to_points_v(unsigned int value);

      //! Returns the constellation point that matches best.
      virtual unsigned int decision_maker(const gr_complex *sample) = 0;
      //! Takes a vector rather than a pointer.  Better for SWIG wrapping.
      unsigned int decision_maker_v(std::vector<gr_complex> sample);
      //! Also calculates the phase error.
      unsigned int decision_maker_pe(const gr_complex *sample, float *phase_error);
      //! Calculates distance.
      //unsigned int decision_maker_e(const gr_complex *sample, float *error);

      //! Calculates metrics for all points in the constellation.
      //! For use with the viterbi algorithm.
      virtual void calc_metric(const gr_complex *sample, float *metric, gr::digital::trellis_metric_type_t type);
      virtual void calc_euclidean_metric(const gr_complex *sample, float *metric);
      virtual void calc_hard_symbol_metric(const gr_complex *sample, float *metric);

      //! Returns the set of points in this constellation.
      std::vector<gr_complex> points() { return d_constellation;}
      //! Returns the vector of points in this constellation.
      //! Raise error if dimensionality is not one.
      std::vector<gr_complex> s_points();
      //! Returns a vector of vectors of points.
      std::vector<std::vector<gr_complex> > v_points();
      //! Whether to apply an encoding before doing differential encoding. (e.g. gray coding)
      bool apply_pre_diff_code() { return d_apply_pre_diff_code;}
      //! Whether to apply an encoding before doing differential encoding. (e.g. gray coding)
      void set_pre_diff_code(bool a) { d_apply_pre_diff_code = a;}
      //! Returns the encoding to apply before differential encoding.
      std::vector<int> pre_diff_code() { return d_pre_diff_code;}
      //! Returns the order of rotational symmetry.
      unsigned int rotational_symmetry() { return d_rotational_symmetry;}
      //! Returns the number of complex numbers in a single symbol.
      unsigned int dimensionality() {return d_dimensionality;}

      unsigned int bits_per_symbol()
      {
        return floor(log(double(d_constellation.size()))/d_dimensionality/log(2.0));
      }

      unsigned int arity()
      {
        return d_arity;
      }

      constellation_sptr base()
      {
        return shared_from_this();
      }

      pmt::pmt_t as_pmt()
      {
        return pmt::make_any(boost::any(base()));
      }

      /*! \brief Generates the soft decision LUT based on
       *         constellation and symbol map.
       *
       * \details Generates the soft decision LUT based on
       * constellation and symbol map.  It can be given a estimate of
       * the noise power in the channel as \p npwr.
       *
       * \param precision Number of bits of precision on each axis.
       * \param npwr Estimate of the noise power (if known).
       *
       * This is expensive to compute.
       */
      void gen_soft_dec_lut(int precision, float npwr=1.0);

      /*! \brief Calculate soft decisions for the given \p sample.
       *
       * \details Calculate the soft decisions from the given \p sample
       * at the given noise power \p npwr.
       *
       * This is a very costly algorithm (especially for higher order
       * modulations) and should be used sparingly. It uses the
       * #gen_soft_dec_lut function to generate the LUT, which
       * should be done once or if a large change in the noise floor
       * is detected.
       *
       * Instead of using this function, generate the LUT using the
       * #gen_soft_dec_lut after creating the constellation object
       * and then use the #soft_decision_maker function to return the
       * answer from the LUT.
       *
       * \param sample The complex sample to get the soft decisions.
       * \param npwr Estimate of the noise power (if known).
       */
      virtual std::vector<float> calc_soft_dec(gr_complex sample, float npwr=1.0);

      /*! \brief Define a soft decision look-up table.
       *
       * \details Define a soft decision look-up table (LUT). Because
       * soft decisions can be calculated in various ways with various
       * levels of accuracy and complexity, this function allows
       * users to create a LUT in their own way.
       *
       * Setting the LUT here means that #has_soft_dec_lut will return
       * true. Decision vectors returned by #soft_decision_maker will
       * be calculated using this LUT.
       *
       * \param soft_dec_lut The soft decision LUT as a vector of
       *        tuples (vectors in C++) of soft decisions. Each
       *        element of the LUT is a vector of k-bit floats (where
       *        there are k bits/sample in the constellation).
       * \param precision The number of bits of precision used when
       *        generating the LUT.
       */
      void set_soft_dec_lut(const std::vector< std::vector<float> > &soft_dec_lut,
                            int precision);

      //! Returns True if the soft decision LUT has been defined, False otherwise.
      bool has_soft_dec_lut();


      std::vector< std::vector<float> > soft_dec_lut();



      /*! \brief Returns the soft decisions for the given \p sample.
       *
       * \details Returns the soft decisions for the given \p
       * sample. If a LUT is defined for the object, the decisions
       * will be calculated from there. Otherwise, this function will
       * call calc_soft_dec directly to calculate the soft decisions.
       *
       * \param sample The complex sample to get the soft decisions.
       */
      std::vector<float> soft_decision_maker(gr_complex sample);


    protected:
      std::vector<gr_complex> d_constellation;
      std::vector<int> d_pre_diff_code;
      bool d_apply_pre_diff_code;
      unsigned int d_rotational_symmetry;
      unsigned int d_dimensionality;
      unsigned int d_arity;
      //! The factor by which the user given constellation points were
      //! scaled by to achieve an average amplitude of 1.
      float d_scalefactor;
      float d_re_min, d_re_max, d_im_min, d_im_max;

      std::vector< std::vector<float> > d_soft_dec_lut;
      int d_lut_precision;
      float d_lut_scale;

      float get_distance(unsigned int index, const gr_complex *sample);
      unsigned int get_closest_point(const gr_complex *sample);
      void calc_arity();

      void max_min_axes();
    };

    /************************************************************/
    /* constellation_calcdist                                   */
    /*                                                          */
    /************************************************************/

    /*! \brief Calculate Euclidian distance for any constellation
     *  \ingroup digital
     *
     * \details
     * Constellation which calculates the distance to each point in the
     * constellation for decision making. Inefficient for large
     * constellations.
     */
    class DIGITAL_API constellation_calcdist
      : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_calcdist> sptr;

      /*!
       * Make a general constellation object that calculates the Euclidean distance for hard decisions.
       *
       * \param constell List of constellation points (order of list matches pre_diff_code)
       * \param pre_diff_code List of alphabet symbols (before applying any differential
       *                      coding) (order of list matches constell)
       * \param rotational_symmetry Number of rotations around unit circle that have the same representation.
       * \param dimensionality Number of dimensions to the constellation.
       */
      static sptr make(std::vector<gr_complex> constell,
		       std::vector<int> pre_diff_code,
		       unsigned int rotational_symmetry,
		       unsigned int dimensionality);

      unsigned int decision_maker(const gr_complex *sample);
      // void calc_metric(gr_complex *sample, float *metric, trellis_metric_type_t type);
      // void calc_euclidean_metric(gr_complex *sample, float *metric);
      // void calc_hard_symbol_metric(gr_complex *sample, float *metric);

    protected:
      constellation_calcdist(std::vector<gr_complex> constell,
			     std::vector<int> pre_diff_code,
			     unsigned int rotational_symmetry,
			     unsigned int dimensionality);
    };


    /************************************************************/
    /*! constellation_sector                                    */
    /************************************************************/

    /*!
     * \brief Sectorized digital constellation
     * \ingroup digital
     *
     * \details
     * Constellation space is divided into sectors. Each sector is
     * associated with the nearest constellation point.
     */
    class DIGITAL_API constellation_sector : public constellation
    {
    public:

      /*!
       * Make a sectorized constellation object.
       *
       * \param constell List of constellation points (order of list matches pre_diff_code)
       * \param pre_diff_code List of alphabet symbols (before applying any differential
       *                      coding) (order of list matches constell)
       * \param rotational_symmetry Number of rotations around unit circle that have the same representation.
       * \param dimensionality Number of z-axis dimensions to the constellation
       * \param n_sectors Number of sectors in the constellation.
       */
      constellation_sector(std::vector<gr_complex> constell,
			   std::vector<int> pre_diff_code,
			   unsigned int rotational_symmetry,
			   unsigned int dimensionality,
			   unsigned int n_sectors);

      ~constellation_sector();

      unsigned int decision_maker(const gr_complex *sample);

    protected:
      virtual unsigned int get_sector(const gr_complex *sample) = 0;
      virtual unsigned int calc_sector_value(unsigned int sector) = 0;
      void find_sector_values();

      unsigned int n_sectors;

    private:
      std::vector<int> sector_values;
    };

    /************************************************************/
    /* constellation_rect                                       */
    /************************************************************/

    /*!
     * \brief Rectangular digital constellation
     * \ingroup digital
     *
     * Only implemented for 1-(complex)dimensional constellation.
     *
     * Constellation space is divided into rectangular sectors. Each
     * sector is associated with the nearest constellation point.
     *
     * Works well for square QAM.
     *
     * Works for any generic constellation provided sectors are not
     * too large.
     */
    class DIGITAL_API constellation_rect
      : public constellation_sector
    {
    public:
      typedef boost::shared_ptr<constellation_rect> sptr;

      /*!
       * Make a rectangular constellation object.
       *
       * \param constell List of constellation points (order of list matches pre_diff_code)
       * \param pre_diff_code List of alphabet symbols (before applying any differential
       *                      coding) (order of list matches constell)
       * \param rotational_symmetry Number of rotations around unit circle that have the same representation.
       * \param real_sectors Number of sectors the real axis is split in to.
       * \param imag_sectors Number of sectors the imag axis is split in to.
       * \param width_real_sectors width of each real sector to calculate decision boundaries.
       * \param width_imag_sectors width of each imag sector to calculate decision boundaries.
       */
      static constellation_rect::sptr make(std::vector<gr_complex> constell,
					   std::vector<int> pre_diff_code,
					   unsigned int rotational_symmetry,
					   unsigned int real_sectors,
					   unsigned int imag_sectors,
					   float width_real_sectors,
					   float width_imag_sectors);
      ~constellation_rect();

    protected:

      constellation_rect(std::vector<gr_complex> constell,
			 std::vector<int> pre_diff_code,
			 unsigned int rotational_symmetry,
			 unsigned int real_sectors,
			 unsigned int imag_sectors,
			 float width_real_sectors,
			 float width_imag_sectors);

      unsigned int get_sector(const gr_complex *sample);
      gr_complex calc_sector_center(unsigned int sector);
      unsigned int calc_sector_value(unsigned int sector);

    private:
      unsigned int n_real_sectors;
      unsigned int n_imag_sectors;
      float d_width_real_sectors;
      float d_width_imag_sectors;
    };


    /************************************************************/
    /* constellation_expl_rect                                  */
    /************************************************************/

    /*!
     * \brief Rectangular digital constellation.
     * \ingroup digital
     *
     * \details
     * Only implemented for 1-(complex)dimensional constellation.
     *
     * Constellation space is divided into rectangular sectors. Each
     * sector is associated with the nearest constellation point.
     *
     * This class is different from constellation_rect in that the
     * mapping from sector to constellation point is explicitly passed
     * into the constructor as sector_values.  Usually we do not need
     * this, since we want each sector to be automatically mapped to
     * the closest constellation point, however sometimes it's nice to
     * have the flexibility.
     */
    class DIGITAL_API constellation_expl_rect
      : public constellation_rect
    {
    public:
      typedef boost::shared_ptr<constellation_expl_rect> sptr;

      static sptr make(std::vector<gr_complex> constellation,
                       std::vector<int> pre_diff_code,
                       unsigned int rotational_symmetry,
                       unsigned int real_sectors,
                       unsigned int imag_sectors,
                       float width_real_sectors,
                       float width_imag_sectors,
                       std::vector<unsigned int> sector_values);
      ~constellation_expl_rect();

    protected:
      constellation_expl_rect(std::vector<gr_complex> constellation,
                              std::vector<int> pre_diff_code,
                              unsigned int rotational_symmetry,
                              unsigned int real_sectors,
                              unsigned int imag_sectors,
                              float width_real_sectors,
                              float width_imag_sectors,
                              std::vector<unsigned int> sector_values);

      unsigned int calc_sector_value (unsigned int sector) {
        return d_sector_values[sector];
      }

    private:
      std::vector<unsigned int> d_sector_values;
    };

    /************************************************************/
    /* constellation_psk                                        */
    /************************************************************/

    /*!
     * \brief constellation_psk
     * \ingroup digital
     *
     * Constellation space is divided into pie slices sectors.
     *
     * Each slice is associated with the nearest constellation point.
     *
     * Works well for PSK but nothing else.
     *
     * Assumes that there is a constellation point at 1.x
     */
    class DIGITAL_API constellation_psk : public constellation_sector
    {
    public:
      typedef boost::shared_ptr<constellation_psk> sptr;

      // public constructor
      static sptr make(std::vector<gr_complex> constell,
		       std::vector<int> pre_diff_code,
		       unsigned int n_sectors);

      ~constellation_psk();

    protected:
      unsigned int get_sector(const gr_complex *sample);

      unsigned int calc_sector_value(unsigned int sector);

      constellation_psk(std::vector<gr_complex> constell,
			std::vector<int> pre_diff_code,
			unsigned int n_sectors);
    };


    /************************************************************/
    /* constellation_bpsk                                       */
    /*                                                          */
    /* Only works for BPSK.                                     */
    /*                                                          */
    /************************************************************/

    /*!
     * \brief Digital constellation for BPSK .
     * \ingroup digital
     *
     * \details
     * \verbatim
       0 | 1
       \endverbatim
     */
    class DIGITAL_API constellation_bpsk : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_bpsk> sptr;

      // public constructor
      static sptr make();

      ~constellation_bpsk();

      unsigned int decision_maker(const gr_complex *sample);

    protected:
      constellation_bpsk();
    };


    /************************************************************/
    /* constellation_qpsk                                       */
    /*                                                          */
    /* Only works for QPSK.                                     */
    /*                                                          */
    /************************************************************/

    /*!
     * \brief Digital constellation for QPSK
     * \ingroup digital
     *
     * \details Constellation diagram assumes little endian format e.g. top, left means 2 not 1.
     * \verbatim
       01 | 11
       -------
       00 | 10
       \endverbatim
     */
    class DIGITAL_API constellation_qpsk : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_qpsk> sptr;

      // public constructor
      static sptr make();

      ~constellation_qpsk();

      unsigned int decision_maker(const gr_complex *sample);

    protected:
      constellation_qpsk();
    };


    /************************************************************/
    /* constellation_dqpsk                                      */
    /*                                                          */
    /* Works with differential encoding; slower decisions.      */
    /*                                                          */
    /************************************************************/

    /*!
     * \brief Digital constellation for DQPSK.
     * \ingroup digital
     *
     * \details
     * \verbatim
       01 | 00
       -------
       11 | 10
       \endverbatim
     */
    class DIGITAL_API constellation_dqpsk : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_dqpsk> sptr;

      // public constructor
      static sptr make();

      ~constellation_dqpsk();

      unsigned int decision_maker(const gr_complex *sample);

    protected:
      constellation_dqpsk();
    };


    /************************************************************/
    /* constellation_8psk                                       */
    /*                                                          */
    /* Only works for 8PSK.                                     */
    /*                                                          */
    /************************************************************/

    /*!
     * \brief Digital constellation for 8PSK.
     * \ingroup digital
     *
     * \details
     * \verbatim
           101 | 100
       001     |     000
       -----------------
       011     |     010
           111 | 110
       \endverbatim
     */
    class DIGITAL_API constellation_8psk : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_8psk> sptr;

      // public constructor
      static sptr make();

      ~constellation_8psk();

      unsigned int decision_maker(const gr_complex *sample);

    protected:
      constellation_8psk();
    };

    /************************************************************/
    /* constellation_8psk_natural                                       */
    /*                                                          */
    /* Only works for natural 8psk                                     */
    /*                                                          */
    /************************************************************/

    /*!
     * \brief Digital constellation for natually mapped 8PSK.
     * \ingroup digital
     *
     * \details
     * \verbatim
           011 | 010
       100     |     001
       -----------------
       101     |     000
           110 | 111
       \endverbatim
     */
    class DIGITAL_API constellation_8psk_natural : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_8psk_natural> sptr;

      // public constructor
      static sptr make();

      ~constellation_8psk_natural();

      unsigned int decision_maker(const gr_complex *sample);

    protected:
      constellation_8psk_natural();
    };

    /************************************************************/
    /* constellation_16qam                                       */
    /*                                                          */
    /* the 16qam mapping used in set partition of tcm           */
    /*                                                          */
    /************************************************************/

    /*!
     * \brief Digital constellation for 16qam.
     * \ingroup digital
     *
     * \details
     * \verbatim
   1000   1101 | 1100   1001
               |
   1111   1010 | 1011   1110
       -----------------
   0100   0001 | 0000   0101
               |
   0011   0110 | 0111   0010
       \endverbatim
     */
    class DIGITAL_API constellation_16qam : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_16qam> sptr;

      // public constructor
      static sptr make();

      ~constellation_16qam();

      unsigned int decision_maker(const gr_complex *sample);

    protected:
      constellation_16qam();
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_H */
