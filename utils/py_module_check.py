from packaging import version
import argparse

def argParse():
    """Parses commandline args."""
    desc = 'Compares python module version'
    parser = argparse.ArgumentParser(description=desc)

    parser.add_argument("module")
    parser.add_argument("min_version", nargs='?', default=None)

    return parser.parse_args()


def main():
    args = argParse()

    m = __import__(args.module)
    if args.min_version:
        v = m.__version__

        if version.parse(v) < version.parse(args.min_version[0]):
            raise ValueError(f'Min version of {args.module} not met, {v} < {args.min_version}')


if __name__ == "__main__":
    main()
