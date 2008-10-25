/* -*- c++ -*- */
/*
 * Copyright 2002,2007,2008 Free Software Foundation, Inc.
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

#include <gr_firdes.h>
#include <stdexcept>


using std::vector;

#define IzeroEPSILON 1E-21               /* Max error acceptable in Izero */

static double Izero(double x)
{
   double sum, u, halfx, temp;
   int n;

   sum = u = n = 1;
   halfx = x/2.0;
   do {
      temp = halfx/(double)n;
      n += 1;
      temp *= temp;
      u *= temp;
      sum += u;
      } while (u >= IzeroEPSILON*sum);
   return(sum);
}


//
//	=== Low Pass ===
//

vector<float>
gr_firdes::low_pass_2(double gain,
		      double sampling_freq,    // Hz
		      double cutoff_freq,      // Hz BEGINNING of transition band
		      double transition_width, // Hz width of transition band
		      double attenuation_dB,   // attenuation dB
		      win_type window_type,
		      double beta)             // used only with Kaiser
{
  sanity_check_1f (sampling_freq, cutoff_freq, transition_width);

  int ntaps = compute_ntaps_windes (sampling_freq, transition_width,
				    attenuation_dB);

  // construct the truncated ideal impulse response
  // [sin(x)/x for the low pass case]

  vector<float> taps(ntaps);
  vector<float> w = window (window_type, ntaps, beta);

  int M = (ntaps - 1) / 2;
  double fwT0 = 2 * M_PI * cutoff_freq / sampling_freq;
  for (int n = -M; n <= M; n++){
    if (n == 0)
      taps[n + M] = fwT0 / M_PI * w[n + M];
    else {
      // a little algebra gets this into the more familiar sin(x)/x form
      taps[n + M] =  sin (n * fwT0) / (n * M_PI) * w[n + M];
    }
  }
  
  // find the factor to normalize the gain, fmax.
  // For low-pass, gain @ zero freq = 1.0
  
  double fmax = taps[0 + M];
  for (int n = 1; n <= M; n++)
    fmax += 2 * taps[n + M];

  gain /= fmax;	// normalize

  for (int i = 0; i < ntaps; i++)
    taps[i] *= gain;


  return taps;
}

vector<float>
gr_firdes::low_pass (double gain,
		     double sampling_freq,
		     double cutoff_freq,	// Hz center of transition band
		     double transition_width,	// Hz width of transition band
		     win_type window_type,
		     double beta)		// used only with Kaiser
{
  sanity_check_1f (sampling_freq, cutoff_freq, transition_width);

  int ntaps = compute_ntaps (sampling_freq, transition_width,
			     window_type, beta);

  // construct the truncated ideal impulse response
  // [sin(x)/x for the low pass case]

  vector<float> taps(ntaps);
  vector<float> w = window (window_type, ntaps, beta);

  int M = (ntaps - 1) / 2;
  double fwT0 = 2 * M_PI * cutoff_freq / sampling_freq;

  for (int n = -M; n <= M; n++){
    if (n == 0)
      taps[n + M] = fwT0 / M_PI * w[n + M];
    else {
      // a little algebra gets this into the more familiar sin(x)/x form
      taps[n + M] =  sin (n * fwT0) / (n * M_PI) * w[n + M];
    }
  }
  
  // find the factor to normalize the gain, fmax.
  // For low-pass, gain @ zero freq = 1.0
  
  double fmax = taps[0 + M];
  for (int n = 1; n <= M; n++)
    fmax += 2 * taps[n + M];

  gain /= fmax;	// normalize

  for (int i = 0; i < ntaps; i++)
    taps[i] *= gain;

  return taps;
}


//
//	=== High Pass ===
//

