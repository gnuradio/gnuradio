#
# Copyright 2005,2006 Free Software Foundation, Inc.
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

_default_bitrate = 500e3

_valid_samples_per_symbol = (2,3,4,5,6,7)

def _gen_tx_info(converter_rate):
    results = []
    for samples_per_symbol in _valid_samples_per_symbol:
        for interp in range(16, 512 + 1, 4):
            bitrate = converter_rate / interp / samples_per_symbol
            results.append((bitrate, samples_per_symbol, interp))
    results.sort()
    return results

def _gen_rx_info(converter_rate):
    results = []
    for samples_per_symbol in _valid_samples_per_symbol:
        for decim in range(8, 256 + 1, 2):
            bitrate = converter_rate / decim / samples_per_symbol
            results.append((bitrate, samples_per_symbol, decim))
    results.sort()
    return results
    
def _filter_info(info, samples_per_symbol, xrate):
    if samples_per_symbol is not None:
        info = [x for x in info if x[1] == samples_per_symbol]
    if xrate is not None:
        info = [x for x in info if x[2] == xrate]
    return info

def _pick_best(target_bitrate, bits_per_symbol, info):
    """
    @returns tuple (bitrate, samples_per_symbol, interp_rate_or_decim_rate)
    """
    if len(info) == 0:
        raise RuntimeError, "info is zero length!"

    if target_bitrate is None:     # return the fastest one
        return info[-1]
    
    # convert bit rate to symbol rate
    target_symbolrate = target_bitrate / bits_per_symbol
    
    # Find the closest matching symbol rate.
    # In the event of a tie, the one with the lowest samples_per_symbol wins.
    # (We already sorted them, so the first one is the one we take)

    best = info[0]
    best_delta = abs(target_symbolrate - best[0])
    for x in info[1:]:
        delta = abs(target_symbolrate - x[0])
        if delta < best_delta:
            best_delta = delta
            best = x

    # convert symbol rate back to bit rate
    return ((best[0] * bits_per_symbol),) + best[1:]

def _pick_bitrate(bitrate, bits_per_symbol, samples_per_symbol,
                  xrate, converter_rate, gen_info):
    """
    @returns tuple (bitrate, samples_per_symbol, interp_rate_or_decim_rate)
    """
    if not isinstance(bits_per_symbol, int) or bits_per_symbol < 1:
        raise ValueError, "bits_per_symbol must be an int >= 1"
    
    if samples_per_symbol is not None and xrate is not None:  # completely determined
        return (float(converter_rate) / xrate / samples_per_symbol,
                samples_per_symbol, xrate)

    if bitrate is None and samples_per_symbol is None and xrate is None:
        bitrate = _default_bitrate

    # now we have a target bitrate and possibly an xrate or
    # samples_per_symbol constraint, but not both of them.

    return _pick_best(bitrate, bits_per_symbol,
                      _filter_info(gen_info(converter_rate), samples_per_symbol, xrate))
    
# ---------------------------------------------------------------------------------------

def pick_tx_bitrate(bitrate, bits_per_symbol, samples_per_symbol,
                    interp_rate, converter_rate=128e6):
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

    @returns tuple (bitrate, samples_per_symbol, interp_rate)
    """
    return _pick_bitrate(bitrate, bits_per_symbol, samples_per_symbol,
                         interp_rate, converter_rate, _gen_tx_info)


def pick_rx_bitrate(bitrate, bits_per_symbol, samples_per_symbol,
                    decim_rate, converter_rate=64e6):
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

    @returns tuple (bitrate, samples_per_symbol, decim_rate)
    """
    return _pick_bitrate(bitrate, bits_per_symbol, samples_per_symbol,
                         decim_rate, converter_rate, _gen_rx_info)
