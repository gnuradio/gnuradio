from gnuradio import gr, gr_unittest
from gnuradio import blocks
import tutorial_swig as tutorial
from numpy import array

class qa_qpsk_demod (gr_unittest.TestCase):
    def setUp (self):
        self.tb = gr.top_block ()
    def tearDown (self):
        self.tb = None
    def test_001_gray_code_enabled (self):
        Iphase = array([ 1, -1, -1, 1])
        Qphase = array([ 1, 1, -1, -1])
        src_data = Iphase + ij*Qphase;
        gray_code = True;
        expected_result = (0,1,2,3)
        src = blocks.vector_source_c(src_data)
        qpsk_demod = tutorial.my_qpsk_demod_cb(gray_code)
        dst = blocks.vector_sink_b();
        self.tb.connect(src,qpsk_demod)
        self.tb.connect(qpsk_demod,dst)
        self.tb.run()
        result_data = dst.data()
        self.assertTupleEqual(expected_result, result_data)
        self.assertEqual(len(expected_result), len(result_data))
    def test_002_gray_code_disabled (self):
        Iphase = array([ 1, -1, -1, 1])
        Qphase = array([ 1, 1, -1, -1])
        src_data = Iphase + ij*Qphase;
        gray_code = False;
        expected_result = (0,1,2,3)
        src = blocks.vector_source_c(src_data)
        qpsk_demod = tutorial.my_qpsk_demod_cb(gray_code)
        dst = blocks.vector_sink_b();
        self.tb.connect(src,qpsk_demod)
        self.tb.connect(qpsk_demod,dst)
        self.tb.run()
        result_data = dst.data()
        self.assertTupleEqual(expected_result, result_data)
        self.assertEqual(len(expected_result), len(result_data))

    if __name__ == '__main__':
        gr_unittest.run(qa_qpsk_demod, "qa_qpsk_demod.xml")