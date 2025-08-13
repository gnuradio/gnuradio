set(UHD_VERSION 4.9.0.0) # version is also used in custom install command
set(UHD_URL https://github.com/EttusResearch/uhd/archive/refs/tags/v${UHD_VERSION}-rc1.zip)
set(UHD_HASH d37a095826b76f98660572ab7d651eb8068517d7c80fb4e24fc1470ec241a8b9)
set(UHD_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/uhd)
# Define some optional CMAKE_ARGS
# Solution has the drawback of generating cmake warnings when optional
# arguments evaluate to empty string.
# On Windows disable UHD SIM feature
if(WIN32)
  set(EXTRA_CMAKE_ARGS_SIM -DENABLE_SIM=OFF)
endif(WIN32)
# UHD is using FindPythonInterp to find and identify the python installation.
# This method is depreciated and has limited support for detecting python 
# virtual environments and expects the venv folder name to include an encoded 
# python major minor version (like venv312).
# Common workaround for this, is defining the PYTHON_EXECUTABLE preprocessor 
# variable, if its defined, pass it allow to the external uhd project.
if(DEFINED PYTHON_EXECUTABLE)
  set(EXTRA_CMAKE_ARGS_PYTHON -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE})
endif(DEFINED PYTHON_EXECUTABLE)
# Define value for CMAKE_ARGS as variable to allow variables evaluating to 
# empty string to be ignored (and not treaded as empty cmake args)
set(CMAKE_ARGS -DLIBUSB_INCLUDE_DIRS=${LIBUSB_INCLUDE_DIRS} -DCMAKE_BUILD_TYPE=Release -DLIBUSB_LIBRARIES=${LIBUSB_LIBRARIES} -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DUHD_RELEASE_MODE=ON -DENABLE_EXAMPLES=OFF -DENABLE_TESTS=OFF  -DENABLE_EXTEND_WIN_PACKAGING=OFF -DENABLE_PYTHON_API=ON -DCMAKE_INSTALL_PREFIX=${UHD_INSTALL_DIR} "-DCMAKE_CXX_FLAGS=-DBOOST_ALL_NO_LIB /EHsc" ${EXTRA_CMAKE_ARGS_SIM} ${EXTRA_CMAKE_ARGS_PYTHON})
# Define custom install command to install python module on Windows
if(NOT DEFINED PYTHON_SHORT_VER)
  set(PYTHON_SHORT_VER ${Python_VERSION_MAJOR}${Python_VERSION_MINOR})
endif(NOT DEFINED PYTHON_SHORT_VER)
if(WIN32)
  set(INSTALL_COMMAND INSTALL_COMMAND ninja install && python -m pip install <BINARY_DIR>/python/dist/uhd-${UHD_VERSION}-cp${PYTHON_SHORT_VER}-cp${PYTHON_SHORT_VER}-win_amd64.whl --force --disable-pip-version-check)
endif(WIN32)
include(ExternalProject)
ExternalProject_Add(
    gnu-radio-UHD
    URL ${UHD_URL}
    URL_HASH SHA256=${UHD_HASH}
    PREFIX ${CMAKE_BINARY_DIR}/deps
    CMAKE_ARGS ${CMAKE_ARGS}
    ${INSTALL_COMMAND}
    SOURCE_SUBDIR host
)

add_dependencies(gnu-radio-UHD gnu-radio-libusb)


# -DBoost_INCLUDE_DIR=${Boost_INCLUDE_DIR} -DBoost_LIBRARIES=${Boost_LIBRARIES}