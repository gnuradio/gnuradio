import argparse
import os


def argParse():
    """Parses commandline args."""
    desc = 'Copies a file from point A to point B'
    parser = argparse.ArgumentParser(description=desc)

    parser.add_argument("--dir1")
    parser.add_argument("--dir2")

    return parser.parse_args()


def main():
    args = argParse()
    
    print(os.path.relpath(args.dir1, args.dir2))

if __name__ == "__main__":
    main()
