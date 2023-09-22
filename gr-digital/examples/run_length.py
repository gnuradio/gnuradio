#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from optparse import OptionParser
import sys


def main():
    parser = OptionParser()
    parser.add_option("-f", "--file", default=None,
                      help="Choose file to read data from.")
    (options, args) = parser.parse_args()

    if options.file == None:
        print("Must specify file to read from using '-f'.")
        sys.exit(1)
    print("Using", options.file, "for data.")

    f = open(options.file, 'r')
    runs = []
    count = 0
    current = 0
    bytes = 0
    bits = 0

    for ch in f.read():
        x = ord(ch)
        bytes = bytes + 1
        for i in range(7, -1, -1):
            bits = bits + 1
            t = (x >> i) & 0x1
            if t == current:
                count = count + 1
            else:
                if count > 0:
                    if len(runs) < count:
                        for j in range(count - len(runs)):
                            runs.append(0)
                    runs[count - 1] = runs[count - 1] + 1

                current = 1 - current
                count = 1

    # Deal with last run at EOF
    if len(runs) < count and count > 0:
        for j in range(count - len(runs)):
            runs.append(0)
    runs[count - 1] = runs[count - 1] + 1

    chk = 0
    print("Bytes read: ", bytes)
    print("Bits read:  ", bits)
    print()
    for i in range(len(runs)):
        chk = chk + runs[i] * (i + 1)
        print("Runs of length", i + 1, ":", runs[i])
    print()
    print("Sum of runs:", chk, "bits")
    print()
    print("Maximum run length is", len(runs), "bits")


if __name__ == "__main__":
    main()