vector<float>
gr_firdes::high_pass_2 (double gain,
                      double sampling_freq,
                      double cutoff_freq,       // Hz center of transition band
                      double transition_width,  // Hz width of transition band
		      double attenuation_dB,   // attenuation dB
                      win_type window_type,
                      double beta)              // used only with Kaiser
{
  sanity_check_1f (sampling_freq, cutoff_freq, transition_width);

  int ntaps = compute_ntaps_windes (sampling_freq, transition_width,
				    attenuation_dB);

  // construct the truncated ideal impulse response times the window function

  vector<float> taps(ntaps);
  vector<float> w = window (window_type, ntaps, beta);

  int M = (ntaps - 1) / 2;
  double fwT0 = 2 * M_PI * cutoff_freq / sampling_freq;

  for (int n = -M; n <= M; n++){
    if (n == 0)
      taps[n + M] = (1 - (fwT0 / M_PI)) * w[n + M];
    else {
      // a little algebra gets this into the more familiar sin(x)/x form
      taps[n + M] =  -sin (n * fwT0) / (n * M_PI) * w[n + M];
    }
  }

  // find the factor to normalize the gain, fmax.
  // For high-pass, gain @ fs/2 freq = 1.0

  double fmax = taps[0 + M];
  for (int n = 1; n <= M; n++)
    fmax += 2 * taps[n + M] * cos (n * M_PI);

  gain /= fmax; // normalize

  for (int i = 0; i < ntaps; i++)
    taps[i] *= gain;


  return taps;
}


vector<float>
gr_firdes::high_pass (double gain,
                      double sampling_freq,
                      double cutoff_freq,       // Hz center of transition band
                      double transition_width,  // Hz width of transition band
                      win_type window_type,
                      double beta)              // used only with Kaiser
{
  sanity_check_1f (sampling_freq, cutoff_freq, transition_width);

  int ntaps = compute_ntaps (sampling_freq, transition_width,
                             window_type, beta);

  // construct the truncated ideal impulse response times the window function

  vector<float> taps(ntaps);
  vector<float> w = window (window_type, ntaps, beta);

  int M = (ntaps - 1) / 2;
  double fwT0 = 2 * M_PI * cutoff_freq / sampling_freq;

  for (int n = -M; n <= M; n++){
    if (n == 0)
      taps[n + M] = (1 - (fwT0 / M_PI)) * w[n + M];
    else {
      // a little algebra gets this into the more familiar sin(x)/x form
      taps[n + M] =  -sin (n * fwT0) / (n * M_PI) * w[n + M];
    }
  }

  // find the factor to normalize the gain, fmax.
  // For high-pass, gain @ fs/2 freq = 1.0

  double fmax = taps[0 + M];
  for (int n = 1; n <= M; n++)
    fmax += 2 * taps[n + M] * cos (n * M_PI);

  gain /= fmax; // normalize

  for (int i = 0; i < ntaps; i++)
    taps[i] *= gain;

  return taps;
}

//
//      === Band Pass ===
//

vector<float>
gr_firdes::band_pass_2 (double gain,
		      double sampling_freq,
		      double low_cutoff_freq,	// Hz center of transition band
		      double high_cutoff_freq,	// Hz center of transition band
		      double transition_width,	// Hz width of transition band
		      double attenuation_dB,   // attenuation dB
		      win_type window_type,
		      double beta)		// used only with Kaiser
{
  sanity_check_2f (sampling_freq,
		   low_cutoff_freq,
		   high_cutoff_freq, transition_width);

  int ntaps = compute_ntaps_windes (sampling_freq, transition_width,
				    attenuation_dB);

  vector<float> taps(ntaps);
  vector<float> w = window (window_type, ntaps, beta);

  int M = (ntaps - 1) / 2;
  double fwT0 = 2 * M_PI * low_cutoff_freq / sampling_freq;
  double fwT1 = 2 * M_PI * high_cutoff_freq / sampling_freq;

  for (int n = -M; n <= M; n++){
    if (n == 0)
      taps[n + M] = (fwT1 - fwT0) / M_PI * w[n + M];
    else {
      taps[n + M] =  (sin (n * fwT1) - sin (n * fwT0)) / (n * M_PI) * w[n + M];
    }
  }
  
  // find the factor to normalize the gain, fmax.
  // For band-pass, gain @ center freq = 1.0
  
  double fmax = taps[0 + M];
  for (int n = 1; n <= M; n++)
    fmax += 2 * taps[n + M] * cos (n * (fwT0 + fwT1) * 0.5);

  gain /= fmax;	// normalize

  for (int i = 0; i < ntaps; i++)
    taps[i] *= gain;

  return taps;
}


