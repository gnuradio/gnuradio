import hashlib
import re
from pathlib import Path
from pprint import pprint

block_mods = [
    "analog",
    "audio",
    "blocks",
    "channels",
    "digital",
    "dtv",
    "fec",
    "fft",
    "filter",
    "iio",
    "network",
    "pdu",
    "qtgui",
    "soapy",
    "trellis",
    "uhd",
    "video-sdl",
    "vocoder",
    "wavelet",
    "zeromq",
]
includes_paths = [
    (
        Path("../../") / ("gr-" + x) / "include" / "gnuradio" / x.replace("-", "_")
    ).resolve()
    for x in block_mods
]
bindings_paths = [
    (
        Path("../../") / ("gr-" + x) / "python" / x.replace("-", "_") / "bindings"
    ).resolve()
    for x in block_mods
]


def main() -> None:
    from pprint import pprint

    for binding, include in zip(bindings_paths, includes_paths):
        blocks = sorted(
            [p for p in include.iterdir() if p.suffix == ".h" and p.name != "api.h"]
        )
        actual_blocks = []
        for block in blocks:
            with block.open("r") as ff:
                a = ff.read()
                is_block = re.search(r"\\ingroup", a) is not None
                if is_block:
                    actual_blocks.append(block)

        bindings = sorted(
            [binding / f"{b.stem}_python.cc" for b in actual_blocks],
            key=lambda p: p.stem.removesuffix("_python"),
        )

        for block, bind in zip(actual_blocks, bindings):
            with block.open("rb") as f:
                md5 = hashlib.file_digest(f, "md5").hexdigest()

            try:
                with bind.open("r") as ff:
                    binding_file = ff.read()
            except FileNotFoundError:
                print(f"Failed to find {bind}")
                continue

            updated = re.sub(
                r"(\/\*\s*BINDTOOL_HEADER_FILE_HASH)\(([\w\d]+)(\)\ +\*\/)",
                rf"\1({md5}\3",
                binding_file,
            )
            with bind.open("w") as ff:
                ff.write(updated)


if __name__ == "__main__":
    main()
