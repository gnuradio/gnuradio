
from gnuradio import gr, blks2, packet_utils

# Some constants
NOISE_VOLTAGE = 0.1
FREQUENCY_OFFSET = 0.0000
TIMING_OFFSET = 1.0
SAMPLES_PER_SYMBOL = 2
GAIN = 1.0
SW_DECIM = 1
BAND_MIDPOINT = 1.0
BAND_WIDTH = 0.5
FREQ_ALPHA = 0.005
EXCESS_BW = 0.35

# Modulation/Demodulation methods
modulator = blks2.qam_mod(16, SAMPLES_PER_SYMBOL)
demodulator = blks2.qam_demod(16, SAMPLES_PER_SYMBOL, freq_alpha=FREQ_ALPHA)

#Transmission Blocks
packet_transmitter = blks2.mod_pkts(modulator, access_code=None, msgq_limit=4,
                                    pad_for_usrp=True)
# Channel
channel = gr.channel_model(NOISE_VOLTAGE, FREQUENCY_OFFSET, TIMING_OFFSET)
# Receiver Blocks
chan_coeffs = gr.firdes.low_pass(GAIN, SW_DECIM * SAMPLES_PER_SYMBOL,
                                 BAND_MIDPOINT, BAND_WIDTH, gr.firdes.WIN_HANN)
channel_filter = gr.fft_filter_ccc(SW_DECIM, chan_coeffs)
packet_receiver = blks2.demod_pkts(demodulator, access_code=None, callback=None,
                                   threshold=-1)
# Put it all together and start it up (although nothing will be done
# until we send some packets).
tb = gr.top_block()
tb.connect(packet_transmitter, channel, channel_filter,
           packet_receiver)
tb.start()

# The queue into which recieved packets are placed
pkq = packet_receiver._rcvd_pktq

# The function to create a packet and place it in a queue to be sent.
sender = packet_transmitter.send_pkt

# Some some packets (The second will not be recieved because it gets cut off
# before it can finish.  I think this occurs during modulation.)

# Send some large messages to start off with to let things lock.
for i in range(0, int(20.0/SAMPLES_PER_SYMBOL)):
    sender('a'*4000)

sender('hello1')
sender('hello2')
sender('hello3')
sender('hello4')
sender('hello5')
sender('hello6')
sender('hello7')
sender('hello8')
sender('hello9')
sender('hello10')
sender('hello11')
sender('hello12')
sender('world')
sender(eof=True)

# Wait for all the packets to get sent and received.
tb.wait()

# Check how many messages have been received and print them.
cnt = pkq.count()
print('There are %s messages' % cnt)
for a in range(0, cnt):
    msg = pkq.delete_head()
    ok, payload = packet_utils.unmake_packet(msg.to_string(), int(msg.arg1()))
    print("Message %s is %s" % (a, payload))