vector<float>
gr_firdes::band_pass (double gain,
		      double sampling_freq,
		      double low_cutoff_freq,	// Hz center of transition band
		      double high_cutoff_freq,	// Hz center of transition band
		      double transition_width,	// Hz width of transition band
		      win_type window_type,
		      double beta)		// used only with Kaiser
{
  sanity_check_2f (sampling_freq,
		   low_cutoff_freq,
		   high_cutoff_freq, transition_width);

  int ntaps = compute_ntaps (sampling_freq, transition_width,
			     window_type, beta);

  // construct the truncated ideal impulse response times the window function

  vector<float> taps(ntaps);
  vector<float> w = window (window_type, ntaps, beta);

  int M = (ntaps - 1) / 2;
  double fwT0 = 2 * M_PI * low_cutoff_freq / sampling_freq;
  double fwT1 = 2 * M_PI * high_cutoff_freq / sampling_freq;

  for (int n = -M; n <= M; n++){
    if (n == 0)
      taps[n + M] = (fwT1 - fwT0) / M_PI * w[n + M];
    else {
      taps[n + M] =  (sin (n * fwT1) - sin (n * fwT0)) / (n * M_PI) * w[n + M];
    }
  }
  
  // find the factor to normalize the gain, fmax.
  // For band-pass, gain @ center freq = 1.0
  
  double fmax = taps[0 + M];
  for (int n = 1; n <= M; n++)
    fmax += 2 * taps[n + M] * cos (n * (fwT0 + fwT1) * 0.5);

  gain /= fmax;	// normalize

  for (int i = 0; i < ntaps; i++)
    taps[i] *= gain;

  return taps;
}

//
//	=== Complex Band Pass ===
//

vector<gr_complex>
gr_firdes::complex_band_pass_2 (double gain,
		      double sampling_freq,
		      double low_cutoff_freq,	// Hz center of transition band
		      double high_cutoff_freq,	// Hz center of transition band
		      double transition_width,	// Hz width of transition band
		      double attenuation_dB,      // attenuation dB
		      win_type window_type,
		      double beta)		// used only with Kaiser
{
  sanity_check_2f_c (sampling_freq,
		   low_cutoff_freq,
		   high_cutoff_freq, transition_width);

  int ntaps = compute_ntaps_windes (sampling_freq, transition_width,
				    attenuation_dB);



  vector<gr_complex> taps(ntaps);
  vector<float> lptaps(ntaps);
  vector<float> w = window (window_type, ntaps, beta);

  lptaps = low_pass_2(gain,sampling_freq,(high_cutoff_freq - low_cutoff_freq)/2,transition_width,attenuation_dB,window_type,beta);

  gr_complex *optr = &taps[0];
  float *iptr = &lptaps[0];
  float freq = M_PI * (high_cutoff_freq + low_cutoff_freq)/sampling_freq;
  float phase=0;
  if (lptaps.size() & 01) {
      phase = - freq * ( lptaps.size() >> 1 );
  } else phase = - freq/2.0 * ((1 + 2*lptaps.size()) >> 1);
  for(unsigned int i=0;i<lptaps.size();i++) {
      *optr++ = gr_complex(*iptr * cos(phase),*iptr * sin(phase));
      iptr++, phase += freq;
  }
  
  return taps;
}


vector<gr_complex>
gr_firdes::complex_band_pass (double gain,
		      double sampling_freq,
		      double low_cutoff_freq,	// Hz center of transition band
		      double high_cutoff_freq,	// Hz center of transition band
		      double transition_width,	// Hz width of transition band
		      win_type window_type,
		      double beta)		// used only with Kaiser
{
  sanity_check_2f_c (sampling_freq,
		   low_cutoff_freq,
		   high_cutoff_freq, transition_width);

  int ntaps = compute_ntaps (sampling_freq, transition_width,
			     window_type, beta);

  // construct the truncated ideal impulse response times the window function

  vector<gr_complex> taps(ntaps);
  vector<float> lptaps(ntaps);
  vector<float> w = window (window_type, ntaps, beta);

  lptaps = low_pass(gain,sampling_freq,(high_cutoff_freq - low_cutoff_freq)/2,transition_width,window_type,beta);

  gr_complex *optr = &taps[0];
  float *iptr = &lptaps[0];
  float freq = M_PI * (high_cutoff_freq + low_cutoff_freq)/sampling_freq;
  float phase=0;
  if (lptaps.size() & 01) {
      phase = - freq * ( lptaps.size() >> 1 );
  } else phase = - freq/2.0 * ((1 + 2*lptaps.size()) >> 1);
  for(unsigned int i=0;i<lptaps.size();i++) {
      *optr++ = gr_complex(*iptr * cos(phase),*iptr * sin(phase));
      iptr++, phase += freq;
  }
  
  return taps;
}

//
//	=== Band Reject ===
//

