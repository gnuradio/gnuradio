<p align="center">
<img src="https://github.com/gnuradio/gnuradio/blob/main/docs/gnuradio.png" width="75%" />
</p>

The `dev-4.0` branch is where GR 4.0 development will be taking place.  
It is expected that API and functionality will change rapidly until 4.0 is released

GNU Radio is a free & open-source software development toolkit that 
provides signal processing blocks to implement software radios. It can 
be used with readily-available, low-cost external RF hardware to create 
software-defined radios, or without hardware in a simulation-like 
environment. It is widely used in hobbyist, academic, and commercial 
environments to support both wireless communications research and real-world 
radio systems.

## How to Install GNU Radio 4.0

### From Source

```
git clone https://github.com/gnuradio/gnuradio --branch dev-4.0
cd gnuradio
meson setup build
cd build && ninja 
ninja test
ninja install # (sudo ninja install if not root)
```
to install to a custom prefix:
```
meson setup build --buildtype=debugoptimized --prefix=path/to/prefix --libdir=lib
```

## Legal Matters

Some files have been changed many times throughout the years. Copyright 
notices at the top of source files list which years changes have been 
made. For some files, changes have occurred in many consecutive years. 
These files may often have the format of a year range (e.g., "2006 - 2011"), 
which indicates that these files have had copyrightable changes made 
during each year in the range, inclusive.
