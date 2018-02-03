#!/usr/bin/env python
#
# Copyright 2015 Free Software Foundation
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import sys
import matplotlib
matplotlib.use("QT4Agg")
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from gnuradio.ctrlport.GNURadioControlPortClient import GNURadioControlPortClient
import scipy
from scipy import fftpack

"""
If a host is running the ATSC receiver chain with ControlPort
turned on, this script will connect to the host using the hostname and
port pair of the ControlPort instance and display metrics of the
receiver. The ATSC publishes information about the success of the
Reed-Solomon decoder and Viterbi metrics for use here in displaying
the link quality. This also gets the equalizer taps of the receiver
and displays the frequency response.
"""

class atsc_ctrlport_monitor:
    def __init__(self, host, port):
        argv = [None, host, port]
        radiosys = GNURadioControlPortClient(argv=argv, rpcmethod='thrift')
        self.radio = radiosys.client
        print self.radio


        vt_init_key = 'dtv_atsc_viterbi_decoder0::decoder_metrics'
        data = self.radio.getKnobs([vt_init_key])[vt_init_key]
        init_metric = scipy.mean(data.value)
        self._viterbi_metric = 100*[init_metric,]

        table_col_labels = ('Num Packets', 'Error Rate', 'Packet Error Rate',
                            'Viterbi Metric', 'SNR')

        self._fig = plt.figure(1, figsize=(12,12), facecolor='w')
        self._sp0 = self._fig.add_subplot(4,1,1)
        self._sp1 = self._fig.add_subplot(4,1,2)
        self._sp2 = self._fig.add_subplot(4,1,3)
        self._plot_taps = self._sp0.plot([], [], 'k', linewidth=2)
        self._plot_psd  = self._sp1.plot([], [], 'k', linewidth=2)
        self._plot_data = self._sp2.plot([], [], 'ok', linewidth=2, markersize=4, alpha=0.05)

        self._ax2 = self._fig.add_subplot(4,1,4)
        self._table = self._ax2.table(cellText=[len(table_col_labels)*['0']],
                                      colLabels=table_col_labels,
                                      loc='center')
        self._ax2.axis('off')
        cells = self._table.properties()['child_artists']
        for c in cells:
            c.set_lw(0.1) # set's line width
            c.set_ls('solid')
            c.set_height(0.2)

        ani = animation.FuncAnimation(self._fig, self.update_data, frames=200,
                                      fargs=(self._plot_taps[0], self._plot_psd[0],
                                             self._plot_data[0], self._table),
                                      init_func=self.init_function,
                                      blit=True)
        plt.show()

    def update_data(self, x, taps, psd, syms, table):
        try:
            eqdata_key = 'dtv_atsc_equalizer0::taps'
            symdata_key = 'dtv_atsc_equalizer0::data'
            rs_nump_key = 'dtv_atsc_rs_decoder0::num_packets'
            rs_numbp_key = 'dtv_atsc_rs_decoder0::num_bad_packets'
            rs_numerrs_key = 'dtv_atsc_rs_decoder0::num_errors_corrected'
            vt_metrics_key = 'dtv_atsc_viterbi_decoder0::decoder_metrics'
            snr_key = 'probe2_f0::SNR'

            data = self.radio.getKnobs([])
            eqdata = data[eqdata_key]
            symdata = data[symdata_key]
            rs_num_packets = data[rs_nump_key]
            rs_num_bad_packets = data[rs_numbp_key]
            rs_num_errors_corrected = data[rs_numerrs_key]
            vt_decoder_metrics = data[vt_metrics_key]
            snr_est = data[snr_key]

            vt_decoder_metrics = scipy.mean(vt_decoder_metrics.value)
            self._viterbi_metric.pop()
            self._viterbi_metric.insert(0, vt_decoder_metrics)

        except:
            sys.stderr.write("Lost connection, exiting")
            sys.exit(1)

        ntaps = len(eqdata.value)
        taps.set_ydata(eqdata.value)
        taps.set_xdata(xrange(ntaps))
        self._sp0.set_xlim(0, ntaps)
        self._sp0.set_ylim(min(eqdata.value), max(eqdata.value))

        fs = 6.25e6
        freq = scipy.linspace(-fs/2, fs/2, 10000)
        H = fftpack.fftshift(fftpack.fft(eqdata.value, 10000))
        HdB = 20.0*scipy.log10(abs(H))
        psd.set_ydata(HdB)
        psd.set_xdata(freq)
        self._sp1.set_xlim(0, fs/2)
        self._sp1.set_ylim([min(HdB), max(HdB)])
        self._sp1.set_yticks([min(HdB), max(HdB)])
        self._sp1.set_yticklabels(["min", "max"])

        nsyms = len(symdata.value)
        syms.set_ydata(symdata.value)
        syms.set_xdata(nsyms*[0,])
        self._sp2.set_xlim([-1, 1])
        self._sp2.set_ylim([-10, 10])

        per = float(rs_num_bad_packets.value) / float(rs_num_packets.value)
        ber = float(rs_num_errors_corrected.value) / float(187*rs_num_packets.value)

        table._cells[(1,0)]._text.set_text("{0}".format(rs_num_packets.value))
        table._cells[(1,1)]._text.set_text("{0:.2g}".format(ber))
        table._cells[(1,2)]._text.set_text("{0:.2g}".format(per))
        table._cells[(1,3)]._text.set_text("{0:.1f}".format(scipy.mean(self._viterbi_metric)))
        table._cells[(1,4)]._text.set_text("{0:.4f}".format(snr_est.value[0]))

        return (taps, psd, syms, table)

    def init_function(self):
        return self._plot_taps + self._plot_psd + self._plot_data

if __name__ == "__main__":
    host = sys.argv[1]
    port = sys.argv[2]
    m = atsc_ctrlport_monitor(host, port)
