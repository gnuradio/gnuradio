#
# Copyright 2005,2007,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, filter
import math
import cmath


class fm_deemph(gr.hier_block2):
    r"""
    FM Deemphasis IIR filter

    Args:
        fs: sampling frequency in Hz (float)
        tau: Time constant in seconds (75us in US and South Korea, 50us everywhere else) (float)

  An analog deemphasis filter:

               R
  o------/\/\/\/---+----o
                   |
                  = C
                   |
                  ---

  Has this transfer function:

               1             1
              ----          ---
               RC          tau
  H(s) = ---------- = ----------
                 1             1
            s + ----      s + ---
                 RC           tau

  And has its -3 dB response, due to the pole, at

  |H(j w_c)|^2 = 1/2  =>  s = j w_c = j (1/(RC))

  Historically, this corner frequency of analog audio deemphasis filters
  been specified by the RC time constant used, called tau.
  So w_c = 1/tau.

  FWIW, for standard tau values, some standard analog components would be:
  tau = 75 us = (50K)(1.5 nF) = (50 ohms)(1.5 uF)
  tau = 50 us = (50K)(1.0 nF) = (50 ohms)(1.0 uF)

  In specifying tau for this digital deemphasis filter, tau specifies
  the *digital* corner frequency, w_c, desired.

  The digital deemphasis filter design below, uses the
  "bilinear transformation" method of designing digital filters:

  1. Convert digital specifications into the analog domain, by prewarping
     digital frequency specifications into analog frequencies.

     w_a = (2/T)tan(wT/2)

  2. Use an analog filter design technique to design the filter.

  3. Use the bilinear transformation to convert the analog filter design to a
     digital filter design.

     H(z) = H(s)|
                     s = (2/T)(1-z^-1)/(1+z^-1)


         w_ca         1          1 - (-1) z^-1
  H(z) = ---- * ----------- * -----------------------
         2 fs        -w_ca             -w_ca
                 1 - -----         1 + -----
                      2 fs              2 fs
                               1 - ----------- z^-1
                                       -w_ca
                                   1 - -----
                                        2 fs

  We use this design technique, because it is an easy way to obtain a filter
  design with the -6 dB/octave roll-off required of the deemphasis filter.

  Jackson, Leland B., _Digital_Filters_and_Signal_Processing_Second_Edition_,
    Kluwer Academic Publishers, 1989, pp 201-212

  Orfanidis, Sophocles J., _Introduction_to_Signal_Processing_, Prentice Hall,
    1996, pp 573-583
    """

    def __init__(self, fs, tau=75e-6):
        gr.hier_block2.__init__(self, "fm_deemph",
                                # Input signature
                                gr.io_signature(1, 1, gr.sizeof_float),
                                gr.io_signature(1, 1, gr.sizeof_float))  # Output signature

        # Digital corner frequency
        w_c = 1.0 / tau

        # Prewarped analog corner frequency
        w_ca = 2.0 * fs * math.tan(w_c / (2.0 * fs))

        # Resulting digital pole, zero, and gain term from the bilinear
        # transformation of H(s) = w_ca / (s + w_ca) to
        # H(z) = b0 (1 - z1 z^-1)/(1 - p1 z^-1)
        k = -w_ca / (2.0 * fs)
        z1 = -1.0
        p1 = (1.0 + k) / (1.0 - k)
        b0 = -k / (1.0 - k)

        btaps = [b0 * 1.0, b0 * -z1]
        ataps = [1.0, -p1]

        # Since H(s = 0) = 1.0, then H(z = 1) = 1.0 and has 0 dB gain at DC

        deemph = filter.iir_filter_ffd(btaps, ataps, False)
        self.connect(self, deemph, self)


