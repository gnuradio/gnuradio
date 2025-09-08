
# Download, setup and install a copy of Python
if(APPLE)
  set(py_minor_patch 11.4)
elseif(WIN32)
  set(py_minor_patch 12.10)
else()
  set(py_minor_patch 12.10)
endif()
set(PYTHON_VERSION "3.${py_minor_patch}" CACHE STRING "Version of Python to build." FORCE)
# Use official Python.org full distribution for Windows (includes tkinter and full standard library)
# Keep ParaView distribution for other platforms
set(PYTHON_DIR "Python-${PYTHON_VERSION}")
if(WIN32)
  # Use Python.org full distribution (includes tkinter, pip, complete standard library)
  set(PY_DOWNLOAD_LINK "https://www.python.org/ftp/python/${PYTHON_VERSION}/")
  set(PY_FILENAME "python-${PYTHON_VERSION}-amd64.zip")
  set(PYTHON_PLAT_EXT "-amd64")
elseif(APPLE)
  # Keep using Paraview's python build
  set(PY_DOWNLOAD_LINK "https://www.paraview.org/files/dependencies/python-for-wheels/")
  # This says arm64, but the bianry bundled is a macosx universal binary
  # so this will run on x86_64 as well
  set(PY_FILENAME "python-${PYTHON_VERSION}-macos-arm64.tar.xz")
  set(PYTHON_PLAT_EXT "-macos-arm64")
endif()

if(APPLE)
  set(PY_DOWNLOAD_HASH fdb882b53b18675811ca04ad6f5279a586f6f705abd6971c7732c49d214e91b9)
elseif(WIN32)
  set(PY_DOWNLOAD_HASH 8649692DE846C56A7189D6DAE5C322AB20DEB1B5908B6F39426B62A36F39415D)
endif()
# download python
file(DOWNLOAD "${PY_DOWNLOAD_LINK}/${PY_FILENAME}"
  "${CMAKE_CURRENT_BINARY_DIR}/${PY_FILENAME}"
  STATUS download_status
  EXPECTED_HASH "SHA256=${PY_DOWNLOAD_HASH}"
)
list(GET download_status 0 res)
if(res)
  list(GET download_status 1 err)
  message(FATAL_ERROR "Failed to download ${PY_FILENAME} ${err}")
endif()
message(STATUS "Successfully downloaded ${PY_FILENAME}")

# extract
if(WIN32)
  # Python.org full distribution extracts files directly, so create target directory first
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_DIR}${PYTHON_PLAT_EXT}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz
    "${CMAKE_CURRENT_BINARY_DIR}/${PY_FILENAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_DIR}${PYTHON_PLAT_EXT}"
    RESULT_VARIABLE res)
else()
  # ParaView distributions create their own subdirectory structure
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz
    "${CMAKE_CURRENT_BINARY_DIR}/${PY_FILENAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    RESULT_VARIABLE res)
endif()
if(NOT res EQUAL 0)
message(FATAL_ERROR "Extraction of ${PY_FILENAME} failed.")
endif()
message(STATUS "Extracted ${PY_FILENAME}.")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_DIR}${PYTHON_PLAT_EXT}/ DESTINATION Python${PYTHON_SHORT_VER} COMPONENT pythonapi)

if(WIN32)
  configure_file(${PROJECT_SOURCE_DIR}/release/win32/bundle.wxs.in ${CMAKE_BINARY_DIR}/bundle.wxs)
endif()
# install python module dependencies
get_property(GR_PYTHON_DEPS_SET GLOBAL PROPERTY GR_PYTHON_VENDOR_DEPS SET)
if(GR_PYTHON_DEPS_SET AND ENABLE_PYTHON)
  get_property(GR_PYTHON_DEPS GLOBAL PROPERTY GR_PYTHON_VENDOR_DEPS)
  foreach(module ${GR_PYTHON_DEPS})
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                    "import importlib
import inspect
import os
${CMAKE_IMPORT_GTK}
mod = importlib.import_module(\"${module}\")
print(os.path.dirname(inspect.getfile(mod)))
"
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_VARIABLE MODULE_DIR
    )
    file(TO_CMAKE_PATH ${MODULE_DIR} MODULE_DIR)
    message(STATUS "Installing ${module} to ${GR_PYTHON_DIR}/${module}")
    install(DIRECTORY ${MODULE_DIR} DESTINATION ${GR_PYTHON_DIR} COMPONENT pythonapi)
    if(${module} STREQUAL "gi" AND WIN32)
      # Gi requires the GTK c++ binaries to function, install them with the python module
      # for a self contained, relocatable Python${PYTHON_SHORT_VER} distro
      message(STATUS "Installing ${GTK_ROOT} to ${GR_PYTHON_DIR}/${module}")
      install(DIRECTORY ${GTK_ROOT}/ DESTINATION ${GR_PYTHON_DIR}/${module}/gtk/ COMPONENT pythonapi)
    endif()
  endforeach()
  # Need to install numpy.libs as well as numpy
  execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
  "import site
import os
print(os.path.join(site.getsitepackages()[1], \"numpy.libs\"))
"
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE NUMPY_LIB_DIR
  )
  if(NUMPY_LIB_DIR)
    file(TO_CMAKE_PATH ${NUMPY_LIB_DIR} NUMPY_LIB_DIR)
    install(DIRECTORY ${NUMPY_LIB_DIR} DESTINATION ${GR_PYTHON_DIR} COMPONENT pythonapi)
  endif()
endif()


configure_file(${CMAKE_SOURCE_DIR}/release/resources/make_py_link.py ${CMAKE_BINARY_DIR}/make_py_link.py)
# We also want to install a symlink of our custom Python to the bin dir
install(CODE [=[
execute_process(
  COMMAND ${PYTHON_EXECUTABLE}
    ${CMAKE_BINARY_DIR}/make_py_link.py
  WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
  )
]=])