import re
from pathlib import Path
from textwrap import dedent

from gnuradio import gr
from gnuradio.grc.core import platform

filename_bits_to_ignore = [
    "_bb",
    "_bc",
    "_cc",
    "_ff",
    "_fb",
    "_fc",
    "_ccf",
    "_ccc",
    "_ccz",
    "_fff",
    "_blk",
    "_s",
    "_uc",
    "_sb",
    "_bs",
    "_ps",
    "_sp",
    "_ss",
    "_f",
    "_c",
    "_b",
    "_bf",
    "_x",
    "_xx",
]

path_replacements = [
    (" ","_"),
    ("/","_"),
    ("\\","_"),
    ("-","_"),
    ("(",""),
    (")",""),
    ("[",""),
    ("]",""),
    (":",""),
]

def block_to_group_name(block):
    group_name = f"block_{block.__name__}"
    for bit in filename_bits_to_ignore:
        group_name = group_name.removesuffix(bit)

    return group_name

def fix_path_name(f_name):
    for old, new in path_replacements:
        f_name = f_name.replace(old, new)

    return f_name



def get_block_tree():
    model = platform.Platform(
        version=gr.version(),
        version_parts=(gr.major_version(), gr.api_version(), gr.minor_version()),
        prefs=gr.prefs(),
    )
    model.build_library()

    block_tree = {}
    for block in model.blocks.values():
        if block.category:
            category = block.category[:]
            sub_tree = block_tree
            while category:
                current = category.pop(0)
                if current not in sub_tree:
                    sub_tree[current] = {}
                sub_tree = sub_tree[current]
            sub_tree[block.label] = block

    return block_tree


def _make_doxy_groups(tree, file_handle):
    for k, v in tree.items():
        if isinstance(v, dict):
            print(f"{'=' * 20} {k} {'=' * 20}")
            group_name = f"blocks_{k.lower().replace(' ', '_').replace('/', '_')}"
            file_handle.write(rf"/*! \defgroup {group_name} {k}")
            file_handle.write("\n")
            file_handle.write(" * @{\n")
            file_handle.write(" */\n\n")
            _make_doxy_groups(v, file_handle)
            file_handle.write("\n/*! @} */\n\n")
        else:
            print(f"{v}:\t", end="")
            group_name = block_to_group_name(v)
            print(group_name)
            file_handle.write(rf"/*! \defgroup {group_name} {k} */")
            file_handle.write("\n")


def make_doxy_groups(tree):
    top_matter = dedent(
        r"""\
        # Copyright (C) 2017 Free Software Foundation, Inc.
        #
        # Permission is granted to copy, distribute and/or modify this document
        # under the terms of the GNU Free Documentation License, Version 1.3
        # or any later version published by the Free Software Foundation;
        # with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
        # A copy of the license is included in the section entitled "GNU
        # Free Documentation License".

        /*!
        * \defgroup block GNU Radio Blocks
        * \\brief All blocks that can be used in GR graphs are listed here or in
        * the subcategories below.
        *
    """,
    )

    current_dir = Path(__file__)
    doxy_group_defs_path = (
        current_dir / "../../doxygen/other/group_defs.dox"
    ).resolve()
    with open(doxy_group_defs_path, "w") as ff:
        ff.write(top_matter)
        ff.write("* @{ */\n\n")
        _make_doxy_groups(tree["Core"], ff)
        ff.write("\n/*! @} */\n")


def fix_doxy_groups():
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
    paths = [
        (
            Path("../../") / ("gr-" + x) / "include" / "gnuradio" / x.replace("-", "_")
        ).resolve()
        for x in block_mods
    ]

    for path in paths:
        print(f"{'='*10} {path.name} {'='*10}")
        blocks = [p for p in path.iterdir() if p.suffix == ".h" and p.name != "api.h"]
        for block in blocks:
            group_stem = str(block.stem)
            for bit in filename_bits_to_ignore:
                group_stem = group_stem.removesuffix(bit)

            group_name = f"block_{path.name}_{group_stem}"

            with block.open("r") as ff:
                a = ff.read()
                is_block = re.search(r"\ *\*\ *\\ingroup\ *(\w*)", a) is not None

            if is_block:
                print(
                    f"[{path.name}]\tUpdating {block.stem} with group name {group_name}"
                )
                updated = re.sub(r"(\ *\*\ *\\ingroup\ *)(\w*)", rf"\1{group_name}", a)
                with block.open("w") as ff:
                    ff.write(updated)