vector<float>
gr_firdes::band_reject_2 (double gain,
  		        double sampling_freq,
		        double low_cutoff_freq,	 // Hz center of transition band
		        double high_cutoff_freq, // Hz center of transition band
		        double transition_width, // Hz width of transition band
		        double attenuation_dB,   // attenuation dB
		        win_type window_type,
		        double beta)		 // used only with Kaiser
{
  sanity_check_2f (sampling_freq,
		   low_cutoff_freq,
		   high_cutoff_freq, transition_width);

  int ntaps = compute_ntaps_windes (sampling_freq, transition_width,
				    attenuation_dB);

  // construct the truncated ideal impulse response times the window function

  vector<float> taps(ntaps);
  vector<float> w = window (window_type, ntaps, beta);

  int M = (ntaps - 1) / 2;
  double fwT0 = 2 * M_PI * low_cutoff_freq / sampling_freq;
  double fwT1 = 2 * M_PI * high_cutoff_freq / sampling_freq;

  for (int n = -M; n <= M; n++){
    if (n == 0)
      taps[n + M] = 1.0 + ((fwT0 - fwT1) / M_PI * w[n + M]);
    else {
      taps[n + M] =  (sin (n * fwT0) - sin (n * fwT1)) / (n * M_PI) * w[n + M];
    }
  }
  
  // find the factor to normalize the gain, fmax.
  // For band-reject, gain @ zero freq = 1.0
  
  double fmax = taps[0 + M];
  for (int n = 1; n <= M; n++)
    fmax += 2 * taps[n + M];

  gain /= fmax;	// normalize

  for (int i = 0; i < ntaps; i++)
    taps[i] *= gain;

  return taps;
}

vector<float>
gr_firdes::band_reject (double gain,
  		        double sampling_freq,
		        double low_cutoff_freq,	 // Hz center of transition band
		        double high_cutoff_freq, // Hz center of transition band
		        double transition_width, // Hz width of transition band
		        win_type window_type,
		        double beta)		 // used only with Kaiser
{
  sanity_check_2f (sampling_freq,
		   low_cutoff_freq,
		   high_cutoff_freq, transition_width);

  int ntaps = compute_ntaps (sampling_freq, transition_width,
			     window_type, beta);

  // construct the truncated ideal impulse response times the window function

  vector<float> taps(ntaps);
  vector<float> w = window (window_type, ntaps, beta);

  int M = (ntaps - 1) / 2;
  double fwT0 = 2 * M_PI * low_cutoff_freq / sampling_freq;
  double fwT1 = 2 * M_PI * high_cutoff_freq / sampling_freq;

  for (int n = -M; n <= M; n++){
    if (n == 0)
      taps[n + M] = 1.0 + ((fwT0 - fwT1) / M_PI * w[n + M]);
    else {
      taps[n + M] =  (sin (n * fwT0) - sin (n * fwT1)) / (n * M_PI) * w[n + M];
    }
  }
  
  // find the factor to normalize the gain, fmax.
  // For band-reject, gain @ zero freq = 1.0
  
  double fmax = taps[0 + M];
  for (int n = 1; n <= M; n++)
    fmax += 2 * taps[n + M];

  gain /= fmax;	// normalize

  for (int i = 0; i < ntaps; i++)
    taps[i] *= gain;

  return taps;
}

//
// Hilbert Transform
//

vector<float>
gr_firdes::hilbert (unsigned int ntaps,
		    win_type windowtype, 
		    double beta)
{
  if(!(ntaps & 1))
    throw std::out_of_range("Hilbert:  Must have odd number of taps");

  vector<float> taps(ntaps);
  vector<float> w = window (windowtype, ntaps, beta);
  unsigned int h = (ntaps-1)/2;
  float gain=0;
  for (unsigned int i = 1; i <= h; i++)
    {
      if(i&1)
	{
	  float x = 1/(float)i;
	  taps[h+i] = x * w[h+i];
	  taps[h-i] = -x * w[h-i];
	  gain = taps[h+i] - gain;
	}
      else
	taps[h+i] = taps[h-i] = 0;
    }
  gain = 2 * fabs(gain);
  for ( unsigned int i = 0; i < ntaps; i++)
      taps[i] /= gain;
  return taps;
}

//
// Gaussian
//

