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
_sps_min = 2
_sps_max = 100

def _pick_bitrate(bitrate, bits_per_symbol, samples_per_symbol,
                  xrate, converter_rate, xrates):
    """
    @returns tuple (bitrate, samples_per_symbol, interp_rate_or_decim_rate)
    """

    if not isinstance(bits_per_symbol, int) or bits_per_symbol < 1:
        raise ValueError, "bits_per_symbol must be an int >= 1"

    converter_rate = float(converter_rate)
    bits_per_symbol = float(bits_per_symbol)

    # completely determined; if bitrate is specified, this overwrites it
    if (samples_per_symbol is not None) and (xrate is not None):
        bitrate = converter_rate / bits_per_symbol / xrate / samples_per_symbol

    # If only SPS is given
    if (bitrate is None) and (samples_per_symbol is not None) and (xrate is None):
        xrate = max(xrates)
        bitrate = converter_rate / bits_per_symbol / xrate / samples_per_symbol
        
    # If only xrate is given, just set SPS to 2 and calculate bitrate
    if (bitrate is None) and (samples_per_symbol is None) and (xrate is not None):
        samples_per_symbol = 2.0
        bitrate = converter_rate / bits_per_symbol / xrate / samples_per_symbol

    # If no parameters are give, use the default bit rate
    if (bitrate is None) and (samples_per_symbol is None) and (xrate is None):
        bitrate = _default_bitrate

    # If only bitrate is specified, return max xrate and appropriate
    # samples per symbol (minimum of 2.0) to reach bit rate
    if (samples_per_symbol is None) and (xrate is None):
        xrates.sort()
        for i in xrange(len(xrates)):
            if((converter_rate / bits_per_symbol / xrates[i]) >= 2*bitrate):
                rate = xrates[i]
            else:
                break

        try:
            xrate = rate
        except UnboundLocalError:
            print "Requested bitrate out of bounds"
            sys.exit(1)
            
        samples_per_symbol = converter_rate / bits_per_symbol / rate / bitrate
        bitrate = converter_rate / bits_per_symbol / xrate / samples_per_symbol

    # If bitrate and xrate are specified
    if(samples_per_symbol is None):
        samples_per_symbol = converter_rate / xrate / bits_per_symbol / bitrate

    # If bitrate and SPS are specified
    if(xrate is None):
        xrate = converter_rate / samples_per_symbol / bits_per_symbol / bitrate
        if((xrate in xrates) == False):
            # Find the closest avaiable rate larger than the calculated one
            xrates.sort()
            for x in xrates:
                if(x > xrate):
                    xrate = x
                    break
            if(xrate > max(xrates)):
                xrate = max(xrates)
            
            bitrate = converter_rate / bits_per_symbol / xrate / samples_per_symbol
            print "Could not find suitable rate for specified SPS and Bitrate"
            print "Using rate = %d for bitrate of %sbps" % \
                  (xrate, (eng_notation.num_to_str(bitrate)))

    if((xrate in xrates) == False):
        raise ValueError(("Invalid rate (rate = %d)" % xrate))
    if((samples_per_symbol < _sps_min) or (samples_per_symbol > _sps_max)):
        raise ValueError(("Invalid samples per symbol (sps = %.2f). Must be in [%.0f, %.0f]." \
                          % (xrate, _sps_min, _sps_max)))
        
    return (bitrate, samples_per_symbol, int(xrate))


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
                         interp_rate, converter_rate, possible_interps)


def pick_rx_bitrate(bitrate, bits_per_symbol, samples_per_symbol,
                    decim_rate, converter_rate, possible_decims):
    """
    Given the 4 input parameters, return at configuration that matches

    @param bitrate: desired bitrate or None
    @type bitrate: number or None
    @param bits_per_symbol: E.g., BPSK -> 1, QPSK -> 2, 8-PSK -> 3
    @type bits_per_symbol: integer >= 1
    @param samples_per_symbol: samples/baud (aka samples/symbol)
    @type samples_per_symbol: number or None
    @param decim_rate: USRP decimation factor
    @type decim_rate: integer or None
    @param converter_rate: converter sample rate in Hz
    @type converter_rate: number
    @param possible_decims: a list of possible rates
    @type possible_decims: a list of integers

    @returns tuple (bitrate, samples_per_symbol, decim_rate)
    """

    return _pick_bitrate(bitrate, bits_per_symbol, samples_per_symbol,
                         decim_rate, converter_rate, possible_decims)
