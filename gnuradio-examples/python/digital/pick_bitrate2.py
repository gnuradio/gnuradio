#
# Copyright 2010 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import eng_notation

_default_bitrate = 500e3

def _pick_bitrate(bitrate, bits_per_symbol, samples_per_symbol,
                  xrate, converter_rate, xrates, gen_info):
    """
    @returns tuple (bitrate, samples_per_symbol, interp_rate_or_decim_rate)
    """

    if not isinstance(bits_per_symbol, int) or bits_per_symbol < 1:
        raise ValueError, "bits_per_symbol must be an int >= 1"
    
    if samples_per_symbol is not None and xrate is not None:  # completely determined
        return (float(converter_rate) / xrate / samples_per_symbol,
                samples_per_symbol, xrate)

    # If no parameters are give, use the default bit rate
    if bitrate is None and samples_per_symbol is None and xrate is None:
        bitrate = _default_bitrate

    # If only bitrate is specified, return max xrate and appropriate
    # samples per symbol to reach bit rate
    if samples_per_symbol is None and xrate is None:
        xrates.sort()
        for i in xrange(len(xrates)):
            if((converter_rate / float(bits_per_symbol) / xrates[i]) >= 2*bitrate):
                decim = xrates[i]
            else:
                break
            
        sps = converter_rate / float(bits_per_symbol) / decim / bitrate
        br = converter_rate / float(bits_per_symbol) / decim / sps

        return (br, sps, int(decim))


    # now we have a target bitrate and possibly an xrate or
    # samples_per_symbol constraint, but not both of them.
    ret = _pick_best(bitrate, bits_per_symbol,
                      _filter_info(gen_info(converter_rate, xrates),
                                   samples_per_symbol, xrate))

    print "Actual Bitrate:", eng_notation.num_to_str(ret[0])
    return ret



def pick_tx_bitrate(bitrate, bits_per_symbol, samples_per_symbol,
                    interp_rate, converter_rate, possible_interps):
    """
    Given the 4 input parameters, return at configuration that matches

    @param bitrate: desired bitrate or None
    @type bitrate: number or None
    @param bits_per_symbol: E.g., BPSK -> 1, QPSK -> 2, 8-PSK -> 3
    @type bits_per_symbol: integer >= 1
    @param samples_per_symbol: samples/baud (aka samples/symbol)
    @type samples_per_symbol: number or None
    @param interp_rate: USRP interpolation factor
    @type interp_rate: integer or None
    @param converter_rate: converter sample rate in Hz
    @type converter_rate: number
    @param possible_interps: a list of possible rates
    @type possible_interps: a list of integers

    @returns tuple (bitrate, samples_per_symbol, interp_rate)
    """

    return _pick_bitrate(bitrate, bits_per_symbol, samples_per_symbol,
                         interp_rate, converter_rate, possible_interps, _gen_tx_info)

    rates = list(possible_interps)
    rates.sort()

    for i in xrange(len(rates)):
        if((converter_rate / float(bits_per_symbol) / rates[i]) >= 2*bitrate):
            interp = rates[i]
        else:
            break

    sps = converter_rate / float(bits_per_symbol) / interp / bitrate
    br = converter_rate / float(bits_per_symbol) / interp / sps

    return (br, sps, int(interp))
