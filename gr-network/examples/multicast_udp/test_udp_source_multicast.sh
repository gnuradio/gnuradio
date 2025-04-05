#!/usr/bin/bash
ADDRESS=239.0.0.10
PORT=50500
# Run in the background, exit after 5 s
./test_udp_source_multicast.py --address $ADDRESS --port $PORT < <(sleep 5) >/dev/null &
# Allow the GNU Radio runtime some time to start
sleep 2
# Send some data
head --bytes 8k </dev/urandom >in.bin
nc -q5 -u $ADDRESS $PORT <in.bin
# Check that we got the same data back
sha256sum in.bin out.bin
