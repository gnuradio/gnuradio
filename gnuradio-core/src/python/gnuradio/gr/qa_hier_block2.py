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

    def test_027a_internally_unconnected_input(self):
        tb = gr.top_block()
        hb = gr.hier_block2("block",
                            gr.io_signature(1, 1, 1),
                            gr.io_signature(1, 1, 1))
        hsrc = gr.vector_source_b([1,])
        hb.connect(hsrc, hb) # wire output internally
        src = gr.vector_source_b([1, ])
        dst = gr.vector_sink_b()
        tb.connect(src, hb, dst) # hb's input is not connected internally
        self.assertRaises(RuntimeError, 
                          lambda: tb.run())

    def test_027b_internally_unconnected_output(self):
        tb = gr.top_block()

        hb = gr.hier_block2("block",
                            gr.io_signature(1, 1, 1),
                            gr.io_signature(1, 1, 1))
        hdst = gr.vector_sink_b()
        hb.connect(hb, hdst) # wire input internally
        src = gr.vector_source_b([1, ])
        dst = gr.vector_sink_b()
        tb.connect(src, hb, dst) # hb's output is not connected internally
        self.assertRaises(RuntimeError, 
                          lambda: tb.run())

    def test_027c_fully_unconnected_output(self):
        tb = gr.top_block()
        hb = gr.hier_block2("block",
                            gr.io_signature(1, 1, 1),
                            gr.io_signature(1, 1, 1))
        hsrc = gr.vector_sink_b()
        hb.connect(hb, hsrc) # wire input internally
        src = gr.vector_source_b([1, ])
        dst = gr.vector_sink_b()
        tb.connect(src, hb) # hb's output is not connected internally or externally
        self.assertRaises(RuntimeError, 
                          lambda: tb.run())

    def test_027d_fully_unconnected_input(self):
        tb = gr.top_block()
        hb = gr.hier_block2("block",
                            gr.io_signature(1, 1, 1),
                            gr.io_signature(1, 1, 1))
        hdst = gr.vector_source_b([1,])
        hb.connect(hdst, hb) # wire output internally
        dst = gr.vector_sink_b()
        tb.connect(hb, dst) # hb's input is not connected internally or externally
        self.assertRaises(RuntimeError, 
                          lambda: tb.run())

    def test_028_singleton_reconfigure(self):
        tb = gr.top_block()
        hb = gr.hier_block2("block", 
                            gr.io_signature(0, 0, 0), gr.io_signature(0, 0, 0))
        src = gr.vector_source_b([1, ])
        dst = gr.vector_sink_b()
        hb.connect(src, dst)
        tb.connect(hb) # Singleton connect
        tb.lock()
        tb.disconnect_all()
        tb.connect(src, dst)
        tb.unlock()

    def test_029_singleton_disconnect(self):
        tb = gr.top_block()
        src = gr.vector_source_b([1, ])
        dst = gr.vector_sink_b()
        tb.connect(src, dst)
        tb.disconnect(src)   # Singleton disconnect
        tb.connect(src, dst)
        tb.run()
        self.assertEquals(dst.data(), (1,))

    def test_030_nested_input(self):
        tb = gr.top_block()
        src = gr.vector_source_b([1,])
        hb1 = gr.hier_block2("hb1",
                             gr.io_signature(1, 1, gr.sizeof_char),
                             gr.io_signature(0, 0, 0))
        hb2 = gr.hier_block2("hb2",
                             gr.io_signature(1, 1, gr.sizeof_char),
                             gr.io_signature(0, 0, 0))
        dst = gr.vector_sink_b()
        tb.connect(src, hb1)
        hb1.connect(hb1, hb2)
        hb2.connect(hb2, gr.kludge_copy(gr.sizeof_char), dst)
        tb.run()
        self.assertEquals(dst.data(), (1,))

    def test_031_multiple_internal_inputs(self):
        tb = gr.top_block()
        src = gr.vector_source_f([1.0,])
        hb = gr.hier_block2("hb",
                            gr.io_signature(1, 1, gr.sizeof_float),
                            gr.io_signature(1, 1, gr.sizeof_float))
        m1 = gr.multiply_const_ff(1.0)
        m2 = gr.multiply_const_ff(2.0)
        add = gr.add_ff()
        hb.connect(hb, m1)       # m1 is connected to hb external input #0
        hb.connect(hb, m2)       # m2 is also connected to hb external input #0
        hb.connect(m1, (add, 0)) 
        hb.connect(m2, (add, 1))
        hb.connect(add, hb)      # add is connected to hb external output #0
        dst = gr.vector_sink_f()
        tb.connect(src, hb, dst)
        tb.run()
        self.assertEquals(dst.data(), (3.0,))

    def test_032_nested_multiple_internal_inputs(self):
        tb = gr.top_block()
        src = gr.vector_source_f([1.0,])
        hb = gr.hier_block2("hb",
                            gr.io_signature(1, 1, gr.sizeof_float),
                            gr.io_signature(1, 1, gr.sizeof_float))
        hb2 = gr.hier_block2("hb",
                            gr.io_signature(1, 1, gr.sizeof_float),
                            gr.io_signature(1, 1, gr.sizeof_float))

        m1 = gr.multiply_const_ff(1.0)
        m2 = gr.multiply_const_ff(2.0)
        add = gr.add_ff()
        hb2.connect(hb2, m1)       # m1 is connected to hb2 external input #0
        hb2.connect(hb2, m2)       # m2 is also connected to hb2 external input #0
        hb2.connect(m1, (add, 0)) 
        hb2.connect(m2, (add, 1))
        hb2.connect(add, hb2)      # add is connected to hb2 external output #0
        hb.connect(hb, hb2, hb)   # hb as hb2 as nested internal block
        dst = gr.vector_sink_f()
        tb.connect(src, hb, dst)
        tb.run()
        self.assertEquals(dst.data(), (3.0,))
        
    
if __name__ == "__main__":
    gr_unittest.run(test_hier_block2, "test_hier_block2.xml")
