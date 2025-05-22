meson setup builddir --prefix=/usr
meson compile -C builddir
sudo meson install -C builddir
which dec2bin
