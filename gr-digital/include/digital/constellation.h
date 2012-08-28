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

#include <digital/api.h>
#include <digital/metric_type.h>
#include <boost/enable_shared_from_this.hpp>
#include <gr_complex.h>
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
     * \ingroup digital
     *
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
      ~constellation();

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
      virtual void calc_metric(const gr_complex *sample, float *metric, trellis_metric_type_t type);
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

    protected:
      std::vector<gr_complex> d_constellation;
      std::vector<int> d_pre_diff_code; 
      bool d_apply_pre_diff_code;
      unsigned int d_rotational_symmetry;
      unsigned int d_dimensionality;
      unsigned int d_arity;

      float get_distance(unsigned int index, const gr_complex *sample);
      unsigned int get_closest_point(const gr_complex *sample);
      void calc_arity();
    };

    /************************************************************/
    /* constellation_calcdist                                   */
    /*                                                          */
    /************************************************************/

    /*! \brief Calculate Euclidian distance for any constellation
     *  \ingroup digital
     *
     * Constellation which calculates the distance to each point in the
     * constellation for decision making. Inefficient for large
     * constellations.
     */
    class DIGITAL_API constellation_calcdist
      : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_calcdist> sptr;

      // public constructor
      static sptr make(std::vector<gr_complex> constell,
		       std::vector<int> pre_diff_code,
		       unsigned int rotational_symmetry,
		       unsigned int dimensionality);

      unsigned int decision_maker(const gr_complex *sample);
      // void calc_metric(gr_complex *sample, float *metric, trellis_metric_type_t type);
      // void calc_euclidean_metric(gr_complex *sample, float *metric);
      // void calc_hard_symbol_metric(gr_complex *sample, float *metric);

    private:
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
     * Constellation space is divided into sectors. Each sector is
     * associated with the nearest constellation point.
     *
     */
    class DIGITAL_API constellation_sector : public constellation
    {
    public:

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

      // public constructor
      static constellation_rect::sptr make(std::vector<gr_complex> constell,
					   std::vector<int> pre_diff_code,
					   unsigned int rotational_symmetry,
					   unsigned int real_sectors,
					   unsigned int imag_sectors,
					   float width_real_sectors,
					   float width_imag_sectors);
      ~constellation_rect();

    protected:
      unsigned int get_sector(const gr_complex *sample);
  
      unsigned int calc_sector_value(unsigned int sector);

    private:
      unsigned int n_real_sectors;
      unsigned int n_imag_sectors;
      float d_width_real_sectors;
      float d_width_imag_sectors;

      constellation_rect(std::vector<gr_complex> constell,
			 std::vector<int> pre_diff_code,
			 unsigned int rotational_symmetry,
			 unsigned int real_sectors,
			 unsigned int imag_sectors,
			 float width_real_sectors,
			 float width_imag_sectors);
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

    private:
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
     * \brief Digital constellation for BPSK 
     * \ingroup digital
     */
    class DIGITAL_API constellation_bpsk : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_bpsk> sptr;

      // public constructor
      static sptr make();

      ~constellation_bpsk();

      unsigned int decision_maker(const gr_complex *sample);

    private:
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
     */
    class DIGITAL_API constellation_qpsk : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_qpsk> sptr;

      // public constructor
      static sptr make();

      ~constellation_qpsk();

      unsigned int decision_maker(const gr_complex *sample);

    private:
      constellation_qpsk();
    };


    /************************************************************/
    /* constellation_dqpsk                                      */
    /*                                                          */
    /* Works with differential encoding; slower decisions.      */
    /*                                                          */
    /************************************************************/

    /*!
     * \brief Digital constellation for DQPSK
     * \ingroup digital
     */
    class DIGITAL_API constellation_dqpsk : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_dqpsk> sptr;

      // public constructor
      static sptr make();

      ~constellation_dqpsk();

      unsigned int decision_maker(const gr_complex *sample);

    private:
      constellation_dqpsk();
    };


    /************************************************************/
    /* constellation_8psk                                       */
    /*                                                          */
    /* Only works for 8PSK.                                     */
    /*                                                          */
    /************************************************************/

    /*! 
     * \brief Digital constellation for 8PSK
     * \ingroup digital
     */
    class DIGITAL_API constellation_8psk : public constellation
    {
    public:
      typedef boost::shared_ptr<constellation_8psk> sptr;

      // public constructor
      static sptr make();

      ~constellation_8psk();

      unsigned int decision_maker(const gr_complex *sample);

    private:
      constellation_8psk();
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_H */
