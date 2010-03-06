#!/usr/bin/env python

from gnuradio import gr, gru, modulation_utils
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import random, time, struct, sys, os, math

from threading import Thread

# from current dir
from transmit_path import transmit_path
from receive_path import receive_path

try:
    from gnuradio.qtgui import qtgui
    from PyQt4 import QtGui, QtCore
    import sip
except ImportError:
    print "Please install gr-qtgui."
    sys.exit(1)
    
try:
    from qt_digital_window2 import Ui_DigitalWindow
except ImportError:
    print "Error: could not find qt_digital_window2.py:"
    print "\t\"pyuic4 qt_digital_window2.ui -o qt_digital_window2.py\""
    sys.exit(1)


#print os.getpid()
#raw_input()


# ////////////////////////////////////////////////////////////////////
#        Define the QT Interface and Control Dialog
# ////////////////////////////////////////////////////////////////////


class dialog_box(QtGui.QMainWindow):
    def __init__(self, snkTx, snkRx, fg, parent=None):

        QtGui.QWidget.__init__(self, parent)
        self.gui = Ui_DigitalWindow()
        self.gui.setupUi(self)

        self.fg = fg

        self.set_sample_rate(self.fg.sample_rate())

        self.set_snr(self.fg.snr())
        self.set_frequency(self.fg.frequency_offset())
        self.set_time_offset(self.fg.timing_offset())

        self.set_gain_clock(self.fg.rx_gain_clock())
        self.set_gain_phase(self.fg.rx_gain_phase())
        self.set_gain_freq(self.fg.rx_gain_freq())

        # Add the qtsnk widgets to the hlayout box
        self.gui.sinkLayout.addWidget(snkTx)
        self.gui.sinkLayout.addWidget(snkRx)


        # Connect up some signals
        self.connect(self.gui.pauseButton, QtCore.SIGNAL("clicked()"),
                     self.pauseFg)

        self.connect(self.gui.sampleRateEdit, QtCore.SIGNAL("editingFinished()"),
                     self.sampleRateEditText)

        self.connect(self.gui.snrEdit, QtCore.SIGNAL("editingFinished()"),
                     self.snrEditText)
        self.connect(self.gui.freqEdit, QtCore.SIGNAL("editingFinished()"),
                     self.freqEditText)
        self.connect(self.gui.timeEdit, QtCore.SIGNAL("editingFinished()"),
                     self.timeEditText)

        self.connect(self.gui.gainClockEdit, QtCore.SIGNAL("editingFinished()"),
                     self.gainClockEditText)
        self.connect(self.gui.gainPhaseEdit, QtCore.SIGNAL("editingFinished()"),
                     self.gainPhaseEditText)
        self.connect(self.gui.gainFreqEdit, QtCore.SIGNAL("editingFinished()"),
                     self.gainFreqEditText)

        # Build a timer to update the packet number and PER fields
        self.update_delay = 250  # time between updating packet rate fields
        self.pkt_timer = QtCore.QTimer(self)
        self.connect(self.pkt_timer, QtCore.SIGNAL("timeout()"),
                     self.updatePacketInfo)
        self.pkt_timer.start(self.update_delay)

    def pauseFg(self):
        if(self.gui.pauseButton.text() == "Pause"):
            self.fg.stop()
            self.fg.wait()
            self.gui.pauseButton.setText("Unpause")
        else:
            self.fg.start()
            self.gui.pauseButton.setText("Pause")

    # Accessor functions for Gui to manipulate system parameters
    def set_sample_rate(self, sr):
        ssr = eng_notation.num_to_str(sr)
        self.gui.sampleRateEdit.setText(QtCore.QString("%1").arg(ssr))

    def sampleRateEditText(self):
        try:
            rate = self.gui.sampleRateEdit.text().toAscii()
            srate = eng_notation.str_to_num(rate)
            #self.fg.set_sample_rate(srate)
        except RuntimeError:
            pass


    # Accessor functions for Gui to manipulate channel model
    def set_snr(self, snr):
        self.gui.snrEdit.setText(QtCore.QString("%1").arg(snr))

    def set_frequency(self, fo):
        self.gui.freqEdit.setText(QtCore.QString("%1").arg(fo))

    def set_time_offset(self, to):
        self.gui.timeEdit.setText(QtCore.QString("%1").arg(to))

    def snrEditText(self):
        try:
            snr = self.gui.snrEdit.text().toDouble()[0]
            self.fg.set_snr(snr)
        except RuntimeError:
            pass

    def freqEditText(self):
        try:
            freq = self.gui.freqEdit.text().toDouble()[0]
            self.fg.set_frequency_offset(freq)
        except RuntimeError:
            pass

    def timeEditText(self):
        try:
            to = self.gui.timeEdit.text().toDouble()[0]
            self.fg.set_timing_offset(to)
        except RuntimeError:
            pass


    # Accessor functions for Gui to manipulate receiver parameters
    def set_gain_clock(self, gain):
        self.gui.gainClockEdit.setText(QtCore.QString("%1").arg(gain))

    def set_gain_phase(self, gain_phase):
        self.gui.gainPhaseEdit.setText(QtCore.QString("%1").arg(gain_phase))

    def set_gain_freq(self, gain_freq):
        self.gui.gainFreqEdit.setText(QtCore.QString("%1").arg(gain_freq))
        

    def set_alpha_time(self, alpha):
        self.gui.alphaTimeEdit.setText(QtCore.QString("%1").arg(alpha))

    def set_beta_time(self, beta):
        self.gui.betaTimeEdit.setText(QtCore.QString("%1").arg(beta))

    def set_alpha_phase(self, alpha):
        self.gui.alphaPhaseEdit.setText(QtCore.QString("%1").arg(alpha))

    def gainPhaseEditText(self):
        try:
            gain_phase = self.gui.gainPhaseEdit.text().toDouble()[0]
            self.fg.set_rx_gain_phase(gain_phase)
        except RuntimeError:
            pass

    def gainClockEditText(self):
        try:
            gain = self.gui.gainClockEdit.text().toDouble()[0]
            self.fg.set_rx_gain_clock(gain)
        except RuntimeError:
            pass

    def gainFreqEditText(self):
        try:
            gain = self.gui.gainFreqEdit.text().toDouble()[0]
            self.fg.set_rx_gain_freq(gain)
        except RuntimeError:
            pass

    # Accessor functions for packet error reporting
    def updatePacketInfo(self):
        # Pull these globals in from the main thread
        global n_rcvd, n_right, pktno

        if(pktno > 0):
            per = float(n_rcvd - n_right)/float(pktno)
        else:
            per = 0
        self.gui.pktsRcvdEdit.setText(QtCore.QString("%1").arg(n_rcvd))
        self.gui.pktsCorrectEdit.setText(QtCore.QString("%1").arg(n_right))
        self.gui.perEdit.setText(QtCore.QString("%1").arg(float(per), 0, 'e', 4))



