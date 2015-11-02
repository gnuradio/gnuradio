/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dvbt2_miso_cc_impl.h"

namespace gr {
  namespace dtv {

    dvbt2_miso_cc::sptr
    dvbt2_miso_cc::make(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode)
    {
      return gnuradio::get_initial_sptr
        (new dvbt2_miso_cc_impl(carriermode, fftsize, pilotpattern, guardinterval, numdatasyms, paprmode));
    }

    /*
     * The private constructor
     */
    dvbt2_miso_cc_impl::dvbt2_miso_cc_impl(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode)
      : gr::sync_block("dvbt2_miso_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(2, 2, sizeof(gr_complex)))
    {
      switch (fftsize) {
        case FFTSIZE_1K:
          N_P2 = 16;
          C_P2 = 546;
          break;
        case FFTSIZE_2K:
          N_P2 = 8;
          C_P2 = 1098;
          break;
        case FFTSIZE_4K:
          N_P2 = 4;
          C_P2 = 2198;
          break;
        case FFTSIZE_8K:
        case FFTSIZE_8K_T2GI:
          N_P2 = 2;
          C_P2 = 4398;
          break;
        case FFTSIZE_16K:
        case FFTSIZE_16K_T2GI:
          N_P2 = 1;
          C_P2 = 8814;
          break;
        case FFTSIZE_32K:
        case FFTSIZE_32K_T2GI:
          N_P2 = 1;
          C_P2 = 17612;
          break;
      }
      switch (fftsize) {
        case FFTSIZE_1K:
          switch (pilotpattern) {
            case PILOT_PP1:
              C_DATA = 764;
              N_FC = 568;
              C_FC = 402;
              break;
            case PILOT_PP2:
              C_DATA = 768;
              N_FC = 710;
              C_FC = 654;
              break;
            case PILOT_PP3:
              C_DATA = 798;
              N_FC = 710;
              C_FC = 490;
              break;
            case PILOT_PP4:
              C_DATA = 804;
              N_FC = 780;
              C_FC = 707;
              break;
            case PILOT_PP5:
              C_DATA = 818;
              N_FC = 780;
              C_FC = 544;
              break;
            case PILOT_PP6:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
            case PILOT_PP7:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
            case PILOT_PP8:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 10;
            }
            if (N_FC != 0) {
              N_FC -= 10;
            }
            if (C_FC != 0) {
              C_FC -= 10;
            }
          }
          break;
        case FFTSIZE_2K:
          switch (pilotpattern) {
            case PILOT_PP1:
              C_DATA = 1522;
              N_FC = 1136;
              C_FC = 804;
              break;
            case PILOT_PP2:
              C_DATA = 1532;
              N_FC = 1420;
              C_FC = 1309;
              break;
            case PILOT_PP3:
              C_DATA = 1596;
              N_FC = 1420;
              C_FC = 980;
              break;
            case PILOT_PP4:
              C_DATA = 1602;
              N_FC = 1562;
              C_FC = 1415;
              break;
            case PILOT_PP5:
              C_DATA = 1632;
              N_FC = 1562;
              C_FC = 1088;
              break;
            case PILOT_PP6:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
            case PILOT_PP7:
              C_DATA = 1646;
              N_FC = 1632;
              C_FC = 1396;
              break;
            case PILOT_PP8:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 18;
            }
            if (N_FC != 0) {
              N_FC -= 18;
            }
            if (C_FC != 0) {
              C_FC -= 18;
            }
          }
          break;
        case FFTSIZE_4K:
          switch (pilotpattern) {
            case PILOT_PP1:
              C_DATA = 3084;
              N_FC = 2272;
              C_FC = 1609;
              break;
            case PILOT_PP2:
              C_DATA = 3092;
              N_FC = 2840;
              C_FC = 2619;
              break;
            case PILOT_PP3:
              C_DATA = 3228;
              N_FC = 2840;
              C_FC = 1961;
              break;
            case PILOT_PP4:
              C_DATA = 3234;
              N_FC = 3124;
              C_FC = 2831;
              break;
            case PILOT_PP5:
              C_DATA = 3298;
              N_FC = 3124;
              C_FC = 2177;
              break;
            case PILOT_PP6:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
            case PILOT_PP7:
              C_DATA = 3328;
              N_FC = 3266;
              C_FC = 2792;
              break;
            case PILOT_PP8:
              C_DATA = 0;
              N_FC = 0;
              C_FC = 0;
              break;
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 36;
            }
            if (N_FC != 0) {
              N_FC -= 36;
            }
            if (C_FC != 0) {
              C_FC -= 36;
            }
          }
          break;
        case FFTSIZE_8K:
        case FFTSIZE_8K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 6208;
                N_FC = 4544;
                C_FC = 3218;
                break;
              case PILOT_PP2:
                C_DATA = 6214;
                N_FC = 5680;
                C_FC = 5238;
                break;
              case PILOT_PP3:
                C_DATA = 6494;
                N_FC = 5680;
                C_FC = 3922;
                break;
              case PILOT_PP4:
                C_DATA = 6498;
                N_FC = 6248;
                C_FC = 5662;
                break;
              case PILOT_PP5:
                C_DATA = 6634;
                N_FC = 6248;
                C_FC = 4354;
                break;
              case PILOT_PP6:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP7:
                C_DATA = 6698;
                N_FC = 6532;
                C_FC = 5585;
                break;
              case PILOT_PP8:
                C_DATA = 6698;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          else {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 6296;
                N_FC = 4608;
                C_FC = 3264;
                break;
              case PILOT_PP2:
                C_DATA = 6298;
                N_FC = 5760;
                C_FC = 5312;
                break;
              case PILOT_PP3:
                C_DATA = 6584;
                N_FC = 5760;
                C_FC = 3978;
                break;
              case PILOT_PP4:
                C_DATA = 6588;
                N_FC = 6336;
                C_FC = 5742;
                break;
              case PILOT_PP5:
                C_DATA = 6728;
                N_FC = 6336;
                C_FC = 4416;
                break;
              case PILOT_PP6:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP7:
                C_DATA = 6788;
                N_FC = 6624;
                C_FC = 5664;
                break;
              case PILOT_PP8:
                C_DATA = 6788;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 72;
            }
            if (N_FC != 0) {
              N_FC -= 72;
            }
            if (C_FC != 0) {
              C_FC -= 72;
            }
          }
          break;
        case FFTSIZE_16K:
        case FFTSIZE_16K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 12418;
                N_FC = 9088;
                C_FC = 6437;
                break;
              case PILOT_PP2:
                C_DATA = 12436;
                N_FC = 11360;
                C_FC = 10476;
                break;
              case PILOT_PP3:
                C_DATA = 12988;
                N_FC = 11360;
                C_FC = 7845;
                break;
              case PILOT_PP4:
                C_DATA = 13002;
                N_FC = 12496;
                C_FC = 11324;
                break;
              case PILOT_PP5:
                C_DATA = 13272;
                N_FC = 12496;
                C_FC = 8709;
                break;
              case PILOT_PP6:
                C_DATA = 13288;
                N_FC = 13064;
                C_FC = 11801;
                break;
              case PILOT_PP7:
                C_DATA = 13416;
                N_FC = 13064;
                C_FC = 11170;
                break;
              case PILOT_PP8:
                C_DATA = 13406;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          else {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 12678;
                N_FC = 9280;
                C_FC = 6573;
                break;
              case PILOT_PP2:
                C_DATA = 12698;
                N_FC = 11600;
                C_FC = 10697;
                break;
              case PILOT_PP3:
                C_DATA = 13262;
                N_FC = 11600;
                C_FC = 8011;
                break;
              case PILOT_PP4:
                C_DATA = 13276;
                N_FC = 12760;
                C_FC = 11563;
                break;
              case PILOT_PP5:
                C_DATA = 13552;
                N_FC = 12760;
                C_FC = 8893;
                break;
              case PILOT_PP6:
                C_DATA = 13568;
                N_FC = 13340;
                C_FC = 12051;
                break;
              case PILOT_PP7:
                C_DATA = 13698;
                N_FC = 13340;
                C_FC = 11406;
                break;
              case PILOT_PP8:
                C_DATA = 13688;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 144;
            }
            if (N_FC != 0) {
              N_FC -= 144;
            }
            if (C_FC != 0) {
              C_FC -= 144;
            }
          }
          break;
        case FFTSIZE_32K:
        case FFTSIZE_32K_T2GI:
          if (carriermode == CARRIERS_NORMAL) {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP2:
                C_DATA = 24886;
                N_FC = 22720;
                C_FC = 20952;
                break;
              case PILOT_PP3:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP4:
                C_DATA = 26022;
                N_FC = 24992;
                C_FC = 22649;
                break;
              case PILOT_PP5:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP6:
                C_DATA = 26592;
                N_FC = 26128;
                C_FC = 23603;
                break;
              case PILOT_PP7:
                C_DATA = 26836;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP8:
                C_DATA = 26812;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          else {
            switch (pilotpattern) {
              case PILOT_PP1:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP2:
                C_DATA = 25412;
                N_FC = 23200;
                C_FC = 21395;
                break;
              case PILOT_PP3:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP4:
                C_DATA = 26572;
                N_FC = 25520;
                C_FC = 23127;
                break;
              case PILOT_PP5:
                C_DATA = 0;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP6:
                C_DATA = 27152;
                N_FC = 26680;
                C_FC = 24102;
                break;
              case PILOT_PP7:
                C_DATA = 27404;
                N_FC = 0;
                C_FC = 0;
                break;
              case PILOT_PP8:
                C_DATA = 27376;
                N_FC = 0;
                C_FC = 0;
                break;
            }
          }
          if (paprmode == PAPR_TR || paprmode == PAPR_BOTH) {
            if (C_DATA != 0) {
              C_DATA -= 288;
            }
            if (N_FC != 0) {
              N_FC -= 288;
            }
            if (C_FC != 0) {
              C_FC -= 288;
            }
          }
          break;
      }
      if (N_FC == 0) {
        set_output_multiple((N_P2 * C_P2) + (numdatasyms * C_DATA));
        miso_items = (N_P2 * C_P2) + (numdatasyms * C_DATA);
      }
      else {
        set_output_multiple((N_P2 * C_P2) + ((numdatasyms - 1) * C_DATA) + N_FC);
        miso_items = (N_P2 * C_P2) + ((numdatasyms - 1) * C_DATA) + N_FC;
      }
    }

    /*
     * Our virtual destructor.
     */
    dvbt2_miso_cc_impl::~dvbt2_miso_cc_impl()
    {
    }

    int
    dvbt2_miso_cc_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out1 = (gr_complex *) output_items[0];
      gr_complex *out2 = (gr_complex *) output_items[1];
      gr_complex temp1, temp2;

      for (int i = 0; i < noutput_items; i += miso_items) {
        memcpy(out1, in, sizeof(gr_complex) * miso_items);
        out1 += miso_items;
        for (int j = 0; j < miso_items; j += 2) {
          temp1 = std::conj(*in++);
          temp2 = std::conj(*in++);
          *out2++ = -temp2;
          *out2++ =  temp1;
        }
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

