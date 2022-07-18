#!/usr/bin/env python3

from gnuradio import gr_unittest, gr, blocks, math, streamops
from time import time, sleep

class test_basic(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.flowgraph()
        self.rt = gr.runtime()

    def tearDown(self):
        self.tb = None
        self.rt = None

    def test_basic(self):
        run_time = 1

        src = blocks.vector_source_f([1,2,3,4,5],True)
        throttle = streamops.throttle(32000)
        mult1 = math.multiply_const_ff(100.0)
        snk1 = blocks.vector_sink_f()
        snk2 = blocks.vector_sink_f()

        self.tb.connect([src, throttle, mult1, snk1])
        self.tb.connect(mult1,snk2)
        
        self.rt.initialize(self.tb)
        self.rt.start()

        start = time()
        k = 1.0

        k_set = []
        k_queried = []

        while True: 
            mult1.set_k(k)
            k_set.append(k)

            if (time() - start > run_time):
                break

            sleep(0.1)

            k += 1.0

            query_k = mult1.k()
            # print(query_k)

            k_queried.append(query_k)


        sleep(0.25)

        self.rt.stop()

        # Search for the values that we set in the values we queried
        all_values_found = True
        for i in range(len(k_set)-1):
            if not k_set[i] in k_queried:
                all_values_found = False

        self.assertTrue(all_values_found)

        # // now look at the data
        # EXPECT_GT(snk1->data().size(), 5);
        # EXPECT_GT(snk2->data().size(), 5);

        # // TODO - check for query
        # // TODO - set changes at specific sample numbers

if __name__ == "__main__":
    gr_unittest.run(test_basic)