# ////////////////////////////////////////////////////////////////////
#        Define the GNU Radio Top Block
# ////////////////////////////////////////////////////////////////////



class my_top_block(gr.top_block):
    def __init__(self, mod_class, demod_class, rx_callback, options):
        gr.top_block.__init__(self)

        self._sample_rate = options.sample_rate

        channelon = True;

        self.gui_on = options.gui

        self._frequency_offset = options.frequency_offset
        self._timing_offset = options.timing_offset
        self._tx_amplitude = options.tx_amplitude
        self._snr_dB = options.snr

        self._noise_voltage = self.get_noise_voltage(self._snr_dB)

        # With new interface, sps does not get set by default, but
        # in the loopback, we don't recalculate it; so just force it here
        if(options.samples_per_symbol == None):
            options.samples_per_symbol = 2

        self.txpath = transmit_path(mod_class, options)
        self.throttle = gr.throttle(gr.sizeof_gr_complex, self.sample_rate())
        self.rxpath = receive_path(demod_class, rx_callback, options)

        # FIXME: do better exposure to lower issues for control
        self._gain_clock = self.rxpath.packet_receiver._demodulator._timing_alpha
        self._gain_phase = self.rxpath.packet_receiver._demodulator._costas_alpha
        self._gain_freq  = self.rxpath.packet_receiver._demodulator._freq_alpha

        if channelon:
            self.channel = gr.channel_model(self._noise_voltage,
                                            self.frequency_offset(),
                                            self.timing_offset())
            
            if options.discontinuous:
                z = 20000*[0,]
                self.zeros = gr.vector_source_c(z, True)
                packet_size = 5*((4+8+4+1500+4) * 8)
                self.mux = gr.stream_mux(gr.sizeof_gr_complex, [packet_size-0, int(9e5)])

                # Connect components
                self.connect(self.txpath, self.throttle, (self.mux,0))
                self.connect(self.zeros, (self.mux,1))
                self.connect(self.mux, self.channel, self.rxpath)

            else:
                self.connect(self.txpath, self.throttle, self.channel, self.rxpath)

            if self.gui_on:
                self.qapp = QtGui.QApplication(sys.argv)
                fftsize = 2048

                self.snk_tx = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS,
                                           0, 1,
                                           "Tx", True, True, False, True, True)
                self.snk_rx = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS,
                                           0, 1,
                                           "Rx", True, True, False, True, True)

                self.snk_tx.set_frequency_axis(-80, 0)
                self.snk_rx.set_frequency_axis(-60, 20)

                self.freq_recov = self.rxpath.packet_receiver._demodulator.freq_recov
                self.phase_recov = self.rxpath.packet_receiver._demodulator.phase_recov
                self.time_recov = self.rxpath.packet_receiver._demodulator.time_recov
                self.freq_recov.set_alpha(self._gain_freq)
                self.freq_recov.set_beta(self._gain_freq/10.0)
                self.phase_recov.set_alpha(self._gain_phase)
                self.phase_recov.set_beta(0.25*self._gain_phase*self._gain_phase)
                self.time_recov.set_alpha(self._gain_clock)
                self.time_recov.set_beta(0.25*self._gain_clock*self._gain_clock)

                # Connect to the QT sinks
                # FIXME: make better exposure to receiver from rxpath
                self.connect(self.channel, self.snk_tx)
                self.connect(self.phase_recov, self.snk_rx)
                #self.connect(self.freq_recov, self.snk_rx)

                pyTxQt  = self.snk_tx.pyqwidget()
                pyTx = sip.wrapinstance(pyTxQt, QtGui.QWidget)
                 
                pyRxQt  = self.snk_rx.pyqwidget()
                pyRx = sip.wrapinstance(pyRxQt, QtGui.QWidget)

                self.main_box = dialog_box(pyTx, pyRx, self)
                self.main_box.show()
                
        else:
            # Connect components
            self.connect(self.txpath, self.throttle, self.rxpath)



    # System Parameters
    def sample_rate(self):
        return self._sample_rate
    
    def set_sample_rate(self, sr):
        self._sample_rate = sr
        #self.throttle.set_samples_per_second(self._sample_rate)

    # Channel Model Parameters
    def snr(self):
        return self._snr_dB
    
    def set_snr(self, snr):
        self._snr_dB = snr
        self._noise_voltage = self.get_noise_voltage(self._snr_dB)
        self.channel.set_noise_voltage(self._noise_voltage)

    def get_noise_voltage(self, SNR):
        snr = 10.0**(SNR/10.0)        
        power_in_signal = abs(self._tx_amplitude)**2
        noise_power = power_in_signal/snr
        noise_voltage = math.sqrt(noise_power)
        return noise_voltage

    def frequency_offset(self):
        return self._frequency_offset * self.sample_rate()

    def set_frequency_offset(self, fo):
        self._frequency_offset = fo / self.sample_rate()
        self.channel.set_frequency_offset(self._frequency_offset)

    def timing_offset(self):
        return self._timing_offset
    
    def set_timing_offset(self, to):
        self._timing_offset = to
        self.channel.set_timing_offset(self._timing_offset)


    # Receiver Parameters
    def rx_gain_clock(self):
        return self._gain_clock

    def rx_gain_clock_beta(self):
        return self._gain_clock_beta

    def set_rx_gain_clock(self, gain):
        self._gain_clock = gain
        self._gain_clock_beta = .25 * self._gain_clock * self._gain_clock
        self.rxpath.packet_receiver._demodulator.time_recov.set_alpha(self._gain_clock)
        self.rxpath.packet_receiver._demodulator.time_recov.set_beta(self._gain_clock_beta)

    def rx_gain_phase(self):
        return self._gain_phase

    def rx_gain_phase_beta(self):
        return self._gain_phase_beta
    
    def set_rx_gain_phase(self, gain_phase):
        self._gain_phase = gain_phase
        self._gain_phase_beta = .25 * self._gain_phase * self._gain_phase
        self.rxpath.packet_receiver._demodulator.phase_recov.set_alpha(self._gain_phase)
        self.rxpath.packet_receiver._demodulator.phase_recov.set_beta(self._gain_phase_beta)


    def rx_gain_freq(self):
        return self._gain_freq

    def set_rx_gain_freq(self, gain_freq):
        self._gain_freq = gain_freq
        #self._gain_freq_beta = .25 * self._gain_freq * self._gain_freq
        self.rxpath.packet_receiver._demodulator.freq_recov.set_alpha(self._gain_freq)
        self.rxpath.packet_receiver._demodulator.freq_recov.set_beta(self._gain_freq/10.0)
        #self.rxpath.packet_receiver._demodulator.freq_recov.set_beta(self._gain_fre_beta)


