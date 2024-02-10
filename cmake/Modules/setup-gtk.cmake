# Setup variables + functions required to use GTK on Windows due
# to how difficult GTK makes consuming their binaries (also a lack of RPath)

set(GR_BUNDLE_PYTHON "..\\Python${PYTHON_SHORT_VER}\\python.exe")
if(NOT GTK_BIN_DIR)
    # set GVS Build default install location for the GTK
    # ideally this is set by the user
    set(GTK_BIN_DIR "C:/gtk-build/gtk/x64/release/bin")
endif()
get_filename_component(GTK_ROOT ${GTK_BIN_DIR} DIRECTORY)
set(GTK_REL_BIN_DIR "../Python${PYTHON_SHORT_VER}/Lib/site-packages/gi/gtk/bin")
if(WIN32)
    set(DLL_IMPORT_GTK "os.add_dll_directory(os.path.abspath(\"${GTK_REL_BIN_DIR}\"))")
    set(CMAKE_IMPORT_GTK "import os;os.add_dll_directory(\"${GTK_BIN_DIR}\");")
endif()