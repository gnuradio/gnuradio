#!/usr/bin/env python
# Copyright 2012-2016 Free Software Foundation, Inc.
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

from __future__ import print_function
import time
import random
import numpy
from gnuradio import gr
from gnuradio import gr_unittest
from gnuradio import digital
from gnuradio import blocks
import pmt

def make_tag(key, value, offset):
    tag = gr.tag_t()
    tag.offset = offset
    tag.key = pmt.string_to_symbol(key)
    tag.value = pmt.to_pmt(value)
    return tag


class HeaderToMessageBlock(gr.sync_block):
    """
    Helps with testing the HPD. Receives a header, stores it, posts
    a predetermined message.
    """
    def __init__(self, itemsize, header_len, messages, header_is_symbol=False):
        gr.sync_block.__init__(
            self,
            name="HeaderToMessageBlock",
            in_sig=[itemsize],
            out_sig=[itemsize],
        )
        self.header_len = header_len
        self.message_port_register_out(pmt.intern('header_data'))
        self.messages = messages
        self.msg_count = 0

    def work(self, input_items, output_items):
        for i in xrange(len(input_items[0])/self.header_len):
            msg = self.messages[self.msg_count] or False
            #print("Sending message: {0}".format(msg))
            self.message_port_pub(pmt.intern('header_data'), pmt.to_pmt(msg))
            self.msg_count += 1
        output_items[0][:] = input_items[0][:]
        return len(input_items[0])


