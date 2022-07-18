import shutil
import argparse
import os


def argParse():
    """Parses commandline args."""
    desc = 'Copies a file from point A to point B'
    parser = argparse.ArgumentParser(description=desc)

    parser.add_argument("--input_file")
    parser.add_argument("--output_file")
    parser.add_argument("--output_dir", default='')

    return parser.parse_args()


def main():
    args = argParse()

    print(
        f"copying {args.input_file} to {args.output_file} in dir {args.output_dir}")

    # Copy to the expected dir - needed so that ninja doesn't trigger constantly
    outdir = os.path.dirname(args.output_file)
    if not os.path.exists(outdir):
        os.makedirs(outdir)
    shutil.copyfile(args.input_file, args.output_file)

    # # Copy to the include dir
    # shutil.copyfile(args.input_file, os.path.join(
    #     args.output_dir, os.path.basename(args.output_file)))

if __name__ == "__main__":
    main()
