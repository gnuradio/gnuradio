/* -*- c++ -*- */
/*
 * Copyright 2010,2011,2012 Free Software Foundation, Inc.
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

%template(gr_complex_vector) std::vector<gr_complex>;
%template(unsigned_int_vector) std::vector<unsigned int>;

// Make sure metric types get SWIGed.
%include "digital_metric_type.h"

class digital_constellation;
typedef boost::shared_ptr<digital_constellation> digital_constellation_sptr;
%template(digital_constellation_sptr) boost::shared_ptr<digital_constellation>;

class digital_constellation
{
public:
  digital_constellation (std::vector<gr_complex> constellation,
			 std::vector<unsigned int> pre_diff_code,
			 unsigned int rotational_symmetry,
			 unsigned int dimensionality);
  std::vector<gr_complex> points();
  std::vector<gr_complex> s_points();
  std::vector<std::vector<gr_complex> > v_points();
  virtual unsigned int decision_maker (gr_complex *sample) = 0;  
  unsigned int decision_maker_v (std::vector<gr_complex> sample);  
  //  void calc_metric(gr_complex *sample, float *metric, trellis_metric_type_t type);
  //  void calc_euclidean_metric(gr_complex *sample, float *metric);
  //  void calc_hard_symbol_metric(gr_complex *sample, float *metric);
  std::vector<gr_complex> map_to_points_v(unsigned int value);
  unsigned int bits_per_symbol ();
  unsigned int arity ();
  digital_constellation_sptr base ();
  bool apply_pre_diff_code();
  void set_pre_diff_code(bool a);
  std::vector<unsigned int> pre_diff_code();
  unsigned int rotational_symmetry();
  unsigned int dimensionality();
};

class digital_constellation_calcdist;
typedef boost::shared_ptr<digital_constellation_calcdist> digital_constellation_calcdist_sptr;
%template(digital_constellation_calcdist_sptr) boost::shared_ptr<digital_constellation_calcdist>;
%rename(constellation_calcdist) digital_make_constellation_calcdist;
digital_constellation_calcdist_sptr
digital_make_constellation_calcdist(std::vector<gr_complex> constellation,
				    std::vector<unsigned int> pre_diff_code,
				    unsigned int rotational_symmetry,
				    unsigned int dimensionality);
%ignore digital_constellation_calcdist;

class digital_constellation_calcdist: public digital_constellation
{
 public:
  digital_constellation_calcdist (std::vector<gr_complex> constellation,
				  std::vector<unsigned int> pre_diff_code,
				  unsigned int rotational_symmetry,
				  unsigned int dimensionality);
  unsigned int decision_maker (const gr_complex *sample);
};

class digital_constellation_sector: public digital_constellation
{
};

class digital_constellation_rect;
typedef boost::shared_ptr<digital_constellation_rect> digital_constellation_rect_sptr;
%template(digital_constellation_rect_sptr) boost::shared_ptr<digital_constellation_rect>;
%rename(constellation_rect) digital_make_constellation_rect;
digital_constellation_rect_sptr digital_make_constellation_rect(std::vector<gr_complex> constellation,
								std::vector<unsigned int> pre_diff_code,
								unsigned int rotational_symmetry,
								unsigned int real_sectors, unsigned int imag_sectors,
								float width_real_sectors, float width_imag_sectors);
%ignore digital_constellation_rect;

class digital_constellation_rect : public digital_constellation_sector
{
public:
  digital_constellation_rect (std::vector<gr_complex> constellation,
			      std::vector<unsigned int> pre_diff_code,
			      unsigned int rotational_symmetry,
			      unsigned int real_sectors, unsigned int imag_sectors,
			      float width_real_sectors, float width_imag_sectors);
};

class digital_constellation_psk;
typedef boost::shared_ptr<digital_constellation_psk> digital_constellation_psk_sptr;
%template(digital_constellation_psk_sptr) boost::shared_ptr<digital_constellation_psk>;
%rename(constellation_psk) digital_make_constellation_psk;
digital_constellation_psk_sptr digital_make_constellation_psk(std::vector<gr_complex> constellation,
							      std::vector<unsigned int> pre_diff_code,
							      unsigned int n_sectors);
%ignore digital_constellation_psk;

class digital_constellation_psk : public digital_constellation_sector
{
public:
  digital_constellation_psk (std::vector<gr_complex> constellation,
			     std::vector<unsigned int> pre_diff_code,
			     unsigned int n_sectors);
};

/*
  BPSK Constellation
*/

