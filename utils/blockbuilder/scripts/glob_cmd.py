#!/usr/bin/env python3

import argparse
from glob import glob
import os

def argParse():
    """Parses commandline args."""
    desc='script for wrapping glob'
    parser = argparse.ArgumentParser(description=desc)
    
    parser.add_argument("--pattern")
    parser.add_argument("--dir")

    return parser.parse_args()

def main():
    args = argParse()
    pattern = os.path.join(args.dir, args.pattern)
    print(pattern)
    files = glob(pattern)
    for f in files:
        print(f)

if __name__ == "__main__":
    main()