class qa_header_payload_demux (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def connect_all_blocks(self,
            data_src, trigger_src,
            hpd,
            mock_header_demod,
            payload_sink, header_sink
    ):
        """
        Connect the standard HPD test flowgraph
        """
        self.tb.connect(data_src,    (hpd, 0))
        if trigger_src is not None:
            self.tb.connect(trigger_src, (hpd, 1))
        self.tb.connect((hpd, 0), mock_header_demod)
        self.tb.connect(mock_header_demod, header_sink)
        self.tb.msg_connect(
                mock_header_demod, 'header_data',
                hpd, 'header_data'
        )
        self.tb.connect((hpd, 1), payload_sink)

    def run_tb(self, payload_sink, payload_len, header_sink, header_len, timeout=30):
        stop_time = time.time() + timeout
        self.tb.start()
        while len(payload_sink.data()) < payload_len and \
                len(header_sink.data()) < header_len and \
                time.time() < stop_time:
            time.sleep(.2)
        self.tb.stop()
        self.tb.wait()

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
        testtag1 = make_tag('tag1', 0, 0)
        # This goes on output 0, item 0:
        testtag2 = make_tag('tag2', 23, n_zeros)
        # This goes on output 0, item 2:
        testtag3 = make_tag('tag3', 42, n_zeros + len(header) - 1)
        # This goes on output 1, item 3:
        testtag4 = make_tag('tag4', 314, n_zeros + len(header) + 3)
        data_src = blocks.vector_source_f(
                data_signal,
                False,
                tags=(testtag1, testtag2, testtag3, testtag4)
        )
        trigger_src = blocks.vector_source_b(trigger_signal, False)
        hpd = digital.header_payload_demux(
            len(header), 1, 0, "frame_len", "detect", False, gr.sizeof_float
        )
        mock_header_demod = HeaderToMessageBlock(
                numpy.float32,
                len(header),
                [len(payload)]
        )
        self.assertEqual(pmt.length(hpd.message_ports_in()), 2) #extra system port defined for you
        payload_sink = blocks.vector_sink_f()
        header_sink = blocks.vector_sink_f()
        self.connect_all_blocks(data_src, trigger_src, hpd, mock_header_demod, payload_sink, header_sink)
        self.run_tb(payload_sink, len(payload), header_sink, len(header))
        self.assertEqual(header_sink.data(), header)
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

    def test_001_t_tags (self):
        """ Like the previous test, but use a trigger tag instead of
        a trigger signal.
        """
        n_zeros = 1
        header = (1, 2, 3)
        payload = tuple(range(5, 20))
        data_signal = (0,) * n_zeros + header + payload
        # Trigger tag
        trigger_tag = make_tag('detect', True, n_zeros)
        # This is dropped:
        testtag1 = make_tag('tag1', 0, 0)
        # This goes on output 0, item 0:
        testtag2 = make_tag('tag2', 23, n_zeros)
        # This goes on output 0, item 2:
        testtag3 = make_tag('tag3', 42, n_zeros + len(header) - 1)
        # This goes on output 1, item 3:
        testtag4 = make_tag('tag4', 314, n_zeros + len(header) + 3)
        data_src = blocks.vector_source_f(
                data_signal,
                False,
                tags=(trigger_tag, testtag1, testtag2, testtag3, testtag4)
        )
        hpd = digital.header_payload_demux(
            len(header), 1, 0, "frame_len", "detect", False, gr.sizeof_float
        )
        self.assertEqual(pmt.length(hpd.message_ports_in()), 2) #extra system port defined for you
        header_sink = blocks.vector_sink_f()
        payload_sink = blocks.vector_sink_f()
        mock_header_demod = HeaderToMessageBlock(
                numpy.float32,
                len(header),
                [len(payload)]
        )
        self.connect_all_blocks(data_src, None, hpd, mock_header_demod, payload_sink, header_sink)
        self.run_tb(payload_sink, len(payload), header_sink, len(header))
        # Check results
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

    def test_001_headerpadding (self):
        """ Like test 1, but with header padding. """
        n_zeros = 3
        header = (1, 2, 3)
        header_padding = 1
        payload = tuple(range(5, 20))
        data_signal = (0,) * n_zeros + header + payload
        trigger_signal = [0,] * len(data_signal)
        trigger_signal[n_zeros] = 1
        # This is dropped:
        testtag1 = make_tag('tag1', 0, 0)
        # This goes on output 0, item 0:
        testtag2 = make_tag('tag2', 23, n_zeros)
        # This goes on output 0, item 2:
        testtag3 = make_tag('tag3', 42, n_zeros + len(header) - 1)
        # This goes on output 1, item 3:
        testtag4 = make_tag('tag4', 314, n_zeros + len(header) + 3)
        data_src = blocks.vector_source_f(
                data_signal,
                False,
                tags=(testtag1, testtag2, testtag3, testtag4)
        )
        trigger_src = blocks.vector_source_b(trigger_signal, False)
        hpd = digital.header_payload_demux(
            len(header),
            1, # Items per symbol
            0, # Guard interval
            "frame_len", # TSB tag key
            "detect", # Trigger tag key
            False, # No symbols please
            gr.sizeof_float, # Item size
            "", # Timing tag key
            1.0, # Samp rate
            (), # No special tags
            header_padding
        )
        mock_header_demod = HeaderToMessageBlock(
            numpy.float32,
            len(header),
            [len(payload)]
        )
        header_sink = blocks.vector_sink_f()
        payload_sink = blocks.vector_sink_f()
        self.connect_all_blocks(data_src, trigger_src, hpd, mock_header_demod, payload_sink, header_sink)
        self.run_tb(payload_sink, len(payload), header_sink, len(header)+2)
        # Check values
        # Header now is padded:
        self.assertEqual(header_sink.data(),  (0,) + header + (payload[0],))
        self.assertEqual(payload_sink.data(), payload)
        ptags_header = []
        for tag in header_sink.tags():
            ptag = gr.tag_to_python(tag)
            ptags_header.append({'key': ptag.key, 'offset': ptag.offset})
        expected_tags_header = [
                {'key': 'tag2', 'offset': 1},
                {'key': 'tag3', 'offset': 3},
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

    def test_001_headerpadding_payload_offset (self):
        """ Like test 1, but with header padding + payload offset. """
        n_zeros = 3
        header = (1, 2, 3)
        header_padding = 1
        payload_offset = -1
        payload = tuple(range(5, 20))
        data_signal = (0,) * n_zeros + header + payload + (0,) * 100
        trigger_signal = [0,] * len(data_signal)
        trigger_signal[n_zeros] = 1
        # This goes on output 1, item 3 + 1 (for payload offset)
        testtag4 = make_tag('tag4', 314, n_zeros + len(header) + 3)
        data_src = blocks.vector_source_f(
                data_signal,
                False,
                tags=(testtag4,)
        )
        trigger_src = blocks.vector_source_b(trigger_signal, False)
        hpd = digital.header_payload_demux(
            len(header),
            1, # Items per symbol
            0, # Guard interval
            "frame_len", # TSB tag key
            "detect", # Trigger tag key
            False, # No symbols please
            gr.sizeof_float, # Item size
            "", # Timing tag key
            1.0, # Samp rate
            (), # No special tags
            header_padding
        )
        self.assertEqual(pmt.length(hpd.message_ports_in()), 2) #extra system port defined for you
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
                pmt.to_pmt({'frame_len': len(payload), 'payload_offset': payload_offset})
        )
        while len(payload_sink.data()) < len(payload):
            time.sleep(.2)
        self.tb.stop()
        self.tb.wait()
        # Header is now padded:
        self.assertEqual(header_sink.data(),  (0,) + header + (payload[0],))
        # Payload is now offset:
        self.assertEqual(
                payload_sink.data(),
                data_signal[n_zeros + len(header) + payload_offset:n_zeros + len(header) + payload_offset + len(payload)]
        )
        ptags_payload = {}
        for tag in payload_sink.tags():
            ptag = gr.tag_to_python(tag)
            ptags_payload[ptag.key] = ptag.offset
        expected_tags_payload = {
                'frame_len': 0,
                'payload_offset': 0,
                'tag4': 3 - payload_offset,
        }
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
        testtag1 = make_tag('tag1', 0, 0)
        # This goes on output 0, item 0 (from the GI)
        testtag2 = make_tag('tag2', 23, n_zeros)
        # This goes on output 0, item 0 (middle of the header symbol)
        testtag3 = make_tag('tag3', 42, n_zeros + gi + 1)
        # This goes on output 1, item 1 (middle of the first payload symbol)
        testtag4 = make_tag('tag4', 314, n_zeros + (gi + items_per_symbol) * 2 + 1)
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
        self.assertEqual(pmt.length(hpd.message_ports_in()), 2) #extra system port defined for you
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
        print("Triggers at: {0} {1} {2}".format(
            n_zeros,
            len(data_signal),
            len(data_signal)+len(header_fail)+n_zeros)
        )
        tx_signal = data_signal + \
                header_fail + (0,) * n_zeros + \
                header + payload2 + (0,) * 1000
        # Timing tag: This is preserved and updated:
        timing_tag = make_tag('rx_time', (0, 0), 0)
        # Rx freq tags:
        rx_freq_tag1 = make_tag('rx_freq', 1.0, 0)
        rx_freq_tag2 = make_tag('rx_freq', 1.5, 29)
        rx_freq_tag3 = make_tag('rx_freq', 2.0, 30)
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
        self.assertEqual(pmt.length(hpd.message_ports_in()), 2) #extra system port defined for you
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

    def test_004_fuzz(self):
        """
        Long random test
        """
        def create_signal(
                n_bursts,
                header_len,
                max_gap,
                max_burstsize,
                fail_rate,
        ):
            signal = []
            indexes = []
            burst_sizes = []
            total_payload_len = 0
            for burst_count in xrange(n_bursts):
                gap_size = random.randint(0, max_gap)
                signal += [0] * gap_size
                is_failure = random.random() < fail_rate
                if not is_failure:
                    burst_size = random.randint(0, max_burstsize)
                else:
                    burst_size = 0
                total_payload_len += burst_size
                indexes += [len(signal)]
                signal += [1] * header_len
                signal += [2] * burst_size
                burst_sizes += [burst_size]
            return (signal, indexes, total_payload_len, burst_sizes)
        def indexes_to_triggers(indexes, signal_len):
            """
            Convert indexes to a mix of trigger signals and tags
            """
            trigger_signal = [0] * signal_len
            trigger_tags = []
            for index in indexes:
                if random.random() > 0.5:
                    trigger_signal[index] = 1
                else:
                    trigger_tags += [make_tag('detect', True, index)]
            return (trigger_signal, trigger_tags)
        ### Go, go, go
        # The divide-by-20 means we'll usually get the same random seed
        # between the first run and the XML run.
        random_seed = int(time.time()/20)
        random.seed(random_seed)
        print("Random seed: {0}".format(random_seed))
        n_bursts = 400
        header_len = 5
        max_gap = 50
        max_burstsize = 100
        fail_rate = 0.05
        signal, indexes, total_payload_len, burst_sizes = create_signal(
            n_bursts, header_len, max_gap, max_burstsize, fail_rate
        )
        trigger_signal, trigger_tags = indexes_to_triggers(indexes, len(signal))
        # Flow graph
        data_src = blocks.vector_source_f(
            signal, False,
            tags=trigger_tags
        )
        trigger_src = blocks.vector_source_b(trigger_signal, False)
        hpd = digital.header_payload_demux(
            header_len=header_len,
            items_per_symbol=1,
            guard_interval=0,
            length_tag_key="frame_len",
            trigger_tag_key="detect",
            output_symbols=False,
            itemsize=gr.sizeof_float,
            timing_tag_key='rx_time',
            samp_rate=1.0,
            special_tags=('rx_freq',),
        )
        mock_header_demod = HeaderToMessageBlock(
                numpy.float32,
                header_len,
                burst_sizes
        )
        header_sink = blocks.vector_sink_f()
        payload_sink = blocks.vector_sink_f()
        self.connect_all_blocks(data_src, trigger_src, hpd, mock_header_demod, payload_sink, header_sink)
        self.run_tb(payload_sink, total_payload_len, header_sink, header_len*n_bursts)
        self.assertEqual(header_sink.data(), tuple([1]*header_len*n_bursts))
        self.assertEqual(payload_sink.data(), tuple([2]*total_payload_len))

if __name__ == '__main__':
    gr_unittest.run(qa_header_payload_demux, "qa_header_payload_demux.xml")

