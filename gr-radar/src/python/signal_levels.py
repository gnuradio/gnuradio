#!/usr/bin/env python

import sys
from math import pi, log10
from gnuradio.eng_notation import num_to_str

def power_density(transmitted_power, distance):
    """
    Estimate power density in Watts/meter.
    Assume isotropic radiator (spherical radiation pattern).

    @param transmitted_power: Watts
    @param distance: distance from transmitter in meters

    """
    return transmitted_power / (4 * pi * distance * distance)


def ratio_of_target_return_to_direct(Rl, Rt, Rr, rcs):
    """
    Estimate relative strength of signal levels for direct and reflected
    path in bistatic radar.  Assume all antenna factors are constant,
    and hence 'wash out'.  Also assume that the antennas are isotropic
    radiators (spherical radiation pattern).

    @param Rl:  distance between Tx and Rx in meters
    @param Rt:  distance between Tx and target in meters
    @param Rr:  distance between Rx and target in meters
    @param rcs: radar cross section in meters^2

    @returns: ratio of reflected to direct in decibels
    """
    Tx_power = 1
    direct_path_power_density = power_density(Tx_power, Rl)
    power_at_target = power_density(Tx_power, Rt) * rcs
    reflected_path_power_density = power_density(power_at_target, Rr)
    return 10*log10(reflected_path_power_density / direct_path_power_density)


def print_header(f):
    f.write("   Rl      Rt      Rr      rcs      dB\n")
    f.write("-----------------------------------------\n")
    
def print_result(f, Rl, Rt, Rr, rcs, dB):
    f.write("%6sm %6sm %6sm %6s  %6s\n" %  tuple([num_to_str(x) for x in [Rl, Rt, Rr, rcs, dB]]))

def calc_print(f, Rl, Rt, Rr, rcs):
    print_result(f, Rl, Rt, Rr, rcs, ratio_of_target_return_to_direct(Rl, Rt, Rr, rcs))
    
def main():
    f = sys.stdout
    print_header(f)
    calc_print(f, 40e3, 100e3, 100e3, 10.0)
    calc_print(f, 40e3,  80e3,  80e3, 10.0)
    calc_print(f, 40e3,  40e3,  40e3, 10.0)
    calc_print(f, 40e3,  40e3,   8e3, 10.0)
    calc_print(f, 40e3,  60e3,  20e3, 10.0)
    calc_print(f, 40e3,  20e3,  60e3, 10.0)

    
if __name__ == '__main__':
    main()
