set(GOBJECT_URL https://gitlab.gnome.org/GNOME/gobject-introspection/-/archive/1.80.0/gobject-introspection-1.80.0.tar.gz)
set(GOBJECT_HASH 0af99a064cd90475ade08fbad67ccaef1dd2269e75ff6a3170f6ddf3fae7440e)
set(GOBJECT_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/gobject-introspection)

ExternalProject_Add(
    gnu-radio-gobject-introspection
    URL ${GOBJECT_URL}
    URL_HASH SHA256=${GOBJECT_HASH}
    PREFIX ${CMAKE_BINARY_DIR}/deps
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DCMAKE_INSTALL_PREFIX=${VOLK_INSTALL_DIR}
)