class digital_constellation_bpsk;
typedef boost::shared_ptr<digital_constellation_bpsk> digital_constellation_bpsk_sptr;
%template(digital_constellation_bpsk_sptr) boost::shared_ptr<digital_constellation_bpsk>;
%rename(constellation_bpsk) digital_make_constellation_bpsk;
digital_constellation_bpsk_sptr digital_make_constellation_bpsk();
%ignore digital_constellation_bpsk;

class digital_constellation_bpsk : public digital_constellation
{
public:
  digital_constellation_bpsk ();
};

/*
  QPSK Constellation
*/

class digital_constellation_qpsk;
typedef boost::shared_ptr<digital_constellation_qpsk> digital_constellation_qpsk_sptr;
%template(digital_constellation_qpsk_sptr) boost::shared_ptr<digital_constellation_qpsk>;
%rename(constellation_qpsk) digital_make_constellation_qpsk;
digital_constellation_qpsk_sptr digital_make_constellation_qpsk();
%ignore digital_constellation_qpsk;

class digital_constellation_qpsk : public digital_constellation
{
public:
  digital_constellation_qpsk ();
};

/*
  DQPSK Constellation
*/

class digital_constellation_dqpsk;
typedef boost::shared_ptr<digital_constellation_dqpsk> digital_constellation_dqpsk_sptr;
%template(digital_constellation_dqpsk_sptr) boost::shared_ptr<digital_constellation_dqpsk>;
%rename(constellation_dqpsk) digital_make_constellation_dqpsk;
digital_constellation_dqpsk_sptr digital_make_constellation_dqpsk();
%ignore digital_constellation_dqpsk;

class digital_constellation_dqpsk : public digital_constellation
{
public:
  digital_constellation_dqpsk ();
};


/*
  8PSK Constellation
*/

class digital_constellation_8psk;
typedef boost::shared_ptr<digital_constellation_8psk> digital_constellation_8psk_sptr;
%template(digital_constellation_8psk_sptr) boost::shared_ptr<digital_constellation_8psk>;
%rename(constellation_8psk) digital_make_constellation_8psk;
digital_constellation_8psk_sptr digital_make_constellation_8psk();
%ignore digital_constellation_8psk;

class digital_constellation_8psk : public digital_constellation
{
public:
  digital_constellation_8psk ();
};

#if SWIGPYTHON
/*
  We want print(constellation) in python to produce nice useful output so
  we include code at the end of the generated python file that overrides
  the SWIG-generated __repr__ method.
 */
%pythoncode %{

digital_constellation_calcdist_sptr.__repr__ = lambda self: '<constellation calcdist (m=%s)>' % str(len(self.points()))
digital_constellation_rect_sptr.__repr__ = lambda self: '<constellation rect (m=%s)>' % str(len(self.points()))
digital_constellation_psk_sptr.__repr__ = lambda self: '<constellation psk (m=%s)>' % str(len(self.points()))
digital_constellation_bpsk_sptr.__repr__ = lambda self: '<constellation bpsk>'
digital_constellation_qpsk_sptr.__repr__ = lambda self: '<constellation qpsk>'
digital_constellation_dqpsk_sptr.__repr__ = lambda self: '<constellation dqpsk>'
digital_constellation_8psk_sptr.__repr__ = lambda self: '<constellation 8psk>'
  
%}
#endif
