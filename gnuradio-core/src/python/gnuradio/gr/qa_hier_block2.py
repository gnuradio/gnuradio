#!/usr/bin/env python

from gnuradio import gr, gr_unittest

class test_hier_block2(gr_unittest.TestCase):

    def setUp(self):
	pass

    def tearDown(self):
    	pass

    def test_001_make(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	self.assertEqual("test_block", hblock.name())
	self.assertEqual(1, hblock.input_signature().max_streams())
	self.assertEqual(1, hblock.output_signature().min_streams())
	self.assertEqual(1, hblock.output_signature().max_streams())
	self.assertEqual(gr.sizeof_int, hblock.output_signature().sizeof_stream_item(0))

    def test_002_connect_input(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	hblock.connect(hblock, nop1)

    def test_003_connect_input_in_use(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	nop2 = gr.nop(gr.sizeof_int)
	hblock.connect(hblock, nop1)
	self.assertRaises(ValueError,
	    lambda: hblock.connect(hblock, nop2))

    def test_004_connect_output(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	hblock.connect(nop1, hblock)

    def test_005_connect_output_in_use(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	nop2 = gr.nop(gr.sizeof_int)
	hblock.connect(nop1, hblock)
	self.assertRaises(ValueError,
	    lambda: hblock.connect(nop2, hblock))

    def test_006_connect_invalid_src_port_neg(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	self.assertRaises(ValueError, 
	    lambda: hblock.connect((hblock, -1), nop1))

    def test_005_connect_invalid_src_port_exceeds(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	self.assertRaises(ValueError, 
	    lambda: hblock.connect((hblock, 1), nop1))

    def test_007_connect_invalid_dst_port_neg(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	nop2 = gr.nop(gr.sizeof_int)
	self.assertRaises(ValueError, 
	    lambda: hblock.connect(nop1, (nop2, -1)))

    def test_008_connect_invalid_dst_port_exceeds(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.null_sink(gr.sizeof_int)
	nop2 = gr.null_sink(gr.sizeof_int)
	self.assertRaises(ValueError, 
	    lambda: hblock.connect(nop1, (nop2, 1)))

    def test_009_check_topology(self):
	hblock = gr.top_block("test_block")
	hblock.check_topology(0, 0)

    def test_010_run(self):
        expected = (1.0, 2.0, 3.0, 4.0)
        hblock = gr.top_block("test_block")
        src = gr.vector_source_f(expected, False)
        sink1 = gr.vector_sink_f()
        sink2 = gr.vector_sink_f()
        hblock.connect(src, sink1)
        hblock.connect(src, sink2)
        hblock.run()
        actual1 = sink1.data()
        actual2 = sink2.data()
        self.assertEquals(expected, actual1)
        self.assertEquals(expected, actual2)

    def test_012_disconnect_input(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	hblock.connect(hblock, nop1)
        hblock.disconnect(hblock, nop1)
   
    def test_013_disconnect_input_not_connected(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
        nop2 = gr.nop(gr.sizeof_int)
	hblock.connect(hblock, nop1)
        self.assertRaises(ValueError,
            lambda: hblock.disconnect(hblock, nop2))
   
    def test_014_disconnect_input_neg(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	hblock.connect(hblock, nop1)
        self.assertRaises(ValueError,
            lambda: hblock.disconnect((hblock, -1), nop1))

    def test_015_disconnect_input_exceeds(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	hblock.connect(hblock, nop1)
        self.assertRaises(ValueError,
            lambda: hblock.disconnect((hblock, 1), nop1))

    def test_016_disconnect_output(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	hblock.connect(nop1, hblock)
        hblock.disconnect(nop1, hblock)
   
    def test_017_disconnect_output_not_connected(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
        nop2 = gr.nop(gr.sizeof_int)
	hblock.connect(nop1, hblock)
        self.assertRaises(ValueError,
            lambda: hblock.disconnect(nop2, hblock))
   
    def test_018_disconnect_output_neg(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	hblock.connect(hblock, nop1)
        self.assertRaises(ValueError,
            lambda: hblock.disconnect(nop1, (hblock, -1)))

    def test_019_disconnect_output_exceeds(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	hblock.connect(nop1, hblock)
        self.assertRaises(ValueError,
            lambda: hblock.disconnect(nop1, (hblock, 1)))

    def test_020_run(self):
	hblock = gr.top_block("test_block")
	data = (1.0, 2.0, 3.0, 4.0)
	src = gr.vector_source_f(data, False)
	dst = gr.vector_sink_f()
	hblock.connect(src, dst)
	hblock.run()
	self.assertEquals(data, dst.data())

    def test_021_connect_single(self):
        hblock = gr.top_block("test_block")
        blk = gr.hier_block2("block",
                             gr.io_signature(0, 0, 0),
                             gr.io_signature(0, 0, 0))
        hblock.connect(blk)

    def test_022_connect_single_with_ports(self):
        hblock = gr.top_block("test_block")
        blk = gr.hier_block2("block",
                             gr.io_signature(1, 1, 1),
                             gr.io_signature(1, 1, 1))
        self.assertRaises(ValueError,
                          lambda: hblock.connect(blk))

    def test_023_connect_single_twice(self):
        hblock = gr.top_block("test_block")
        blk = gr.hier_block2("block",
                             gr.io_signature(0, 0, 0),
                             gr.io_signature(0, 0, 0))
        hblock.connect(blk)
        self.assertRaises(ValueError,
                          lambda: hblock.connect(blk))

    def test_024_disconnect_single(self):
        hblock = gr.top_block("test_block")
        blk = gr.hier_block2("block",
                             gr.io_signature(0, 0, 0),
                             gr.io_signature(0, 0, 0))
        hblock.connect(blk)
        hblock.disconnect(blk)

    def test_025_disconnect_single_not_connected(self):
        hblock = gr.top_block("test_block")
        blk = gr.hier_block2("block",
                             gr.io_signature(0, 0, 0),
                             gr.io_signature(0, 0, 0))
        self.assertRaises(ValueError,
                          lambda: hblock.disconnect(blk))

    def test_026_run_single(self):
        expected_data = (1.0,)
        tb = gr.top_block("top_block")
        hb = gr.hier_block2("block",
                            gr.io_signature(0, 0, 0),
                            gr.io_signature(0, 0, 0))
        src = gr.vector_source_f(expected_data)
        dst = gr.vector_sink_f()
        hb.connect(src, dst)
        tb.connect(hb)
        tb.run()
        self.assertEquals(expected_data, dst.data())
    
if __name__ == "__main__":
    gr_unittest.main()
