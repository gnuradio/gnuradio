import time

from gnuradio import gr, gr_unittest, blocks

class test_message_tags (gr_unittest.TestCase):

    def test_1 (self):
        data = ('hello', 'you', 'there')
        tx_msgq = gr.msg_queue()
        rx_msgq = gr.msg_queue()
        for d in data:
            tx_msgq.insert_tail(gr.message_from_string(d))
        tx_msgq.insert_tail(gr.message(1))                  # send EOF
        tb = gr.top_block()
        src = blocks.message_source(gr.sizeof_char, tx_msgq, "packet_length")
        snk = blocks.message_sink(gr.sizeof_char, rx_msgq, False, "packet_length")
        tb.connect(src, snk)
        tb.start()
        time.sleep(1)
        tb.stop()
        for d in data:
            msg = rx_msgq.delete_head()
            contents = msg.to_string()
            self.assertEqual(d, contents)

if __name__ == '__main__':
    gr_unittest.run(test_message_tags, "test_message_tags.xml")
