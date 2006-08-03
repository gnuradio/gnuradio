#
# Copyright 2005,2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

from math import pi
import Numeric

from gnuradio import gr, packet_utils
import gnuradio.gr.gr_threading as _threading
import gmsk2


def _deprecation_warning(old_name, new_name):
    print '#'
    print '#  Warning: %s is deprecated and will be removed soon.' % (old_name,)
    print '#           Please use the modulation independent block, %s.' % (new_name,)
    print "#"


# /////////////////////////////////////////////////////////////////////////////
#                   GMSK mod/demod with packets as i/o
# /////////////////////////////////////////////////////////////////////////////

class gmsk2_mod_pkts(gr.hier_block):
    """
    GSM modulator that is a GNU Radio source.

    Send packets by calling send_pkt
    """
    def __init__(self, fg, access_code=None, msgq_limit=2, pad_for_usrp=True, *args, **kwargs):
        """
	Hierarchical block for Gaussian Minimum Shift Key (GMSK) modulation.

        Packets to be sent are enqueued by calling send_pkt.
        The output is the complex modulated signal at baseband.

	@param fg: flow graph
	@type fg: flow graph
        @param access_code: AKA sync vector
        @type access_code: string of 1's and 0's between 1 and 64 long
        @param msgq_limit: maximum number of messages in message queue
        @type msgq_limit: int
        @param pad_for_usrp: If true, packets are padded such that they end up a multiple of 128 samples

        See gmsk_mod for remaining parameters
        """
        _deprecation_warning('gmsk2_mod_pkts', 'mod_pkts')
        
        self.pad_for_usrp = pad_for_usrp
        if access_code is None:
            access_code = packet_utils.default_access_code
        if not packet_utils.is_1_0_string(access_code):
            raise ValueError, "Invalid access_code %r. Must be string of 1's and 0's" % (access_code,)
        self._access_code = access_code

        # accepts messages from the outside world
        self.pkt_input = gr.message_source(gr.sizeof_char, msgq_limit)
        self.gmsk_mod = gmsk2.gmsk2_mod(fg, *args, **kwargs)
        fg.connect(self.pkt_input, self.gmsk_mod)
        gr.hier_block.__init__(self, fg, None, self.gmsk_mod)

    def send_pkt(self, payload='', eof=False):
        """
        Send the payload.

        @param payload: data to send
        @type payload: string
        """
        if eof:
            msg = gr.message(1) # tell self.pkt_input we're not sending any more packets
        else:
            # print "original_payload =", string_to_hex_list(payload)
            pkt = packet_utils.make_packet(payload,
                                           self.gmsk_mod.samples_per_baud(),
                                           self.gmsk_mod.bits_per_baud(),
                                           self._access_code,
                                           self.pad_for_usrp)
            #print "pkt =", string_to_hex_list(pkt)
            msg = gr.message_from_string(pkt)
        self.pkt_input.msgq().insert_tail(msg)



class gmsk2_demod_pkts(gr.hier_block):
    """
    GSM demodulator that is a GNU Radio sink.

    The input is complex baseband.  When packets are demodulated, they are passed to the
    app via the callback.
    """

    def __init__(self, fg, access_code=None, callback=None, threshold=-1, *args, **kwargs):
        """
	Hierarchical block for Gaussian Minimum Shift Key (GMSK)
	demodulation.

	The input is the complex modulated signal at baseband.
        Demodulated packets are sent to the handler.

	@param fg: flow graph
	@type fg: flow graph
        @param access_code: AKA sync vector
        @type access_code: string of 1's and 0's
        @param callback:  function of two args: ok, payload
        @type callback: ok: bool; payload: string
        @param threshold: detect access_code with up to threshold bits wrong (-1 -> use default)
        @type threshold: int

        See gmsk_demod for remaining parameters.
	"""

        _deprecation_warning('gmsk2_demod_pkts', 'demod_pkts')

        if access_code is None:
            access_code = packet_utils.default_access_code
        if not packet_utils.is_1_0_string(access_code):
            raise ValueError, "Invalid access_code %r. Must be string of 1's and 0's" % (access_code,)
        self._access_code = access_code

        if threshold == -1:
            threshold = 12              # FIXME raise exception

        self._rcvd_pktq = gr.msg_queue()          # holds packets from the PHY
        self.gmsk_demod = gmsk2.gmsk2_demod(fg, *args, **kwargs)
        self.correlator = gr.correlate_access_code_bb(access_code, threshold)

        self.framer_sink = gr.framer_sink_1(self._rcvd_pktq)
        fg.connect(self.gmsk_demod, self.correlator, self.framer_sink)
        
        gr.hier_block.__init__(self, fg, self.gmsk_demod, None)
        self._watcher = _queue_watcher_thread(self._rcvd_pktq, callback)

    def carrier_sensed(self):
        """
        Return True if we detect carrier.
        """
        return False        # FIXME


class _queue_watcher_thread(_threading.Thread):
    def __init__(self, rcvd_pktq, callback):
        _threading.Thread.__init__(self)
        self.setDaemon(1)
        self.rcvd_pktq = rcvd_pktq
        self.callback = callback
        self.keep_running = True
        self.start()

    #def stop(self):
    #    self.keep_running = False
        
    def run(self):
        while self.keep_running:
            msg = self.rcvd_pktq.delete_head()
            ok, payload = packet_utils.unmake_packet(msg.to_string())
            if self.callback:
                self.callback(ok, payload)