vector<float>
gr_firdes::gaussian (double gain,
		     double spb,
		     double bt,
		     int ntaps)
{

  vector<float> taps(ntaps);
  double scale = 0;
  double dt = 1.0/spb;
  double s = 1.0/(sqrt(log(2)) / (2*M_PI*bt));
  double t0 = -0.5 * ntaps;
  double ts;
  for(int i=0;i<ntaps;i++)
    {
      t0++;
      ts = s*dt*t0;
      taps[i] = exp(-0.5*ts*ts);
      scale += taps[i];
    }
  for(int i=0;i<ntaps;i++)
    taps[i] = taps[i] / scale * gain;
  return taps;
}


//
// Root Raised Cosine
//

vector<float>
gr_firdes::root_raised_cosine (double gain,
			       double sampling_freq,
			       double symbol_rate,
			       double alpha,
			       int ntaps)
{
  ntaps |= 1;	// ensure that ntaps is odd

  double spb = sampling_freq/symbol_rate; // samples per bit/symbol
  vector<float> taps(ntaps);
  double scale = 0;
  for(int i=0;i<ntaps;i++)
    {
      double x1,x2,x3,num,den;
      double xindx = i - ntaps/2;
      x1 = M_PI * xindx/spb;
      x2 = 4 * alpha * xindx / spb;
      x3 = x2*x2 - 1;
      if( fabs(x3) >= 0.000001 )  // Avoid Rounding errors...
	{
	  if( i != ntaps/2 )
	    num = cos((1+alpha)*x1) + sin((1-alpha)*x1)/(4*alpha*xindx/spb);
	  else
	    num = cos((1+alpha)*x1) + (1-alpha) * M_PI / (4*alpha);
	  den = x3 * M_PI;
	}
      else
	{
	  if(alpha==1)
	    {
	      taps[i] = -1;
	      continue;
	    }
	  x3 = (1-alpha)*x1;
	  x2 = (1+alpha)*x1;
	  num = (sin(x2)*(1+alpha)*M_PI
		 - cos(x3)*((1-alpha)*M_PI*spb)/(4*alpha*xindx)
		 + sin(x3)*spb*spb/(4*alpha*xindx*xindx));
	  den = -32 * M_PI * alpha * alpha * xindx/spb;
	}
      taps[i] = 4 * alpha * num / den;
      scale += taps[i];
    }

  for(int i=0;i<ntaps;i++)
    taps[i] = taps[i] * gain / scale;

  return taps;
}

//
//	=== Utilities ===
//

// delta_f / width_factor gives number of taps required.
static const float width_factor[5] = {   // indexed by win_type
  3.3,			// WIN_HAMMING
  3.1,			// WIN_HANN
  5.5,              	// WIN_BLACKMAN
  2.0,                  // WIN_RECTANGULAR
  //5.0                   // WIN_KAISER  (guesstimate compromise)
  //2.0                   // WIN_KAISER  (guesstimate compromise)
  10.0			// WIN_KAISER
};

int
gr_firdes::compute_ntaps_windes(double sampling_freq,
			         double transition_width, // this is frequency, not relative frequency
			   	 double attenuation_dB)
{
  // Based on formula from Multirate Signal Processing for
  // Communications Systems, fredric j harris
  int ntaps = (int)(attenuation_dB*sampling_freq/(22.0*transition_width));
  if ((ntaps & 1) == 0)	// if even...
    ntaps++;		// ...make odd
  return ntaps;
}

int
gr_firdes::compute_ntaps (double sampling_freq,
			  double transition_width,
			  win_type window_type,
			  double beta)
{
  // normalized transition width
  double delta_f = transition_width / sampling_freq;

  // compute number of taps required for given transition width
  int ntaps = (int) (width_factor[window_type] / delta_f + 0.5);
  if ((ntaps & 1) == 0)	// if even...
    ntaps++;		// ...make odd

  return ntaps;
}

