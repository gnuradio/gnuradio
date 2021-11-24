#!/bin/bash

#############################################################################
#  Perform python formatting on the GNU Radio source tree
#  Requires:
#    - autopep8 
#    - pycodestyle
#
#  Usage:
#    sh autoformat_python.sh /path/to/module/
#  Example
#    sh check_pep8.sh ./gr-digital
#    sh check_pep8.sh . # to check the entire source tree
#
#  WARNING:
#    autopep8 can be a bit aggressive - it particularly messes with __init__.py
#     files because of some of the import ordering
#############################################################################

if [ $# -lt 1 ]; then
    echo The number of arguments provided is incorrect. Please provide the path \
    to the module to be formatted.
    exit 1
fi

echo "WARNING: this script calls autopep8 which can cause issues in __init__py especially"

TARGET=$1  

autopep8 -r -i ./$TARGET --select E101,E11,E121,E122,E123,E124,E125,E126,E127,\
E128,E129,E131,E133,E20,E211,E22,E224,E225,E226,E227,E228,E231,E241,E242,E251,\
E252,E26,E265,E266,E27,E301,E302,E303,E304,E305,E306,E401,E402,E501,E502,E701,\
E70,E711,E712,E713,E714,E721,E722,E731,W291,W292,W293,W391,W503,W601,W602,W603,\
W604,W690

find ./$TARGET -iname *.py | xargs pycodestyle --max-line-length=120 --ignore \
E265,E266,E402,E501,E704,E712,E713,E714,E711,E722,E741,W504,W605