
# Download, setup and install a copy of Python
if(APPLE)
  set(py_minor_patch 11.4)
elseif(WIN32)
  set(py_minor_patch 12.10)
else()
  set(py_minor_patch 12.10)
endif()
set(PYTHON_VERSION "3.${py_minor_patch}" CACHE STRING "Version of Python to build." FORCE)
set(PYTHON_DIR "Python-${PYTHON_VERSION}")
if(WIN32)
  # Use Python.org full distribution (includes tkinter library)
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
if(APPLE)
  # ParaView distributions create their own subdirectory structure
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz
    "${CMAKE_CURRENT_BINARY_DIR}/${PY_FILENAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    RESULT_VARIABLE res)
elseif(WIN32)
  # Python.org full distribution extracts files directly, so create target directory first
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_DIR}${PYTHON_PLAT_EXT}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz
    "${CMAKE_CURRENT_BINARY_DIR}/${PY_FILENAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_DIR}${PYTHON_PLAT_EXT}"
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
                    "import importlib, inspect, os, site, glob
${CMAKE_IMPORT_GTK}
mod = importlib.import_module(\"${module}\")
module_file = inspect.getfile(mod)
module_dir = os.path.dirname(module_file)
is_single_file = os.path.basename(module_file).replace('.py', '') == \"${module}\" or os.path.basename(module_file) == '__init__.py'
# Find module's dist-info folder
dist_info_path = ''
try:
    site_dir = site.getsitepackages()[1]
    if os.path.exists(site_dir):
        module_name = \"${module}\".lower().replace('_', '-').replace('.', '-')
        for dist_dir in glob.glob(os.path.join(site_dir, '*.dist-info')):
            dist_name = os.path.basename(dist_dir).lower()
            if (dist_name.startswith(module_name + '-') or 
                module_name in dist_name):
                dist_info_path = dist_dir
                break
except: pass

# Output module info and dist-info on single line with separator
if (is_single_file and not os.path.basename(module_file) == '__init__.py'):
    # Single file module like six.py
    print('SINGLE_FILE:' + module_file + '|DIST_INFO:' + dist_info_path)
else:
    # Directory module 
    print('DIRECTORY:' + module_dir + '|DIST_INFO:' + dist_info_path)
"
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_VARIABLE MODULE_INFO
    )
    
    # Parse module and dist-info from single line with | separator
    string(FIND "${MODULE_INFO}" "|DIST_INFO:" DIST_INFO_POS)
    if(DIST_INFO_POS GREATER -1)
      string(SUBSTRING "${MODULE_INFO}" 0 ${DIST_INFO_POS} MODULE_PART)
      math(EXPR DIST_INFO_START "${DIST_INFO_POS} + 11")  # Length of "|DIST_INFO:"
      string(SUBSTRING "${MODULE_INFO}" ${DIST_INFO_START} -1 DIST_INFO_DIR)
    else()
      set(MODULE_PART "${MODULE_INFO}")
      set(DIST_INFO_DIR "")
    endif()
    
    # Install module
    if(MODULE_PART MATCHES "^SINGLE_FILE:")
      string(REGEX REPLACE "^SINGLE_FILE:" "" MODULE_FILE "${MODULE_PART}")
      file(TO_CMAKE_PATH ${MODULE_FILE} MODULE_FILE)
      message(STATUS "Installing single-file module ${module} from ${MODULE_FILE}")
      install(FILES ${MODULE_FILE} DESTINATION ${GR_PYTHON_DIR} COMPONENT pythonapi)
    else()
      string(REGEX REPLACE "^DIRECTORY:" "" MODULE_DIR "${MODULE_PART}")
      file(TO_CMAKE_PATH ${MODULE_DIR} MODULE_DIR)
      message(STATUS "Installing directory module ${module} from ${MODULE_DIR}")
      install(DIRECTORY ${MODULE_DIR} DESTINATION ${GR_PYTHON_DIR} COMPONENT pythonapi)
    endif()
    
    # Install dist-info if found
    if(DIST_INFO_DIR AND NOT DIST_INFO_DIR STREQUAL "")
      if(EXISTS "${DIST_INFO_DIR}")
        file(TO_CMAKE_PATH ${DIST_INFO_DIR} DIST_INFO_DIR)
        get_filename_component(dist_name "${DIST_INFO_DIR}" NAME)
        message(STATUS "Installing ${dist_name} for ${module}")
        install(DIRECTORY ${DIST_INFO_DIR} DESTINATION ${GR_PYTHON_DIR} COMPONENT pythonapi)
      endif()
    endif()
    
    if(${module} STREQUAL "gi" AND WIN32)
      # Gi requires the GTK c++ binaries to function, install them with the python module
      # for a self contained, relocatable Python${PYTHON_SHORT_VER} distro
      message(STATUS "Installing ${GTK_ROOT} to ${GR_PYTHON_DIR}/${module}")
      install(DIRECTORY ${GTK_ROOT}/ DESTINATION ${GR_PYTHON_DIR}/${module}/gtk/ COMPONENT pythonapi)
    endif()

    # Need to install numpy.libs as well as numpy
    if(${module} STREQUAL "numpy")
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
        message(STATUS "Installing numpy.libs from ${NUMPY_LIB_DIR}")
        install(DIRECTORY ${NUMPY_LIB_DIR} DESTINATION ${GR_PYTHON_DIR} COMPONENT pythonapi)
      endif()
    endif()

    # Need to install scipy.libs as well as scipy
    if(${module} STREQUAL "scipy")
      execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
      "import site
import os
print(os.path.join(site.getsitepackages()[1], \"scipy.libs\"))
"
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_VARIABLE SCIPY_LIB_DIR
      )
      if(SCIPY_LIB_DIR)
        file(TO_CMAKE_PATH ${SCIPY_LIB_DIR} SCIPY_LIB_DIR)
        message(STATUS "Installing scipy.libs from ${SCIPY_LIB_DIR}")
        install(DIRECTORY ${SCIPY_LIB_DIR} DESTINATION ${GR_PYTHON_DIR} COMPONENT pythonapi)
      endif()
    endif()
  endforeach()
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