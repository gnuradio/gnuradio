##################################################
# XMLRPC example
##################################################

XMLRPC allows software to make remote function calls over http. 
In the case of GRC, one can use XMLRPC to modify variables in a running flow graph.
See http://www.xmlrpc.com/

--- Server Example ---
Place an "XMLRPC Server" block inside of any flow graph. 
The server will provide set functions for every variable in the flow graph.
If a variable is called "freq", the server will provide a function set_freq(new_freq).
Run the server example and experiment with the example client script.

-- Client Example --
The "XMLRPC Client" block will give a variable control over one remove function.
In the example client, there is one client block and gui control per variable.
This technique can be used to remotely control a flow graph, perhaps running on a non-gui machine.
