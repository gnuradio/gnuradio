/* -*- c++ -*- */
/*
 * Copyright 2010, 2011 Free Software Foundation, Inc.
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

#include <digital_api.h>
#include <vector>
#include <math.h>
#include <gr_complex.h>
#include <boost/enable_shared_from_this.hpp>
#include <digital_metric_type.h>

/************************************************************/
/* digital_constellation                                    */
/*                                                          */
/* Base class defining interface.                           */
/************************************************************/

class digital_constellation;
typedef boost::shared_ptr<digital_constellation> digital_constellation_sptr;

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
class DIGITAL_API digital_constellation : public boost::enable_shared_from_this<digital_constellation>
{
public:
  digital_constellation (std::vector<gr_complex> constellation,
			 std::vector<unsigned int> pre_diff_code,
			 unsigned int rotational_symmetry,
			 unsigned int dimensionality);
  digital_constellation ();

  //! Returns the constellation points for a symbol value
  void map_to_points(unsigned int value, gr_complex *points);
  std::vector<gr_complex> map_to_points_v(unsigned int value);

  //! Returns the constellation point that matches best.
  virtual unsigned int decision_maker (const gr_complex *sample) = 0;
  //! Takes a vector rather than a pointer.  Better for SWIG wrapping.
  unsigned int decision_maker_v (std::vector<gr_complex> sample);
  //! Also calculates the phase error.
  unsigned int decision_maker_pe (const gr_complex *sample, float *phase_error);
  //! Calculates distance.
  unsigned int decision_maker_e (const gr_complex *sample, float *error);
  
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
  std::vector<unsigned int> pre_diff_code() { return d_pre_diff_code;}
  //! Returns the order of rotational symmetry.
  unsigned int rotational_symmetry() { return d_rotational_symmetry;}
  //! Returns the number of complex numbers in a single symbol.
  unsigned int dimensionality() {return d_dimensionality;}

  unsigned int bits_per_symbol () {
    return floor(log(double(d_constellation.size()))/d_dimensionality/log(2.0));
  }
  
  unsigned int arity () {
    return d_arity;
  }

  digital_constellation_sptr base() {
    return shared_from_this();
  }  

 protected:

  std::vector<gr_complex> d_constellation;
  std::vector<unsigned int> d_pre_diff_code; 
  bool d_apply_pre_diff_code;
  unsigned int d_rotational_symmetry;
  unsigned int d_dimensionality;
  unsigned int d_arity;

  float get_distance(unsigned int index, const gr_complex *sample);
  unsigned int get_closest_point(const gr_complex *sample);
  void calc_arity ();
};

/************************************************************/
/* digital_constellation_calcdist                           */
/*                                                          */
/************************************************************/

class digital_constellation_calcdist;
typedef boost::shared_ptr<digital_constellation_calcdist> digital_constellation_calcdist_sptr;

// public constructor
DIGITAL_API digital_constellation_calcdist_sptr
digital_make_constellation_calcdist (std::vector<gr_complex> constellation,
				     std::vector<unsigned int> pre_diff_code,
				     unsigned int rotational_symmetry,
				     unsigned int dimensionality);


/*! \brief Calculate Euclidian distance for any constellation
 *  \ingroup digital
 *
 * Constellation which calculates the distance to each point in the
 * constellation for decision making. Inefficient for large
 * constellations.
 */
class DIGITAL_API digital_constellation_calcdist : public digital_constellation
{
 public:
  digital_constellation_calcdist (std::vector<gr_complex> constellation,
				  std::vector<unsigned int> pre_diff_code,
				  unsigned int rotational_symmetry,
				  unsigned int dimensionality);
  unsigned int decision_maker (const gr_complex *sample);
  // void calc_metric(gr_complex *sample, float *metric, trellis_metric_type_t type);
  // void calc_euclidean_metric(gr_complex *sample, float *metric);
  // void calc_hard_symbol_metric(gr_complex *sample, float *metric);
  
 private:
  friend DIGITAL_API digital_constellation_calcdist_sptr
  digital_make_constellation_calcdist (std::vector<gr_complex> constellation);
};


/************************************************************/
/*! digital_constellation_sector                             */
/************************************************************/

/*!
 * \brief Sectorized digital constellation
 * \ingroup digital
 *
 * Constellation space is divided into sectors. Each sector is
 * associated with the nearest constellation point.
 *
 */
class DIGITAL_API digital_constellation_sector : public digital_constellation
{
 public:

  digital_constellation_sector (std::vector<gr_complex> constellation,
				std::vector<unsigned int> pre_diff_code,
				unsigned int rotational_symmetry,
				unsigned int dimensionality,
				unsigned int n_sectors);

  unsigned int decision_maker (const gr_complex *sample);

 protected:

  virtual unsigned int get_sector (const gr_complex *sample) = 0;
  virtual unsigned int calc_sector_value (unsigned int sector) = 0;
  void find_sector_values ();

  unsigned int n_sectors;

 private:

  std::vector<unsigned int> sector_values;

};

/************************************************************/
/* digital_constellation_rect                               */
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
 * Works for any generic constellation provided sectors are not too
 * large.
 */

class digital_constellation_rect;
typedef boost::shared_ptr<digital_constellation_rect> digital_constellation_rect_sptr;

// public constructor
DIGITAL_API digital_constellation_rect_sptr 
digital_make_constellation_rect (std::vector<gr_complex> constellation,
				 std::vector<unsigned int> pre_diff_code,
				 unsigned int rotational_symmetry,
				 unsigned int real_sectors,
				 unsigned int imag_sectors,
				 float width_real_sectors,
				 float width_imag_sectors);

