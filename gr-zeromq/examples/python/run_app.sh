#!/bin/sh
export LD_LIBRARY_PATH="$PWD/../../build/gr-zeromq/lib"
export PYTHONPATH="$PWD/../../python"
/usr/bin/python $1 $2 $3 $4 $5 $6 $7 $8 $9
