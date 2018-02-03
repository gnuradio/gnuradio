/* -*- c++ -*- */
/*
 * Copyright 2010-2012,2014 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/math.h>
#include <gnuradio/gr_complex.h>
#include <cstdlib>
#include <cfloat>
#include <stdexcept>
#include <boost/format.hpp>
#include <iostream>

namespace gr {
  namespace digital {

#define M_TWOPI (2*M_PI)
#define SQRT_TWO 0.707107

    // Base Constellation Class
    constellation::constellation(std::vector<gr_complex> constell,
                                 std::vector<int> pre_diff_code,
                                 unsigned int rotational_symmetry,
                                 unsigned int dimensionality
    ) :
      d_constellation(constell),
      d_pre_diff_code(pre_diff_code),
      d_rotational_symmetry(rotational_symmetry),
      d_dimensionality(dimensionality),
      d_re_min(1e20),
      d_re_max(1e20),
      d_im_min(1e20),
      d_im_max(1e20),
      d_lut_precision(0),
      d_lut_scale(0)
    {
      // Scale constellation points so that average magnitude is 1.
      float summed_mag = 0;
      unsigned int constsize = d_constellation.size();
      for (unsigned int i=0; i<constsize; i++) {
        gr_complex c = d_constellation[i];
        summed_mag += sqrt(c.real()*c.real() + c.imag()*c.imag());
      }
      d_scalefactor = constsize/summed_mag;
      for (unsigned int i=0; i<constsize; i++) {
        d_constellation[i] = d_constellation[i]*d_scalefactor;
      }
      if(pre_diff_code.size() == 0)
        d_apply_pre_diff_code = false;
      else if(pre_diff_code.size() != constsize)
        throw std::runtime_error(
          "The constellation and pre-diff code must be of the same length.");
      else
        d_apply_pre_diff_code = true;
      calc_arity();
    }

    constellation::constellation() :
      d_apply_pre_diff_code(false),
      d_rotational_symmetry(0),
      d_dimensionality(1),
      d_scalefactor(1.0),
      d_re_min(1e20),
      d_re_max(1e20),
      d_im_min(1e20),
      d_im_max(1e20),
      d_lut_precision(0.0),
      d_lut_scale(0.0)
    {
      calc_arity();
    }

    constellation::~constellation()
    {
    }

    //! Returns the constellation points for a symbol value
    void
    constellation::map_to_points(unsigned int value, gr_complex *points)
    {
      for(unsigned int i=0; i<d_dimensionality; i++)
        points[i] = d_constellation[value*d_dimensionality + i];
    }

    std::vector<gr_complex>
    constellation::map_to_points_v(unsigned int value)
    {
      std::vector<gr_complex> points_v;
      points_v.resize(d_dimensionality);
      map_to_points(value, &(points_v[0]));
      return points_v;
    }

    float
    constellation::get_distance(unsigned int index, const gr_complex *sample)
    {
      float dist = 0;
      for(unsigned int i=0; i<d_dimensionality; i++) {
        dist += norm(sample[i] - d_constellation[index*d_dimensionality + i]);
      }
      return dist;
    }

    unsigned int
    constellation::get_closest_point(const gr_complex *sample)
    {
      unsigned int min_index = 0;
      float min_euclid_dist;
      float euclid_dist;

      min_euclid_dist = get_distance(0, sample);
      min_index = 0;
      for(unsigned int j = 1; j < d_arity; j++){
        euclid_dist = get_distance(j, sample);
        if(euclid_dist < min_euclid_dist){
          min_euclid_dist = euclid_dist;
          min_index = j;
        }
      }
      return min_index;
    }

    unsigned int
    constellation::decision_maker_pe(const gr_complex *sample,
                                     float *phase_error)
    {
      unsigned int index = decision_maker(sample);
      *phase_error = 0;
      for(unsigned int d=0; d<d_dimensionality; d++)
        *phase_error += -arg(sample[d]*conj(d_constellation[index+d]));
      return index;
    }

    std::vector<gr_complex> constellation::s_points()
    {
      if(d_dimensionality != 1)
        throw std::runtime_error(
          "s_points only works for dimensionality 1 constellations.");
      else
        return d_constellation;
    }

    std::vector<std::vector<gr_complex> >
    constellation::v_points()
    {
      std::vector<std::vector<gr_complex> > vv_const;
      vv_const.resize(d_arity);
      for(unsigned int p=0; p<d_arity; p++) {
        std::vector<gr_complex> v_const;
        v_const.resize(d_dimensionality);
        for(unsigned int d=0; d<d_dimensionality; d++) {
          v_const[d] = d_constellation[p*d_dimensionality+d];
        }
        vv_const[p] = v_const;
      }
      return vv_const;
    }

    void
    constellation::calc_metric(const gr_complex *sample, float *metric,
                               trellis_metric_type_t type)
    {
      switch(type){
      case TRELLIS_EUCLIDEAN:
        calc_euclidean_metric(sample, metric);
        break;
      case TRELLIS_HARD_SYMBOL:
        calc_hard_symbol_metric(sample, metric);
        break;
      case TRELLIS_HARD_BIT:
        throw std::runtime_error("Invalid metric type (not yet implemented).");
        break;
      default:
        throw std::runtime_error("Invalid metric type.");
      }
    }

    void
    constellation::calc_euclidean_metric(const gr_complex *sample,
                                         float *metric)
    {
      for(unsigned int o=0; o<d_arity; o++) {
        metric[o] = get_distance(o, sample);
      }
    }

    void
    constellation::calc_hard_symbol_metric(const gr_complex *sample,
                                           float *metric)
    {
      float minm = FLT_MAX;
      unsigned int minmi = 0;
      for(unsigned int o=0; o<d_arity; o++) {
        float dist = get_distance(o, sample);
        if(dist < minm) {
          minm = dist;
          minmi = o;
        }
      }
      for(unsigned int o=0; o<d_arity; o++) {
        metric[o] = (o==minmi?0.0:1.0);
      }
    }

    void
    constellation::calc_arity()
    {
      if(d_constellation.size() % d_dimensionality != 0)
        throw std::runtime_error(
          "Constellation vector size must be a multiple of the dimensionality.");
      d_arity = d_constellation.size()/d_dimensionality;
    }

    unsigned int
    constellation::decision_maker_v(std::vector<gr_complex> sample)
    {
      assert(sample.size() == d_dimensionality);
      return decision_maker(&(sample[0]));
    }


    void
    constellation::gen_soft_dec_lut(int precision, float npwr)
    {
      d_soft_dec_lut.clear();
      d_lut_scale = powf(2.0f, static_cast<float>(precision));

      // We know we've normalized the constellation, so the min/max
      // dimensions in either direction are scaled to +/-1.
      float maxd = 1.0f;
      float step = (2.0f*maxd) / (d_lut_scale-1);
      float y = -maxd;
      while(y < maxd+step) {
        float x = -maxd;
        while(x < maxd+step) {
          gr_complex pt = gr_complex(x, y);
          d_soft_dec_lut.push_back(calc_soft_dec(pt, npwr));
          x += step;
        }
        y += step;
      }

      d_lut_precision = precision;
    }

    std::vector<float>
    constellation::calc_soft_dec(gr_complex sample, float npwr)
    {
      int v;
      int M = static_cast<int>(d_constellation.size());
      int k = static_cast<int>(log(static_cast<double>(M))/log(2.0));
      std::vector<float> tmp(2*k, 0);
      std::vector<float> s(k, 0);

      for(int i = 0; i < M; i++) {
        // Calculate the distance between the sample and the current
        // constellation point.
        float dist = std::abs(sample - d_constellation[i]);
        // Calculate the probability factor from the distance and
        // the scaled noise power.
        float d = expf(-dist/npwr);

        if(d_apply_pre_diff_code)
          v = d_pre_diff_code[i];
        else
          v = i;

        for(int j = 0; j < k; j++) {
          // Get the bit at the jth index
          int mask = 1 << j;
          int bit = (v & mask) >> j;

          // If the bit is a 0, add to the probability of a zero
          if(bit == 0)
            tmp[2*j+0] += d;
          // else, add to the probability of a one
          else
            tmp[2*j+1] += d;
        }
      }

      // Calculate the log-likelihood ratio for all bits based on the
      // probability of ones (tmp[2*i+1]) over the probability of a zero
      // (tmp[2*i+0]).
      for(int i = 0; i < k; i++) {
        s[k-1-i] = (logf(tmp[2*i+1]) - logf(tmp[2*i+0]));
      }

      return s;
    }

    void
    constellation::set_soft_dec_lut(const std::vector< std::vector<float> > &soft_dec_lut,
                                    int precision)
    {
      max_min_axes();

      d_soft_dec_lut = soft_dec_lut;
      d_lut_precision = precision;
      d_lut_scale = powf(2.0, static_cast<float>(precision));
    }

    bool
    constellation::has_soft_dec_lut()
    {
      return d_soft_dec_lut.size() > 0;
    }

    std::vector< std::vector<float> >
    constellation::soft_dec_lut()
    {
      return d_soft_dec_lut;
    }

    std::vector<float>
    constellation::soft_decision_maker(gr_complex sample)
    {
      if(has_soft_dec_lut()) {
        // Clip to just below 1 --> at 1, we can overflow the index
        // that will put us in the next row of the 2D LUT.
        float xre = branchless_clip(sample.real(), 0.99);
        float xim = branchless_clip(sample.imag(), 0.99);

        // We normalize the constellation in the ctor, so we know that
        // the maximum dimensions go from -1 to +1. We can infer the x
        // and y scale directly.
        float scale = d_lut_scale / (2.0f);

        // Convert the clipped x and y samples to nearest index offset
        xre = floorf((1.0f + xre) * scale);
        xim = floorf((1.0f + xim) * scale);
        int index = static_cast<int>(d_lut_scale*xim + xre);

        int max_index = d_lut_scale*d_lut_scale;

        // Make sure we are in bounds of the index
        while(index >= max_index) {
          index -= d_lut_scale;
        }
        while(index < 0) {
          index += d_lut_scale;
        }

        return d_soft_dec_lut[index];
      }
      else {
        return calc_soft_dec(sample);
      }
    }

    void
    constellation::max_min_axes()
    {
      // Find min/max of constellation for both real and imag axes.
      d_re_min = 1e20;
      d_im_min = 1e20;
      d_re_max = -1e20;
      d_im_max = -1e20;
      for(size_t i = 0; i < d_constellation.size(); i++) {
        if(d_constellation[i].real() > d_re_max)
          d_re_max = d_constellation[i].real();
        if(d_constellation[i].imag() > d_im_max)
          d_im_max = d_constellation[i].imag();

        if(d_constellation[i].real() < d_re_min)
          d_re_min = d_constellation[i].real();
        if(d_constellation[i].imag() < d_im_min)
          d_im_min = d_constellation[i].imag();
      }
      if(d_im_min == 0)
        d_im_min = d_re_min;
      if(d_im_max == 0)
        d_im_max = d_re_max;
      if(d_re_min == 0)
        d_re_min = d_im_min;
      if(d_re_max == 0)
        d_re_max = d_im_max;
    }

    /********************************************************************/


    constellation_calcdist::sptr
    constellation_calcdist::make(std::vector<gr_complex> constell,
                                 std::vector<int> pre_diff_code,
                                 unsigned int rotational_symmetry,
                                 unsigned int dimensionality)
    {
      return constellation_calcdist::sptr(
        new constellation_calcdist(constell, pre_diff_code,
                                   rotational_symmetry, dimensionality));
    }

    constellation_calcdist::constellation_calcdist(
      std::vector<gr_complex> constell,
      std::vector<int> pre_diff_code,
      unsigned int rotational_symmetry,
      unsigned int dimensionality)
      : constellation(constell, pre_diff_code, rotational_symmetry, dimensionality)
    {}

    // Chooses points base on shortest distance.
    // Inefficient.
    unsigned int
    constellation_calcdist::decision_maker(const gr_complex *sample)
    {
      return get_closest_point(sample);
    }


    /********************************************************************/


    constellation_sector::constellation_sector(std::vector<gr_complex> constell,
                                               std::vector<int> pre_diff_code,
                                               unsigned int rotational_symmetry,
                                               unsigned int dimensionality,
                                               unsigned int n_sectors) :
      constellation(constell, pre_diff_code, rotational_symmetry,
                    dimensionality),
      n_sectors(n_sectors)
    {
    }

    constellation_sector::~constellation_sector()
    {
    }

    unsigned int
    constellation_sector::decision_maker(const gr_complex *sample)
    {
      unsigned int sector;
      sector = get_sector(sample);
      return sector_values[sector];
    }

    void
    constellation_sector::find_sector_values()
    {
      unsigned int i;
      sector_values.clear();
      for(i=0; i<n_sectors; i++) {
        sector_values.push_back(calc_sector_value(i));
      }
    }


    /********************************************************************/


    constellation_rect::sptr
    constellation_rect::make(std::vector<gr_complex> constell,
                             std::vector<int> pre_diff_code,
                             unsigned int rotational_symmetry,
                             unsigned int real_sectors,
                             unsigned int imag_sectors,
                             float width_real_sectors,
                             float width_imag_sectors)
    {
      return constellation_rect::sptr(new constellation_rect
                                      (constell, pre_diff_code,
                                       rotational_symmetry,
                                       real_sectors, imag_sectors,
                                       width_real_sectors,
                                       width_imag_sectors));
    }

    constellation_rect::constellation_rect(
      std::vector<gr_complex> constell,
      std::vector<int> pre_diff_code,
      unsigned int rotational_symmetry,
      unsigned int real_sectors, unsigned int imag_sectors,
      float width_real_sectors, float width_imag_sectors) :
      constellation_sector(constell, pre_diff_code, rotational_symmetry,
                           1, real_sectors * imag_sectors),
      n_real_sectors(real_sectors), n_imag_sectors(imag_sectors),
      d_width_real_sectors(width_real_sectors),
      d_width_imag_sectors(width_imag_sectors)
    {
      d_width_real_sectors *= d_scalefactor;
      d_width_imag_sectors *= d_scalefactor;
      find_sector_values();
    }

    constellation_rect::~constellation_rect()
    {
    }

    unsigned int
    constellation_rect::get_sector(const gr_complex *sample)
    {
      int real_sector, imag_sector;
      unsigned int sector;

      real_sector = int(real(*sample)/d_width_real_sectors
                        + n_real_sectors/2.0);
      if(real_sector < 0)
        real_sector = 0;
      if(real_sector >= (int)n_real_sectors)
        real_sector = n_real_sectors-1;

      imag_sector = int(imag(*sample)/d_width_imag_sectors
                        + n_imag_sectors/2.0);
      if(imag_sector < 0)
        imag_sector = 0;
      if(imag_sector >= (int)n_imag_sectors)
        imag_sector = n_imag_sectors-1;

      sector = real_sector * n_imag_sectors + imag_sector;
      return sector;
    }

    gr_complex
    constellation_rect::calc_sector_center(unsigned int sector)
    {
      unsigned int real_sector, imag_sector;
      gr_complex sector_center;
      real_sector = float(sector)/n_imag_sectors;
      imag_sector = sector - real_sector * n_imag_sectors;
      sector_center = gr_complex(
        (real_sector + 0.5 - n_real_sectors/2.0) * d_width_real_sectors,
        (imag_sector + 0.5 - n_imag_sectors/2.0) * d_width_imag_sectors);
      return sector_center;
    }

    unsigned int
    constellation_rect::calc_sector_value(unsigned int sector)
    {
      gr_complex sector_center = calc_sector_center(sector);
      unsigned int closest_point;
      closest_point = get_closest_point(&sector_center);
      return closest_point;
    }

    /********************************************************************/

    constellation_expl_rect::sptr
    constellation_expl_rect::make(std::vector<gr_complex> constellation,
                                  std::vector<int> pre_diff_code,
                                  unsigned int rotational_symmetry,
                                  unsigned int real_sectors,
                                  unsigned int imag_sectors,
                                  float width_real_sectors,
                                  float width_imag_sectors,
                                  std::vector<unsigned int> sector_values)
    {
      return constellation_expl_rect::sptr
        (new constellation_expl_rect(constellation, pre_diff_code,
                                     rotational_symmetry,
                                     real_sectors, imag_sectors,
                                     width_real_sectors, width_imag_sectors,
                                     sector_values));
    }

    constellation_expl_rect::constellation_expl_rect(
      std::vector<gr_complex> constellation,
      std::vector<int> pre_diff_code,
      unsigned int rotational_symmetry,
      unsigned int real_sectors,
      unsigned int imag_sectors,
      float width_real_sectors,
      float width_imag_sectors,
      std::vector<unsigned int> sector_values)
      : constellation_rect(constellation, pre_diff_code, rotational_symmetry,
                           real_sectors, imag_sectors, width_real_sectors, width_imag_sectors),
        d_sector_values(sector_values)
    {
    }

    constellation_expl_rect::~constellation_expl_rect()
    {
    }

    /********************************************************************/


    constellation_psk::sptr
    constellation_psk::make(std::vector<gr_complex> constell,
                            std::vector<int> pre_diff_code,
                            unsigned int n_sectors)
    {
      return constellation_psk::sptr(new constellation_psk
                                     (constell, pre_diff_code,
                                      n_sectors));
    }

    constellation_psk::constellation_psk(std::vector<gr_complex> constell,
                                         std::vector<int> pre_diff_code,
                                         unsigned int n_sectors) :
      constellation_sector(constell, pre_diff_code, constell.size(),
                           1, n_sectors)
    {
      find_sector_values();
    }

    constellation_psk::~constellation_psk()
    {
    }

    unsigned int
    constellation_psk::get_sector(const gr_complex *sample)
    {
      float phase = arg(*sample);
      float width = M_TWOPI / n_sectors;
      int sector = floor(phase/width + 0.5);
      if(sector < 0)
        sector += n_sectors;
      return sector;
    }

    unsigned int
    constellation_psk::calc_sector_value(unsigned int sector)
    {
      float phase = sector * M_TWOPI / n_sectors;
      gr_complex sector_center = gr_complex(cos(phase), sin(phase));
      unsigned int closest_point = get_closest_point(&sector_center);
      return closest_point;
    }


    /********************************************************************/


    constellation_bpsk::sptr
    constellation_bpsk::make()
    {
      return constellation_bpsk::sptr(new constellation_bpsk());
    }

    constellation_bpsk::constellation_bpsk()
    {
      d_constellation.resize(2);
      d_constellation[0] = gr_complex(-1, 0);
      d_constellation[1] = gr_complex(1, 0);
      d_rotational_symmetry = 2;
      d_dimensionality = 1;
      calc_arity();
    }

    constellation_bpsk::~constellation_bpsk()
    {
    }

    unsigned int
    constellation_bpsk::decision_maker(const gr_complex *sample)
    {
      return (real(*sample) > 0);
    }


    /********************************************************************/


    constellation_qpsk::sptr
    constellation_qpsk::make()
    {
      return constellation_qpsk::sptr(new constellation_qpsk());
    }

    constellation_qpsk::constellation_qpsk()
    {
      d_constellation.resize(4);
      // Gray-coded
      d_constellation[0] = gr_complex(-SQRT_TWO, -SQRT_TWO);
      d_constellation[1] = gr_complex(SQRT_TWO, -SQRT_TWO);
      d_constellation[2] = gr_complex(-SQRT_TWO, SQRT_TWO);
      d_constellation[3] = gr_complex(SQRT_TWO, SQRT_TWO);

      /*
        d_constellation[0] = gr_complex(SQRT_TWO, SQRT_TWO);
        d_constellation[1] = gr_complex(-SQRT_TWO, SQRT_TWO);
        d_constellation[2] = gr_complex(SQRT_TWO, -SQRT_TWO);
        d_constellation[3] = gr_complex(SQRT_TWO, -SQRT_TWO);
      */

      d_pre_diff_code.resize(4);
      d_pre_diff_code[0] = 0x0;
      d_pre_diff_code[1] = 0x2;
      d_pre_diff_code[2] = 0x3;
      d_pre_diff_code[3] = 0x1;

      d_rotational_symmetry = 4;
      d_dimensionality = 1;
      calc_arity();
    }

    constellation_qpsk::~constellation_qpsk()
    {
    }

    unsigned int
    constellation_qpsk::decision_maker(const gr_complex *sample)
    {
      // Real component determines small bit.
      // Imag component determines big bit.
      return 2*(imag(*sample)>0) + (real(*sample)>0);

      /*
        bool a = real(*sample) > 0;
        bool b = imag(*sample) > 0;
        if(a) {
        if(b)
        return 0x0;
        else
        return 0x1;
        }
        else {
        if(b)
        return 0x2;
        else
        return 0x3;
        }
      */
    }


    /********************************************************************/


    constellation_dqpsk::sptr
    constellation_dqpsk::make()
    {
      return constellation_dqpsk::sptr(new constellation_dqpsk());
    }

    constellation_dqpsk::constellation_dqpsk()
    {
      // This constellation is not gray coded, which allows
      // us to use differential encodings (through diff_encode and
      // diff_decode) on the symbols.
      d_constellation.resize(4);
      d_constellation[0] = gr_complex(+SQRT_TWO, +SQRT_TWO);
      d_constellation[1] = gr_complex(-SQRT_TWO, +SQRT_TWO);
      d_constellation[2] = gr_complex(-SQRT_TWO, -SQRT_TWO);
      d_constellation[3] = gr_complex(+SQRT_TWO, -SQRT_TWO);

      // Use this mapping to convert to gray code before diff enc.
      d_pre_diff_code.resize(4);
      d_pre_diff_code[0] = 0x0;
      d_pre_diff_code[1] = 0x1;
      d_pre_diff_code[2] = 0x3;
      d_pre_diff_code[3] = 0x2;
      d_apply_pre_diff_code = true;

      d_rotational_symmetry = 4;
      d_dimensionality = 1;
      calc_arity();
    }

    constellation_dqpsk::~constellation_dqpsk()
    {
    }

    unsigned int
    constellation_dqpsk::decision_maker(const gr_complex *sample)
    {
      // Slower deicison maker as we can't slice along one axis.
      // Maybe there's a better way to do this, still.

      bool a = real(*sample) > 0;
      bool b = imag(*sample) > 0;
      if(a) {
        if(b)
          return 0x0;
        else
          return 0x3;
      }
      else {
        if(b)
          return 0x1;
        else
          return 0x2;
      }
    }


    /********************************************************************/


    constellation_8psk::sptr
    constellation_8psk::make()
    {
      return constellation_8psk::sptr(new constellation_8psk());
    }

    constellation_8psk::constellation_8psk()
    {
      float angle = M_PI/8.0;
      d_constellation.resize(8);
      // Gray-coded
      d_constellation[0] = gr_complex(cos( 1*angle), sin( 1*angle));
      d_constellation[1] = gr_complex(cos( 7*angle), sin( 7*angle));
      d_constellation[2] = gr_complex(cos(15*angle), sin(15*angle));
      d_constellation[3] = gr_complex(cos( 9*angle), sin( 9*angle));
      d_constellation[4] = gr_complex(cos( 3*angle), sin( 3*angle));
      d_constellation[5] = gr_complex(cos( 5*angle), sin( 5*angle));
      d_constellation[6] = gr_complex(cos(13*angle), sin(13*angle));
      d_constellation[7] = gr_complex(cos(11*angle), sin(11*angle));
      d_rotational_symmetry = 8;
      d_dimensionality = 1;
      calc_arity();
    }

    constellation_8psk::~constellation_8psk()
    {
    }

    unsigned int
    constellation_8psk::decision_maker(const gr_complex *sample)
    {
      unsigned int ret = 0;

      float re = sample->real();
      float im = sample->imag();

      if(fabsf(re) <= fabsf(im))
        ret  = 4;
      if(re <= 0)
        ret |= 1;
      if(im <= 0)
        ret |= 2;

      return ret;
    }


    /********************************************************************/


    constellation_8psk_natural::sptr 
    constellation_8psk_natural::make()
    {
      return constellation_8psk_natural::sptr(new constellation_8psk_natural());
    }

    constellation_8psk_natural::constellation_8psk_natural()
    {
      float angle = M_PI/8.0;
      d_constellation.resize(8);
      // Natural-mapping
      d_constellation[0] = gr_complex(cos( 15*angle), sin( 15*angle));
      d_constellation[1] = gr_complex(cos( 1*angle), sin( 1*angle));
      d_constellation[2] = gr_complex(cos(3*angle), sin(3*angle));
      d_constellation[3] = gr_complex(cos( 5*angle), sin( 5*angle));
      d_constellation[4] = gr_complex(cos( 7*angle), sin( 7*angle));
      d_constellation[5] = gr_complex(cos( 9*angle), sin( 9*angle));
      d_constellation[6] = gr_complex(cos(11*angle), sin(11*angle));
      d_constellation[7] = gr_complex(cos(13*angle), sin(13*angle));
      d_rotational_symmetry = 8;
      d_dimensionality = 1;
      calc_arity();
    }

    constellation_8psk_natural::~constellation_8psk_natural()
    {
    }

    unsigned int
    constellation_8psk_natural::decision_maker(const gr_complex *sample)
    {
      unsigned int ret = 0;

      float re = sample->real();
      float im = sample->imag();

      if((re+im) < 0)
        ret  = 4;
      if(fabsf(im) > fabsf(re)){
        ret |= 2;
	if(re*im < 0)
          ret |= 1;
	}
      if(fabsf(im) < fabsf(re) && re*im > 0)
        ret |= 1;

      return ret;
    }


    /********************************************************************/


    constellation_16qam::sptr 
    constellation_16qam::make()
    {
      return constellation_16qam::sptr(new constellation_16qam());
    }

    constellation_16qam::constellation_16qam()
    {
      const float level = sqrt(float(0.1));
      d_constellation.resize(16);
      // The mapping used in 16qam set partition
      d_constellation[0] = gr_complex(1*level,-1*level);
      d_constellation[1] = gr_complex(-1*level,-1*level);
      d_constellation[2] = gr_complex(3*level,-3*level);
      d_constellation[3] = gr_complex(-3*level,-3*level);
      d_constellation[4] = gr_complex(-3*level,-1*level);
      d_constellation[5] = gr_complex(3*level,-1*level);
      d_constellation[6] = gr_complex(-1*level,-3*level);
      d_constellation[7] = gr_complex(1*level,-3*level);
      d_constellation[8] = gr_complex(-3*level,3*level);
      d_constellation[9] = gr_complex(3*level,3*level);
      d_constellation[10] = gr_complex(-1*level,1*level);
      d_constellation[11] = gr_complex(1*level,1*level);
      d_constellation[12] = gr_complex(1*level,3*level);
      d_constellation[13] = gr_complex(-1*level,3*level);
      d_constellation[14] = gr_complex(3*level,1*level);
      d_constellation[15] = gr_complex(-3*level,1*level);
      d_rotational_symmetry = 4;
      d_dimensionality = 1;
      calc_arity();
    }

    constellation_16qam::~constellation_16qam()
    {
    }

    unsigned int
    constellation_16qam::decision_maker(const gr_complex *sample)
    {
      unsigned int ret = 0;
      const float level = sqrt(float(0.1));
      float re = sample->real();
      float im = sample->imag();

      if(im <= 0 && im >= -2*level && re >= 0 && re <= 2*level)
	ret = 0;
      else if(im <= 0 && im >= -2*level && re <= 0 && re >= -2*level)
	ret = 1;
      else if(im <= -2*level && re >= 2*level)
	ret = 2;
      else if(im <= -2*level && re <= -2*level)
	ret = 3;
      else if(im <= 0 && im >= -2*level && re <= -2*level)
	ret = 4;
      else if(im <= 0 && im >= -2*level && re >= 2*level)
	ret = 5;
      else if(im <= -2*level && re <= 0 && re >= -2*level)
	ret = 6;
      else if(im <= -2*level && re >= 0 && re <= 2*level)
	ret = 7;
      else if(im >= 2*level && re <= -2*level)
	ret = 8;
      else if(im >= 2*level && re >= 2*level)
	ret = 9;
      else if(im >= 0 && im <= 2*level && re <= 0 && re >= -2*level)
	ret = 10;
      else if(im >= 0 && im <= 2*level && re >= 0 && re <= 2*level)
	ret = 11;
      else if(im >= 2*level && re >= 0 && re <= 2*level)
	ret = 12;
      else if(im >= 2*level && re <= 0 && re >= -2*level)
	ret = 13;
      else if(im >= 0 && im <= 2*level && re >= 2*level)
	ret = 14;
      else if(im >= 0 && im <= 2*level && re <= -2*level)
	ret = 15;

      return ret;
    }


  } /* namespace digital */
} /* namespace gr */
