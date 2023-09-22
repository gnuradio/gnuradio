POLAR Code Python test functions module
===========

This directory contains all the necessary files for POLAR code testcode and channel construction.

Test code
====
The test code serves as a reference for the C++ implementations.
'common', 'encoder' and 'decoder' are part of this test code.
'testbed' exists to play with this test code.


Channel Construction
=====
'polar_channel_construction' exposes functionality to calculate polar channels for different sizes.
It may be used to calculate Bhattacharyya parameters once and store them in a file in '~/.gnuradio/polar'.
Frozen bit positions are recalculated on every run.

'polar_channel_construction' provides a command-line interface for all the channel construction code.
These features are also accessible via the Polar Configurator block in GRC.

BEC
====
BEC channel construction can be calculated explicitly because the BEC represents a special case which simplifies the task.
All functionality regarding this channel is located in 'channel_construction_bec'.

AWGN
====
In general channel construction for polar codes is a very time consuming task.
Tal and Vardy proposed a quantization algorithm for channel construction which makes it feasible.
The corresponding implementation is located in 'channel_construction_awgn'.
It should be noted that this more of a baseline implementation which lacks all the advanced features the original implementation provides.
However, simulations show that BEC channel construction with a design-SNR of 0.0dB yields similar performance and should be preferred here.