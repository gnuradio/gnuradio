/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_USRP2_TUNE_RESULT_H
#define INCLUDED_USRP2_TUNE_RESULT_H

namespace usrp2 {

  /*
   * \ingroup usrp2
   */
  class tune_result
  {
  public:
    // RF frequency that corresponds to DC in the IF
    double baseband_freq;

    // frequency programmed into the DDC/DUC
    double dxc_freq;

    // residual frequency (typically < 0.01 Hz)
    double residual_freq;

    // is the spectrum inverted?
    bool spectrum_inverted;

    tune_result(double baseband=0, double dxc=0, double residual=0, bool inverted=false)
      : baseband_freq(baseband), dxc_freq(dxc),
	residual_freq(residual), spectrum_inverted(inverted) {}
  };

} // namespace usrp2

#endif /* INCLUDED_USRP2_TUNE_RESULT_H */