# /////////////////////////////////////////////////////////////////////////////
#       Thread to handle the packet sending procedure
#          Operates in parallel with qApp.exec_()       
# /////////////////////////////////////////////////////////////////////////////



class th_send(Thread):
    def __init__(self, send_fnc, megs, sz):
        Thread.__init__(self)
        self.send = send_fnc
        self.nbytes = int(1e6 * megs)
        self.pkt_size = int(sz)

    def run(self):
        # generate and send packets
        n = 0
        pktno = 0
        
        while n < self.nbytes:
            self.send(struct.pack('!H', pktno & 0xffff) +
                      (self.pkt_size - 2) * chr(pktno & 0xff))
            n += self.pkt_size
            pktno += 1
            
        self.send(eof=True)

    def stop(self):
        self.nbytes = 0



# /////////////////////////////////////////////////////////////////////////////
#                                   main
# /////////////////////////////////////////////////////////////////////////////



def main():

    global n_rcvd, n_right, pktno

    n_rcvd = 0
    n_right = 0
    pktno = 0
    
    def rx_callback(ok, payload):
        global n_rcvd, n_right, pktno
        (pktno,) = struct.unpack('!H', payload[0:2])
        n_rcvd += 1
        if ok:
            n_right += 1

        if not options.gui:
            print "ok = %5s  pktno = %4d  n_rcvd = %4d  n_right = %4d" % (
                ok, pktno, n_rcvd, n_right)
        

    def send_pkt(payload='', eof=False):
        return tb.txpath.send_pkt(payload, eof)

    mods = modulation_utils.type_1_mods()
    demods = modulation_utils.type_1_demods()

    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    expert_grp = parser.add_option_group("Expert")
    channel_grp = parser.add_option_group("Channel")

    parser.add_option("-m", "--modulation", type="choice", choices=mods.keys(),
                      default='dbpsk',
                      help="Select modulation from: %s [default=%%default]"
                            % (', '.join(mods.keys()),))

    parser.add_option("-s", "--size", type="eng_float", default=1500,
                      help="set packet size [default=%default]")
    parser.add_option("-M", "--megabytes", type="eng_float", default=1.0,
                      help="set megabytes to transmit [default=%default]")
    parser.add_option("","--discontinuous", action="store_true", default=False,
                      help="enable discontinous transmission (bursts of 5 packets)")
    parser.add_option("-G", "--gui", action="store_true", default=False,
                      help="Turn on the GUI [default=%default]")

    channel_grp.add_option("", "--sample-rate", type="eng_float", default=1e5,
                           help="set speed of channel/simulation rate to RATE [default=%default]") 
    channel_grp.add_option("", "--snr", type="eng_float", default=30,
                           help="set the SNR of the channel in dB [default=%default]")
    channel_grp.add_option("", "--frequency-offset", type="eng_float", default=0,
                           help="set frequency offset introduced by channel [default=%default]")
    channel_grp.add_option("", "--timing-offset", type="eng_float", default=1.0,
                           help="set timing offset introduced by channel [default=%default]")
    channel_grp.add_option("", "--seed", action="store_true", default=False,
                           help="use a random seed for AWGN noise [default=%default]")

    transmit_path.add_options(parser, expert_grp)
    receive_path.add_options(parser, expert_grp)

    for mod in mods.values():
        mod.add_options(expert_grp)
    for demod in demods.values():
        demod.add_options(expert_grp)

    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help()
        sys.exit(1)
        
    r = gr.enable_realtime_scheduling()
    if r != gr.RT_OK:
        print "Warning: failed to enable realtime scheduling"
        
    # Create an instance of a hierarchical block
    tb = my_top_block(mods[options.modulation],
                      demods[options.modulation],
                      rx_callback, options)
    tb.start()

    packet_sender = th_send(send_pkt, options.megabytes, options.size)
    packet_sender.start()

    if(options.gui):
        tb.qapp.exec_()
        packet_sender.stop()
    else:
        # Process until done; hack in to the join to stop on an interrupt
        while(packet_sender.isAlive()):
            try:
                packet_sender.join(1)
            except KeyboardInterrupt:
                packet_sender.stop()
        
    
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
