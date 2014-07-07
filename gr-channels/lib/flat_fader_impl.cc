/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#include <flat_fader_impl.h>

namespace gr {
  namespace channels {

    flat_fader_impl::flat_fader_impl(unsigned int N, float fDTs, bool LOS, float K, int seed ) :
        seed_1((int)seed),
        dist_1(-M_PI, M_PI),
        rv_1( seed_1, dist_1 ), // U(-pi,pi)

        seed_2((int)seed+1),
        dist_2(0, 1),
        rv_2( seed_2, dist_2 ), // U(0,1)

        d_N(N),
        d_fDTs(fDTs),
        d_theta(rv_1()),
        d_theta_los(rv_1()),
        d_step( powf(0.00125*fDTs, 1.1) ),  // max step size approximated from Table 2
        d_m(0),
        d_K(K),
        d_LOS(LOS),

        d_psi(d_N+1, 0),
        d_phi(d_N+1, 0),

        d_table(8*1024),

        scale_sin(sqrtf(2.0/d_N)),
        scale_los(sqrtf(d_K)/sqrtf(d_K+1)),
        scale_nlos(1/sqrtf(d_K+1))
    {
        // generate initial phase values
        for(int i=0; i<d_N+1; i++){
          d_psi[i] = rv_1();
          d_phi[i] = rv_1();
        }
    }


    gr_complex flat_fader_impl::next_sample(){
        gr_complex H(0,0);

        for(int n=1; n<d_N; n++){
	  float alpha_n = (2*M_PI*n - M_PI + d_theta)/(4*d_N);
#if FASTSINCOS == 1
            float s_i = scale_sin*gr::fxpt::cos(gr::fxpt::float_to_fixed(2*M_PI*d_fDTs*d_m*gr::fxpt::cos(gr::fxpt::float_to_fixed(alpha_n))+d_psi[n+1]));
            float s_q = scale_sin*gr::fxpt::cos(gr::fxpt::float_to_fixed(2*M_PI*d_fDTs*d_m*gr::fxpt::sin(gr::fxpt::float_to_fixed(alpha_n))+d_phi[n+1]));
#elif FASTSINCOS == 2
            float s_i = scale_sin*d_table.cos(2*M_PI*d_fDTs*d_m*d_table.cos(alpha_n)+d_psi[n+1]);
            float s_q = scale_sin*d_table.cos(2*M_PI*d_fDTs*d_m*d_table.sin(alpha_n)+d_phi[n+1]);

#else
            float s_i = scale_sin*cos(2*M_PI*d_fDTs*d_m*cos(alpha_n)+d_psi[n+1]);
            float s_q = scale_sin*cos(2*M_PI*d_fDTs*d_m*sin(alpha_n)+d_phi[n+1]);
#endif

            H += gr_complex(s_i, s_q);
        }

        if(d_LOS){
#if FASTSINCOS == 1
            float los_i = gr::fxpt::cos(gr::fxpt::float_to_fixed(2*M_PI*d_fDTs*d_m*gr::fxpt::cos(gr::fxpt::float_to_fixed(d_theta_los)) + d_psi[0]));
            float los_q = gr::fxpt::sin(gr::fxpt::float_to_fixed(2*M_PI*d_fDTs*d_m*gr::fxpt::cos(gr::fxpt::float_to_fixed(d_theta_los)) + d_psi[0]));
#elif FASTSINCOS == 2
            float los_i = d_table.cos(2*M_PI*d_fDTs*d_m*d_table.cos(d_theta_los) + d_psi[0]);
            float los_q = d_table.sin(2*M_PI*d_fDTs*d_m*d_table.cos(d_theta_los) + d_psi[0]);
#else
            float los_i = cos(2*M_PI*d_fDTs*d_m*cos(d_theta_los) + d_psi[0]);
            float los_q = sin(2*M_PI*d_fDTs*d_m*cos(d_theta_los) + d_psi[0]);
#endif

            H = H*scale_nlos + gr_complex(los_i,los_q)*scale_los;
            }

        //out[i] = in[i]*H;
        d_m++;
        update_theta();
        return H;

    }

    void flat_fader_impl::update_theta()
    {
        d_theta += (d_step*rv_2());
        if(d_theta > M_PI){
            d_theta = M_PI; d_step = -d_step;
        } else if(d_theta < -M_PI){
            d_theta = -M_PI; d_step = -d_step;
        }
    }

  } /* namespace channels */
} /* namespace gr */

