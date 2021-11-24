#!/bin/bash

#############################################################################
#  Check python formatting on the GNU Radio source tree
#  Requires:
#    - pycodestyle
#
#  Usage:
#    sh autoformat_python.sh /path/to/module/
#  Example
#    sh check_pep8.sh ./gr-digital
#    sh check_pep8.sh . # to check the entire source tree
#############################################################################

if [ $# -lt 1 ]; then
    echo The number of arguments provided is incorrect. Please provide the path \
    to the module to be formatted.
    exit 1
fi

TARGET=$1 

find ./$TARGET -iname *.py | xargs pycodestyle --max-line-length=120 --ignore \
E265,E266,E402,E501,E704,E712,E713,E714,E711,E722,E741,W504,W605