from gnuradio import eng_notation

def pick_rx_bitrate(bitrate, bits_per_symbol,
                    converter_rate, possible_decims):
    """
    Given the 4 input parameters, return at configuration that matches

    @param bitrate: desired bitrate or None
    @type bitrate: number or None
    @param bits_per_symbol: E.g., BPSK -> 1, QPSK -> 2, 8-PSK -> 3
    @type bits_per_symbol: integer >= 1
    @param converter_rate: converter sample rate in Hz
    @type converter_rate: number
    @param possible_decims: a list of possible rates
    @type possible_decims: a list of integers

    @returns tuple (bitrate, samples_per_symbol, decim_rate)
    """

    rates = list(possible_decims)
    rates.sort()

    for i in xrange(len(rates)):
        if((converter_rate / float(bits_per_symbol) / rates[i]) >= 2*bitrate):
            decim = rates[i]
        else:
            break

    sps = converter_rate / float(bits_per_symbol) / decim / bitrate
    br = converter_rate / float(bits_per_symbol) / decim / sps

    return (br, sps, int(decim))


def pick_tx_bitrate(bitrate, bits_per_symbol,
                    converter_rate, possible_interps):
    """
    Given the 4 input parameters, return at configuration that matches

    @param bitrate: desired bitrate or None
    @type bitrate: number or None
    @param bits_per_symbol: E.g., BPSK -> 1, QPSK -> 2, 8-PSK -> 3
    @type bits_per_symbol: integer >= 1
    @param converter_rate: converter sample rate in Hz
    @type converter_rate: number
    @param possible_interps: a list of possible rates
    @type possible_interps: a list of integers

    @returns tuple (bitrate, samples_per_symbol, interp_rate)
    """

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
