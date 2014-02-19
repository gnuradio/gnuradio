#!/usr/bin/env python
# Copyright 2012 Free Software Foundation, Inc.
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

import time

from gnuradio import gr, gr_unittest, digital, blocks
import pmt

class qa_header_payload_demux (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        """ Simplest possible test: put in zeros, then header,
        then payload, trigger signal, try to demux.
        The return signal from the header parser is faked via _post()
        Add in some tags for fun.
        """
        n_zeros = 1
        header = (1, 2, 3)
        payload = tuple(range(5, 20))
        data_signal = (0,) * n_zeros + header + payload
        trigger_signal = [0,] * len(data_signal)
        trigger_signal[n_zeros] = 1
        # This is dropped:
        testtag1 = gr.tag_t()
        testtag1.offset = 0
        testtag1.key = pmt.string_to_symbol('tag1')
        testtag1.value = pmt.from_long(0)
        # This goes on output 0, item 0:
        testtag2 = gr.tag_t()
        testtag2.offset = n_zeros
        testtag2.key = pmt.string_to_symbol('tag2')
        testtag2.value = pmt.from_long(23)
        # This goes on output 0, item 2:
        testtag3 = gr.tag_t()
        testtag3.offset = n_zeros + len(header) - 1
        testtag3.key = pmt.string_to_symbol('tag3')
        testtag3.value = pmt.from_long(42)
        # This goes on output 1, item 3:
        testtag4 = gr.tag_t()
        testtag4.offset = n_zeros + len(header) + 3
        testtag4.key = pmt.string_to_symbol('tag4')
        testtag4.value = pmt.from_long(314)
        data_src = blocks.vector_source_f(
                data_signal,
                False,
                tags=(testtag1, testtag2, testtag3, testtag4)
        )
        trigger_src = blocks.vector_source_b(trigger_signal, False)
        hpd = digital.header_payload_demux(
            len(header), 1, 0, "frame_len", "detect", False, gr.sizeof_float
        )
        self.assertEqual(pmt.length(hpd.message_ports_in()), 1)
        header_sink = blocks.vector_sink_f()
        payload_sink = blocks.vector_sink_f()

        self.tb.connect(data_src,    (hpd, 0))
        self.tb.connect(trigger_src, (hpd, 1))
        self.tb.connect((hpd, 0), header_sink)
        self.tb.connect((hpd, 1), payload_sink)
        self.tb.start()
        time.sleep(.2) # Need this, otherwise, the next message is ignored
        hpd.to_basic_block()._post(
                pmt.intern('header_data'),
                pmt.from_long(len(payload))
        )
        while len(payload_sink.data()) < len(payload):
            time.sleep(.2)
        self.tb.stop()
        self.tb.wait()

        self.assertEqual(header_sink.data(),  header)
        self.assertEqual(payload_sink.data(), payload)
        ptags_header = []
        for tag in header_sink.tags():
            ptag = gr.tag_to_python(tag)
            ptags_header.append({'key': ptag.key, 'offset': ptag.offset})
        expected_tags_header = [
                {'key': 'tag2', 'offset': 0},
                {'key': 'tag3', 'offset': 2},
        ]
        self.assertEqual(expected_tags_header, ptags_header)
        ptags_payload = []
        for tag in payload_sink.tags():
            ptag = gr.tag_to_python(tag)
            ptags_payload.append({'key': ptag.key, 'offset': ptag.offset})
        expected_tags_payload = [
                {'key': 'frame_len', 'offset': 0},
                {'key': 'tag4', 'offset': 3},
        ]
        self.assertEqual(expected_tags_payload, ptags_payload)

    def test_002_symbols (self):
        """ 
        Same as before, but operate on symbols
        """
        n_zeros = 1
        items_per_symbol = 3
        gi = 1
        n_symbols = 4
        header = (1, 2, 3)
        payload = (1, 2, 3)
        data_signal = (0,) * n_zeros + (0,) + header + ((0,) + payload) * n_symbols
        trigger_signal = [0,] * len(data_signal)
        trigger_signal[n_zeros] = 1
        # This is dropped:
        testtag1 = gr.tag_t()
        testtag1.offset = 0
        testtag1.key = pmt.string_to_symbol('tag1')
        testtag1.value = pmt.from_long(0)
        # This goes on output 0, item 0 (from the GI)
        testtag2 = gr.tag_t()
        testtag2.offset = n_zeros
        testtag2.key = pmt.string_to_symbol('tag2')
        testtag2.value = pmt.from_long(23)
        # This goes on output 0, item 0 (middle of the header symbol)
        testtag3 = gr.tag_t()
        testtag3.offset = n_zeros + gi + 1
        testtag3.key = pmt.string_to_symbol('tag3')
        testtag3.value = pmt.from_long(42)
        # This goes on output 1, item 1 (middle of the first payload symbol)
        testtag4 = gr.tag_t()
        testtag4.offset = n_zeros + (gi + items_per_symbol) * 2 + 1
        testtag4.key = pmt.string_to_symbol('tag4')
        testtag4.value = pmt.from_long(314)
        data_src = blocks.vector_source_f(data_signal, False, tags=(testtag1, testtag2, testtag3, testtag4))
        trigger_src = blocks.vector_source_b(trigger_signal, False)
        hpd = digital.header_payload_demux(
            len(header) / items_per_symbol, # Header length (in symbols)
            items_per_symbol,               # Items per symbols
            gi,                             # Items per guard time
            "frame_len",                    # Frame length tag key
            "detect",                       # Trigger tag key
            True,                           # Output symbols (not items)
            gr.sizeof_float                 # Bytes per item
        )
        self.assertEqual(pmt.length(hpd.message_ports_in()), 1)
        header_sink = blocks.vector_sink_f(items_per_symbol)
        payload_sink = blocks.vector_sink_f(items_per_symbol)
        self.tb.connect(data_src,    (hpd, 0))
        self.tb.connect(trigger_src, (hpd, 1))
        self.tb.connect((hpd, 0), header_sink)
        self.tb.connect((hpd, 1), payload_sink)
        self.tb.start()
        time.sleep(.2) # Need this, otherwise, the next message is ignored
        hpd.to_basic_block()._post(
                pmt.intern('header_data'),
                pmt.from_long(n_symbols)
        )
        while len(payload_sink.data()) < len(payload) * n_symbols:
            time.sleep(.2)
        self.tb.stop()
        self.tb.wait()
        self.assertEqual(header_sink.data(),  header)
        self.assertEqual(payload_sink.data(), payload * n_symbols)
        ptags_header = []
        for tag in header_sink.tags():
            ptag = gr.tag_to_python(tag)
            ptags_header.append({'key': ptag.key, 'offset': ptag.offset})
        expected_tags_header = [
                {'key': 'tag2', 'offset': 0},
                {'key': 'tag3', 'offset': 0},
        ]
        self.assertEqual(expected_tags_header, ptags_header)
        ptags_payload = []
        for tag in payload_sink.tags():
            ptag = gr.tag_to_python(tag)
            ptags_payload.append({'key': ptag.key, 'offset': ptag.offset})
        expected_tags_payload = [
                {'key': 'frame_len', 'offset': 0},
                {'key': 'tag4', 'offset': 1},
        ]
        self.assertEqual(expected_tags_payload, ptags_payload)

    def test_003_t (self):
        """
        Like test 1, but twice, plus one fail
        """
        ### Tx Data
        n_zeros = 5
        header = (1, 2, 3)
        header_fail = (-1, -2, -4) # Contents don't really matter
        payload1 = tuple(range(5, 20))
        payload2 = (42,)
        sampling_rate = 2
        data_signal = (0,) * n_zeros + header + payload1
        trigger_signal = [0,] * len(data_signal) * 2
        trigger_signal[n_zeros] = 1
        trigger_signal[len(data_signal)] = 1
        trigger_signal[len(data_signal)+len(header_fail)+n_zeros] = 1
        tx_signal = data_signal + header_fail + (0,) * n_zeros + header + payload2 + (0,) * 1000
        # Timing tag: This is preserved and updated:
        timing_tag = gr.tag_t()
        timing_tag.offset = 0
        timing_tag.key = pmt.string_to_symbol('rx_time')
        timing_tag.value = pmt.to_pmt((0, 0))
        # Rx freq tags:
        rx_freq_tag1 = gr.tag_t()
        rx_freq_tag1.offset = 0
        rx_freq_tag1.key = pmt.string_to_symbol('rx_freq')
        rx_freq_tag1.value = pmt.from_double(1.0)
        rx_freq_tag2 = gr.tag_t()
        rx_freq_tag2.offset = 29
        rx_freq_tag2.key = pmt.string_to_symbol('rx_freq')
        rx_freq_tag2.value = pmt.from_double(1.5)
        rx_freq_tag3 = gr.tag_t()
        rx_freq_tag3.offset = 30
        rx_freq_tag3.key = pmt.string_to_symbol('rx_freq')
        rx_freq_tag3.value = pmt.from_double(2.0)
        ### Flow graph
        data_src = blocks.vector_source_f(
            tx_signal, False,
            tags=(timing_tag, rx_freq_tag1, rx_freq_tag2, rx_freq_tag3)
        )
        trigger_src = blocks.vector_source_b(trigger_signal, False)
        hpd = digital.header_payload_demux(
            header_len=len(header),
            items_per_symbol=1,
            guard_interval=0,
            length_tag_key="frame_len",
            trigger_tag_key="detect",
            output_symbols=False,
            itemsize=gr.sizeof_float,
            timing_tag_key='rx_time',
            samp_rate=sampling_rate,
            special_tags=('rx_freq',),
        )
        self.assertEqual(pmt.length(hpd.message_ports_in()), 1)
        header_sink = blocks.vector_sink_f()
        payload_sink = blocks.vector_sink_f()
        self.tb.connect(data_src,    (hpd, 0))
        self.tb.connect(trigger_src, (hpd, 1))
        self.tb.connect((hpd, 0), header_sink)
        self.tb.connect((hpd, 1), payload_sink)
        self.tb.start()
        time.sleep(.2) # Need this, otherwise, the next message is ignored
        hpd.to_basic_block()._post(
                pmt.intern('header_data'),
                pmt.from_long(len(payload1))
        )
        while len(payload_sink.data()) < len(payload1):
            time.sleep(.2)
        hpd.to_basic_block()._post(
                pmt.intern('header_data'),
                pmt.PMT_F
        )
        # This next command is a bit of a showstopper, but there's no condition to check upon
        # to see if the previous msg handling is finished
        time.sleep(.7)
        hpd.to_basic_block()._post(
                pmt.intern('header_data'),
                pmt.from_long(len(payload2))
        )
        while len(payload_sink.data()) < len(payload1) + len(payload2):
            time.sleep(.2)
        self.tb.stop()
        self.tb.wait()
        # Signal description:
        # 0:  5 zeros
        # 5:  header 1
        # 8:  payload 1 (length: 15)
        # 23: header 2 (fail)
        # 26: 5 zeros
        # 31: header 3
        # 34: payload 2 (length 1)
        # 35: 1000 zeros
        self.assertEqual(header_sink.data(),  header + header_fail + header)
        self.assertEqual(payload_sink.data(), payload1 + payload2)
        tags_payload = [gr.tag_to_python(x) for x in payload_sink.tags()]
        tags_payload = sorted([(x.offset, x.key, x.value) for x in tags_payload])
        tags_expected_payload = [
            (0,             'frame_len', len(payload1)),
            (len(payload1), 'frame_len', len(payload2)),
        ]
        tags_header = [gr.tag_to_python(x) for x in header_sink.tags()]
        tags_header = sorted([(x.offset, x.key, x.value) for x in tags_header])
        tags_expected_header = [
            (0,             'rx_freq', 1.0),
            (0,             'rx_time', (2, 0.5)), # Hard coded time value :( Is n_zeros/sampling_rate
            (len(header),   'rx_freq', 1.0),
            (len(header),   'rx_time', (11, .5)), # Hard coded time value :(. See above.
            (2*len(header), 'rx_freq', 2.0),
            (2*len(header), 'rx_time', (15, .5)), # Hard coded time value :(. See above.
        ]
        self.assertEqual(tags_header, tags_expected_header)
        self.assertEqual(tags_payload, tags_expected_payload)

if __name__ == '__main__':
    gr_unittest.run(qa_header_payload_demux, "qa_header_payload_demux.xml")

