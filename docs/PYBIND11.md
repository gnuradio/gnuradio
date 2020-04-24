# Generating/maintaining Python bindings using Pybind11

As of the GNU Radio 3.9 release, python bindings are handled using pybind11,
which is inherently different than they were in previous releases

## Dependencies

- pybind11 > 2.4.3 https://pybind11.readthedocs.io/
- pygccxml https://pygccxml.readthedocs.io/en/develop/install.html
  - This is an optional dependency and basic functionality for OOT generation can be performed without pygccxml

## Workflow

### Out-of-Tree modules

The steps for creating an out of tree module with pybind11 bindings are as follows:

1. Use `gr_modtool` to create an out of tree module and add blocks

```
gr_modtool newmod foo
gr_modtool add bar
```

2. Update the parameters or functions in the public include file and rebind with `gr_modtool bind bar`

**NOTE**: without pygccxml, only the make function is currently accounted for, similar to `gr_modtool makeyaml`

3. Build and install

If the public API changes, just call `gr_modtool bind [blockname]` to regenerate the bindings


### In-Tree Modules

Generating bindings for in-tree modules is currently a bit more manual, as they are not expected to change that frequently.  Pygccxml **IS** required for generating these bindings.

Currently the best way to approach this is via the script `gr-utils/bindtool/scripts/bind_gr_module.py`

```
python3 /path/to/gr-utils/bindtool/scripts/bind_gr_module.py --prefix=[GR PREFIX (e.g. ~/gr)] --output_dir /tmp/bindtool modulename
```

where `modulename` is:

```
gr, pmt, blocks, digital, analog, fft, filter, ...
```


## Docstrings

If Doxygen is enabled in GNU Radio and/or the OOT, Docstrings are scraped from the header files, and placed in auto-generated
`[blockname]_pydoc.h` files in the build directory on compile.  Generated templates (via the binding steps described above) are placed in
the `python/bindings/docstrings` directory and are used as placeholders for the scraped strings

Upon compilation, docstrings are scraped from the module and stored in a dictionary (using `update_pydoc.py scrape`) and then
the values are substituted in the template file (using `update_pydoc.py sub`)