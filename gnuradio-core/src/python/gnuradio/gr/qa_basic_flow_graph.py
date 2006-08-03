#!/usr/bin/env python

from gnuradio import gr, gr_unittest


# ----------------------------------------------------------------                         


class test_basic_flow_graph (gr_unittest.TestCase):

    def setUp (self):
        self.fg = gr.basic_flow_graph ()
        
    def tearDown (self):
        self.fg = None

    def test_000_create_delete (self):
        pass

    def test_001a_insert_1 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect (gr.endpoint (src1, 0), gr.endpoint (dst1, 0))

    def test_001b_insert_1 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect (src1, gr.endpoint (dst1, 0))

    def test_001c_insert_1 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect (gr.endpoint (src1, 0), dst1)

    def test_001d_insert_1 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect (src1, dst1)

    def test_001e_insert_1 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect ((src1, 0), (dst1, 0))

    def test_002_dst_in_use (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        src2 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect ((src1, 0), (dst1, 0))
        self.assertRaises (ValueError,
                           lambda : fg.connect ((src2, 0),
                                                (dst1, 0)))

    def test_003_no_such_src_port (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        self.assertRaises (ValueError,
                           lambda : fg.connect ((src1, 1),
                                                (dst1, 0)))

    def test_004_no_such_dst_port (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        self.assertRaises (ValueError,
                           lambda : fg.connect ((src1, 0),
                                                (dst1, 1)))

    def test_005_one_src_two_dst (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        dst2 = gr.null_sink (gr.sizeof_int)
        fg.connect ((src1, 0), (dst1, 0))
        fg.connect ((src1, 0), (dst2, 0))

    def test_006_check_item_sizes (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_char)
        self.assertRaises (ValueError,
                           lambda : fg.connect ((src1, 0),
                                                (dst1, 0)))
        
    def test_007_validate (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        dst2 = gr.null_sink (gr.sizeof_int)
        fg.connect ((src1, 0), (dst1, 0))
        fg.connect ((src1, 0), (dst2, 0))
        fg.validate ()

    def test_008_validate (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        dst2 = gr.null_sink (gr.sizeof_int)
        fg.connect ((src1, 0), (nop1, 0))
        fg.connect ((src1, 0), (nop1, 1))
        fg.connect ((nop1, 0), (dst1, 0))
        fg.connect ((nop1, 1), (dst2, 0))
        fg.validate ()

    def test_009_validate (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        dst2 = gr.null_sink (gr.sizeof_int)
        fg.connect ((src1, 0), (nop1, 0))
        fg.connect ((src1, 0), (nop1, 2))
        fg.connect ((nop1, 0), (dst1, 0))
        fg.connect ((nop1, 1), (dst2, 0))
        self.assertRaises (ValueError,
                           lambda : fg.validate ())


    def test_010_validate (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        dst2 = gr.null_sink (gr.sizeof_int)
        fg.connect ((src1, 0), (nop1, 0))
        fg.connect ((src1, 0), (nop1, 1))
        fg.connect ((nop1, 0), (dst1, 0))
        fg.connect ((nop1, 2), (dst2, 0))
        self.assertRaises (ValueError,
                           lambda : fg.validate ())

        
    def test_011_disconnect (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        dst2 = gr.null_sink (gr.sizeof_int)
        fg.connect ((src1, 0), (nop1, 0))
        fg.connect ((src1, 0), (nop1, 1))
        fg.connect ((nop1, 0), (dst1, 0))
        fg.connect ((nop1, 1), (dst2, 0))
        fg.validate ()
        fg.disconnect ((src1, 0), (nop1, 1))
        fg.validate ()
        self.assertRaises (ValueError,
                           lambda : fg.disconnect ((src1, 0),
                                                   (nop1, 1)))

    def test_012_connect (self):
        fg = self.fg
        src_data = (0, 1, 2, 3)
        expected_result = (2, 3, 4, 5)
        src1 = gr.vector_source_i (src_data)
        op = gr.add_const_ii (2)
        dst1 = gr.vector_sink_i ()
        fg.connect (src1, op)
        fg.connect (op, dst1)
        # print "edge_list:", fg.edge_list
        fg.validate ()

    def test_013_connect_varargs (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        self.assertRaises (ValueError,
                           lambda : fg.connect ())
        self.assertRaises (ValueError,
                           lambda : fg.connect (src1))

    def test_014_connect_varargs (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect (src1, nop1, dst1)
        fg.validate ()

if __name__ == '__main__':
    gr_unittest.main ()
    