double gr_firdes::bessi0(double x)
{
	double ax,ans;
	double y;

	ax=fabs(x);
	if (ax < 3.75)
	{
		y=x/3.75;
		y*=y;
		ans=1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492
			+y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
	}
	else
	{
		y=3.75/ax;
		ans=(exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1
			+y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2
			+y*(-0.2057706e-1+y*(0.2635537e-1+y*(-0.1647633e-1
			+y*0.392377e-2))))))));
	}
	return ans;
}
vector<float>
gr_firdes::window (win_type type, int ntaps, double beta)
{
  vector<float> taps(ntaps);
  int	M = ntaps - 1;		// filter order

  switch (type){
  case WIN_RECTANGULAR:
    for (int n = 0; n < ntaps; n++)
      taps[n] = 1;

  case WIN_HAMMING:
    for (int n = 0; n < ntaps; n++)
      taps[n] = 0.54 - 0.46 * cos ((2 * M_PI * n) / M);
    break;

  case WIN_HANN:
    for (int n = 0; n < ntaps; n++)
      taps[n] = 0.5 - 0.5 * cos ((2 * M_PI * n) / M);
    break;

  case WIN_BLACKMAN:
    for (int n = 0; n < ntaps; n++)
      taps[n] = 0.42 - 0.50 * cos ((2*M_PI * n) / (M-1)) - 0.08 * cos ((4*M_PI * n) / (M-1));
    break;

  case WIN_BLACKMAN_hARRIS:
    for (int n = -ntaps/2; n < ntaps/2; n++)
      taps[n+ntaps/2] = 0.35875 + 0.48829*cos((2*M_PI * n) / (float)M) + 
	0.14128*cos((4*M_PI * n) / (float)M) + 0.01168*cos((6*M_PI * n) / (float)M);
    break;

#if 0
  case WIN_KAISER:
    for (int n = 0; n < ntaps; n++)
	taps[n] = bessi0(beta*sqrt(1.0 - (4.0*n/(M*M))))/bessi0(beta);
    break;
#else

  case WIN_KAISER:
    {
      double IBeta = 1.0/Izero(beta);
      double inm1 = 1.0/((double)(ntaps));
      double temp;
      //fprintf(stderr, "IBeta = %g; inm1 = %g\n", IBeta, inm1);

      for (int i=0; i<ntaps; i++) {
	temp = i * inm1;
	//fprintf(stderr, "temp = %g\n", temp);
	taps[i] = Izero(beta*sqrt(1.0-temp*temp)) * IBeta;
	//fprintf(stderr, "taps[%d] = %g\n", i, taps[i]);
      }
    }
    break;

#endif
  default:
    throw std::out_of_range ("gr_firdes:window: type out of range");
  }

  return taps;
}

void
gr_firdes::sanity_check_1f (double sampling_freq,
			    double fa,			// cutoff freq
			    double transition_width)
{
  if (sampling_freq <= 0.0)
    throw std::out_of_range ("gr_firdes check failed: sampling_freq > 0");

  if (fa <= 0.0 || fa > sampling_freq / 2)
    throw std::out_of_range ("gr_firdes check failed: 0 < fa <= sampling_freq / 2");
  
  if (transition_width <= 0)
    throw std::out_of_range ("gr_dirdes check failed: transition_width > 0");
}

void
gr_firdes::sanity_check_2f (double sampling_freq,
			    double fa,			// first cutoff freq
			    double fb,			// second cutoff freq
			    double transition_width)
{
  if (sampling_freq <= 0.0)
    throw std::out_of_range ("gr_firdes check failed: sampling_freq > 0");

  if (fa <= 0.0 || fa > sampling_freq / 2)
    throw std::out_of_range ("gr_firdes check failed: 0 < fa <= sampling_freq / 2");
  
  if (fb <= 0.0 || fb > sampling_freq / 2)
    throw std::out_of_range ("gr_firdes check failed: 0 < fb <= sampling_freq / 2");
  
  if (fa > fb)
    throw std::out_of_range ("gr_firdes check failed: fa <= fb");
  
  if (transition_width <= 0)
    throw std::out_of_range ("gr_firdes check failed: transition_width > 0");
}

void
gr_firdes::sanity_check_2f_c (double sampling_freq,
			    double fa,			// first cutoff freq
			    double fb,			// second cutoff freq
			    double transition_width)
{
  if (sampling_freq <= 0.0)
    throw std::out_of_range ("gr_firdes check failed: sampling_freq > 0");

  if (fa < -sampling_freq / 2 || fa > sampling_freq / 2)
    throw std::out_of_range ("gr_firdes check failed: 0 < fa <= sampling_freq / 2");
  
  if (fb < -sampling_freq / 2  || fb > sampling_freq / 2)
    throw std::out_of_range ("gr_firdes check failed: 0 < fb <= sampling_freq / 2");
  
  if (fa > fb)
    throw std::out_of_range ("gr_firdes check failed: fa <= fb");
  
  if (transition_width <= 0)
    throw std::out_of_range ("gr_firdes check failed: transition_width > 0");
}
