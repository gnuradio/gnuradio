#!/usr/bin/env python

import sys
import os, os.path
from optparse import OptionParser

def quote_line (line):
    line = line.replace ('&', '&amp;')
    line = line.replace ('<', '&lt;')
    line = line.replace ('>', '&gt;')
    line = line.replace ("'", '&apos;')
    line = line.replace ('"', '&quot;')
    return line

def generate_listing (input_filename, title=None):
    inf = open (input_filename, "r")
    output_filename = os.path.basename (input_filename) + '.xml'
    outf = open (output_filename, "w")
    outf.write ('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
    # outf.write ('<example id="%s">\n' % (input_filename,))
    # if not title:
    #     title = input_filename
    # outf.write ('<title>')
    # outf.write (title)
    # outf.write ('</title>\n')
    outf.write ('<programlisting>\n');

    lineno = 0
    for line in inf:
        line = line.expandtabs (8)
        line = quote_line (line)
        lineno = lineno + 1
        outf.write ('%3d  %s' % (lineno, line))

    outf.write ('</programlisting>\n')
    # outf.write ('</example>\n')


def main ():
    for file in sys.argv[1:]:
        generate_listing (file)

if __name__ == '__main__':
    main ()
    