class DIGITAL_API digital_constellation_rect : public digital_constellation_sector
{
 public:

  digital_constellation_rect (std::vector<gr_complex> constellation,
			      std::vector<unsigned int> pre_diff_code,
			      unsigned int rotational_symmetry,
			      unsigned int real_sectors,
			      unsigned int imag_sectors,
			      float width_real_sectors,
			      float width_imag_sectors);

 protected:

  unsigned int get_sector (const gr_complex *sample);
  
  unsigned int calc_sector_value (unsigned int sector);

 private:

  unsigned int n_real_sectors;
  unsigned int n_imag_sectors;
  float d_width_real_sectors;
  float d_width_imag_sectors;

  friend DIGITAL_API digital_constellation_rect_sptr
  digital_make_constellation_rect (std::vector<gr_complex> constellation,
				   std::vector<unsigned int> pre_diff_code,
				   unsigned int rotational_symmetry,
				   unsigned int real_sectors,
				   unsigned int imag_sectors,
				   float width_real_sectors,
				   float width_imag_sectors);
  
};


/************************************************************/
/* digital_constellation_psk                                */
/************************************************************/

class digital_constellation_psk;
typedef boost::shared_ptr<digital_constellation_psk> digital_constellation_psk_sptr;

// public constructor
DIGITAL_API digital_constellation_psk_sptr 
digital_make_constellation_psk (std::vector<gr_complex> constellation,
				std::vector<unsigned int> pre_diff_code,
				unsigned int n_sectors);

/*! 
 * \brief digital_constellation_psk
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
class DIGITAL_API digital_constellation_psk : public digital_constellation_sector
{
 public:

  digital_constellation_psk (std::vector<gr_complex> constellation,
			     std::vector<unsigned int> pre_diff_code,
			     unsigned int n_sectors);

 protected:

  unsigned int get_sector (const gr_complex *sample);
  
  unsigned int calc_sector_value (unsigned int sector);

 private:

  friend DIGITAL_API digital_constellation_psk_sptr
  digital_make_constellation_psk (std::vector<gr_complex> constellation,
				  std::vector<unsigned int> pre_diff_code,
				  unsigned int n_sectors);
  
};


/************************************************************/
/* digital_constellation_bpsk                               */
/*                                                          */
/* Only works for BPSK.                                     */
/*                                                          */
/************************************************************/

class digital_constellation_bpsk;
typedef boost::shared_ptr<digital_constellation_bpsk> digital_constellation_bpsk_sptr;

// public constructor
DIGITAL_API digital_constellation_bpsk_sptr 
digital_make_constellation_bpsk ();

/*! 
 * \brief Digital constellation for BPSK 
 * \ingroup digital
 */
class DIGITAL_API digital_constellation_bpsk : public digital_constellation
{
 public:

  digital_constellation_bpsk ();
  unsigned int decision_maker (const gr_complex *sample);

  friend DIGITAL_API digital_constellation_bpsk_sptr
  digital_make_constellation_bpsk ();
  
};


/************************************************************/
/* digital_constellation_qpsk                               */
/*                                                          */
/* Only works for QPSK.                                     */
/*                                                          */
/************************************************************/

class digital_constellation_qpsk;
typedef boost::shared_ptr<digital_constellation_qpsk> digital_constellation_qpsk_sptr;

// public constructor
DIGITAL_API digital_constellation_qpsk_sptr 
digital_make_constellation_qpsk ();

/*! 
 * \brief Digital constellation for QPSK
 * \ingroup digital
 */
class DIGITAL_API digital_constellation_qpsk : public digital_constellation
{
 public:

  digital_constellation_qpsk ();
  unsigned int decision_maker (const gr_complex *sample);

  friend DIGITAL_API digital_constellation_qpsk_sptr
  digital_make_constellation_qpsk ();
  
};


/************************************************************/
/* digital_constellation_dqpsk                              */
/*                                                          */
/* Works with differential encoding; slower decisions.      */
/*                                                          */
/************************************************************/

class digital_constellation_dqpsk;
typedef boost::shared_ptr<digital_constellation_dqpsk> digital_constellation_dqpsk_sptr;

// public constructor
DIGITAL_API digital_constellation_dqpsk_sptr 
digital_make_constellation_dqpsk ();

/*!
 * \brief Digital constellation for DQPSK
 * \ingroup digital
 */
class DIGITAL_API digital_constellation_dqpsk : public digital_constellation
{
 public:

  digital_constellation_dqpsk ();
  unsigned int decision_maker (const gr_complex *sample);

  friend DIGITAL_API digital_constellation_dqpsk_sptr
  digital_make_constellation_dqpsk ();
  
};


/************************************************************/
/* digital_constellation_8psk                               */
/*                                                          */
/* Only works for 8PSK.                                     */
/*                                                          */
/************************************************************/

class digital_constellation_8psk;
typedef boost::shared_ptr<digital_constellation_8psk> digital_constellation_8psk_sptr;

// public constructor
DIGITAL_API digital_constellation_8psk_sptr 
digital_make_constellation_8psk ();

/*! 
 * \brief Digital constellation for 8PSK
 * \ingroup digital
 */
class DIGITAL_API digital_constellation_8psk : public digital_constellation
{
 public:

  digital_constellation_8psk ();
  unsigned int decision_maker (const gr_complex *sample);

  friend DIGITAL_API digital_constellation_8psk_sptr
  digital_make_constellation_8psk ();
  
};

#endif
