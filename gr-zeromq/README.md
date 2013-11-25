gr-zmqblocks GNU Radio module
=============================
Provides additional blocks and Python classes to connect GNU Radio flowgraphs
over a network and to perform remote procedure calls.


Build requirements
------------------
In order to build the gr-zmqblocks module you will need to install GNU Radio
(http://gnuradio.org/) and ZeroMQ (http://zeromq.org/) including the C++ and
Python bindings.


How to build
------------
In the gr-zmqblocks folder do

    mkdir build
    cd build
    cmake ../
    make

optional

    make install


How to run the example
----------------------

Assuming that the module has been compiled but not installed, in the
gr-zmqblocks folder do

    cd examples
    ./run_app.sh server

on another terminal or machine

    ./run_app.sh client -s hostname

You can also run a (remote) GUI on any of the two or a third machine for monitoring and control.

    ./run_app.sh gui.py -s servername -c hostname

in doing so the order of starting the scripts is arbitrary. When installing the
module, the run\_app.sh script is of course not needed.


How to use the API
------------------

### PROBE API
Connect a zmq pubsub sink to the block you want to monitor

        self.zmq_probe = zmqblocks.sink_pubsub(gr.sizeof_float,
                                               "tcp://*:5556")

add a probe manager to your Python GUI

        # ZeroMQ
        probe_manager = zmqblocks.probe_manager()
        probe_manager.add_socket("tcp://localhost:5556",
                                      'float32', self.plot_data)

        def plot_data(self,samples):
            [...]

basically creates a watcher thread that calls the call back functions and
unpacks sample data. Now you can use a timer to update the plot, e.g. in PyQt

        update_timer = Qt.QTimer()
        self.connect(update_timer,
                     QtCore.SIGNAL("timeout()"),
                     probe_manager.watcher)
        update_timer.start(30)

### RPC API
Add an rpc manager to your Python app to receive RPCs

        rpc_manager = zmqblocks.rpc_manager()
        rpc_manager.set_reply_socket("tcp://*:6666")
        rpc_manager.add_interface("start_fg",self.start)
        rpc_manager.start_watcher()

to be able to send requests also add one on the other side

        rpc_manager = zmqblocks.rpc_manager()
        rpc_manager.set_request_socket("tcp://localhost:6666")

send a request

        rpc_mganager.request("start_fg")
        rpc_mgr_server.request("set_k",gain)

RPCs use GNU Radio pmt's to serialize arguments, the watcher thread will
regularly poll for incoming RPC requests, deserializes arguments and call the
interface callback function accordingly.


Copyright information
------------------
Copyright © 2013 Institute for Theoretical Information Technology,
                 RWTH Aachen University <https://www.ti.rwth-aachen.de/>
Author: Johannes Schmitz <schmitz@ti.rwth-aachen.de>

Unless otherwise stated, all files are distributed under GPL v3 license.
