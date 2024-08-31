# Setup variables + functions required to use GTK on Windows due
# to how difficult GTK makes consuming their binaries (also a lack of RPath)

set(GR_BUNDLE_PYTHON "..\\Python311\\python.exe")
set(GTK_BIN_DIR "C:/dev/gnu-radio/deps/gtk/bin")
set(GTK_ROOT "C:/dev/gnu-radio/deps/gtk")
set(GTK_REL_BIN_DIR "../Python311/Lib/site-packages/gi/gtk/bin")
set(DLL_IMPORT_GTK "os.add_dll_directory(os.path.abspath(\"${GTK_REL_BIN_DIR}\"))")
