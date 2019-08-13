# - Config file for the libiio package
# It defines the following variables
#  IIO_INCLUDE_DIRS - include directories for FooBar
#  IIO_LIBRARIES    - libraries to link against
#  IIO_FOUND     - system has libad9361 installed

find_library(IIO_LIBRARIES iio)
find_path(IIO_INCLUDE_DIRS iio.h)
if(IIO_LIBRARIES)
	set(IIO_FOUND TRUE)
endif(IIO_LIBRARIES)