class fm_preemph(gr.hier_block2):
    r"""
    FM Preemphasis IIR filter.

    Args:
        fs: sampling frequency in Hz (float)
        tau: Time constant in seconds (75us in US, 50us in EUR) (float)
        fh: High frequency at which to flatten out (< 0 means default of 0.925*fs/2.0) (float)

  An analog preemphasis filter, that flattens out again at the high end:

               C
         +-----||------+
         |             |
  o------+             +-----+--------o
         |      R1     |     |
         +----/\/\/\/--+     \
                             /
                             \ R2
                             /
                             \
                             |
  o--------------------------+--------o

  (This fine ASCII rendition is based on Figure 5-15
   in "Digital and Analog Communication Systems", Leon W. Couch II)

  Has this transfer function:

                   1
              s + ---
                  R1C
  H(s) = ------------------
               1       R1
          s + --- (1 + --)
              R1C      R2


  It has a corner due to the numerator, where the rise starts, at

  |Hn(j w_cl)|^2 = 2*|Hn(0)|^2  =>  s = j w_cl = j (1/(R1C))

  It has a corner due to the denominator, where it levels off again, at

  |Hn(j w_ch)|^2 = 1/2*|Hd(0)|^2  =>  s = j w_ch = j (1/(R1C) * (1 + R1/R2))

  Historically, the corner frequency of analog audio preemphasis filters
  been specified by the R1C time constant used, called tau.

  So
  w_cl = 1/tau  =         1/R1C; f_cl = 1/(2*pi*tau)  =         1/(2*pi*R1*C)
  w_ch = 1/tau2 = (1+R1/R2)/R1C; f_ch = 1/(2*pi*tau2) = (1+R1/R2)/(2*pi*R1*C)

  and note f_ch = f_cl * (1 + R1/R2).

  For broadcast FM audio, tau is 75us in the United States and 50us in Europe.
  f_ch should be higher than our digital audio bandwidth.

  The Bode plot looks like this:


                     /----------------
                    /
                   /  <-- slope = 20dB/decade
                  /
    -------------/
               f_cl  f_ch

  In specifying tau for this digital preemphasis filter, tau specifies
  the *digital* corner frequency, w_cl, desired.

  The digital preemphasis filter design below, uses the
  "bilinear transformation" method of designing digital filters:

  1. Convert digital specifications into the analog domain, by prewarping
     digital frequency specifications into analog frequencies.

     w_a = (2/T)tan(wT/2)

  2. Use an analog filter design technique to design the filter.

  3. Use the bilinear transformation to convert the analog filter design to a
     digital filter design.

     H(z) = H(s)|
                 s = (2/T)(1-z^-1)/(1+z^-1)


                                  -w_cla
                              1 + ------
                                   2 fs
                         1 - ------------ z^-1
              -w_cla              -w_cla
          1 - ------          1 - ------
               2 fs                2 fs
  H(z) = ------------ * -----------------------
              -w_cha              -w_cha
          1 - ------          1 + ------
               2 fs                2 fs
                         1 - ------------ z^-1
                                  -w_cha
                              1 - ------
                                   2 fs

  We use this design technique, because it is an easy way to obtain a filter
  design with the 6 dB/octave rise required of the premphasis filter.

  Jackson, Leland B., _Digital_Filters_and_Signal_Processing_Second_Edition_,
    Kluwer Academic Publishers, 1989, pp 201-212

  Orfanidis, Sophocles J., _Introduction_to_Signal_Processing_, Prentice Hall,
    1996, pp 573-583
    """

    def __init__(self, fs, tau=75e-6, fh=-1.0):
        gr.hier_block2.__init__(self, "fm_preemph",
                                # Input signature
                                gr.io_signature(1, 1, gr.sizeof_float),
                                gr.io_signature(1, 1, gr.sizeof_float))  # Output signature

        # Set fh to something sensible, if needed.
        # N.B. fh == fs/2.0 or fh == 0.0 results in a pole on the unit circle
        # at z = -1.0 or z = 1.0 respectively.  That makes the filter unstable
        # and useless.
        if fh <= 0.0 or fh >= fs / 2.0:
            fh = 0.925 * fs / 2.0

        # Digital corner frequencies
        w_cl = 1.0 / tau
        w_ch = 2.0 * math.pi * fh

        # Prewarped analog corner frequencies
        w_cla = 2.0 * fs * math.tan(w_cl / (2.0 * fs))
        w_cha = 2.0 * fs * math.tan(w_ch / (2.0 * fs))

        # Resulting digital pole, zero, and gain term from the bilinear
        # transformation of H(s) = (s + w_cla) / (s + w_cha) to
        # H(z) = b0 (1 - z1 z^-1)/(1 - p1 z^-1)
        kl = -w_cla / (2.0 * fs)
        kh = -w_cha / (2.0 * fs)
        z1 = (1.0 + kl) / (1.0 - kl)
        p1 = (1.0 + kh) / (1.0 - kh)
        b0 = (1.0 - kl) / (1.0 - kh)

        # Since H(s = infinity) = 1.0, then H(z = -1) = 1.0 and
        # this filter  has 0 dB gain at fs/2.0.
        # That isn't what users are going to expect, so adjust with a
        # gain, g, so that H(z = 1) = 1.0 for 0 dB gain at DC.
        w_0dB = 2.0 * math.pi * 0.0
        g = abs(1.0 - p1 * cmath.rect(1.0, -w_0dB)) \
            / (b0 * abs(1.0 - z1 * cmath.rect(1.0, -w_0dB)))

        btaps = [g * b0 * 1.0, g * b0 * -z1]
        ataps = [1.0, -p1]

        preemph = filter.iir_filter_ffd(btaps, ataps, False)
        self.connect(self, preemph, self)