def _make_rst_files(tree, doc_path):
    for k, v in tree.items():
        if isinstance(v, dict):
            has_children = any(isinstance(v_, dict) for v_ in v.values())
            only_children = all(isinstance(v_, dict) for v_ in v.values())

            path = doc_path / fix_path_name(k)
            path.mkdir(exist_ok=True)
            with open(path / "index.rst", "w") as ff:
                index_file_contents = dedent(
                    f"""\
                    {'-' * len(k)}
                    {k}
                    {'-' * len(k)}

                    .. toctree::
                       :hidden:
                       :glob:

                       {'*/index' if has_children else ''}
                       {'*' if not only_children else ''}
                """,
                )
                ff.write(index_file_contents)
            _make_rst_files(v, path)
        else:

            path = (doc_path / fix_path_name(k)).with_suffix(".rst")
            path.parent.mkdir(parents=True, exist_ok=True)
            wiki_title = k.replace(" ", "_")

            params_table = _make_params_table(tree[k].parameters_data)
            if k == "Python Block":
                params_table = ""

            with open(path, "w") as ff:
                # Multiline f-strings cause weird shenanigans with dedent, so
                # have to use string concatenation
                contents = dedent(
                    f"""\
                    {'-' * len(k)}
                    {k}
                    {'-' * len(k)}

                    :gr-wiki-link:`{wiki_title}`

                    Parameters
                    **********
                """,
                )
                contents += params_table
                contents += dedent(
                    f"""
                    Class Reference
                    *******************

                    .. tabs::
                        .. group-tab:: C++

                            .. doxygengroup:: {block_to_group_name(v)}
                                :content-only:
                                :undoc-members:
                                :private-members:
                                :members:

                        .. group-tab:: Python

                            TODO
                """,
                )

                ff.write(contents)


def _make_params_table(params: list) -> str:
    cell_w = 25 # Minimimum cell width

    for param in params:
        if "$" in param.get("dtype", ""):
            continue
        if param.get("category", "") == "Advanced":
            continue
        a = str(param.get("label", ""))
        b = str(param.get("id", ""))
        c = str(param.get("dtype", ""))
        d = str(param.get("default", ""))


        m = len(max([a, b, c, d], key=len))
        if m > cell_w:
            cell_w = m

    headers = f"+{'-'*cell_w}+{'-'*cell_w}+{'-'*cell_w}+{'-'*cell_w}+{'-'*cell_w}+\n"
    headers += f"|{'Label':<{cell_w}}|{'ID':<{cell_w}}|{'Description':<{cell_w}}|{'Data Type':<{cell_w}}|{'Default':<{cell_w}}|\n"
    headers += f"+{'='*cell_w}+{'='*cell_w}+{'='*cell_w}+{'='*cell_w}+{'='*cell_w}+\n"

    table = [dedent(headers)]


    for param in params:
        if "$" in param.get("dtype", ""):
            continue
        if param.get("category", "") == "Advanced":
            continue

        row = f"|{param.get('label', '')!s:<{cell_w}}"
        row += f"|{param.get('id', '')!s:<{cell_w}}"
        row += f"|{'TODO'!s:<{cell_w}}"
        row += f"|{param.get('dtype', '')!s:<{cell_w}}"
        row += f"|{param.get('default', '')!s:<{cell_w}}|\n"
        row += f"+{'-'*cell_w}+{'-'*cell_w}+{'-'*cell_w}+{'-'*cell_w}+{'-'*cell_w}+\n"

        table += dedent(row)

    return "".join(table)


def make_rst_files(tree):
    current_dir = Path(__file__).parent.resolve()
    block_docs_path = current_dir / "blocks"
    if not block_docs_path.exists():
        block_docs_path.mkdir()
    with open(block_docs_path / "index.rst", "w") as ff:
        block_root_doc = dedent(
            """\
            Built-in Blocks
            ===============

            .. toctree::
            \t:titlesonly:
            \t:glob:

            \t*/index
        """,
        )
        ff.write(block_root_doc)
    _make_rst_files(tree["Core"], block_docs_path)


if __name__ == "__main__":
    block_tree = get_block_tree()
    fix_doxy_groups()
    make_doxy_groups(block_tree)
    make_rst_files(block_tree)
