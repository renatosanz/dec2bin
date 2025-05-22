# dec2bin
Simple number base converter (decimal, binary, octal, hexadecimal) written in C with GTK4.

## Compiling
Just ensure install `gtk4`, [blueprint-compiler](https://gnome.pages.gitlab.gnome.org/blueprint-compiler/), `meson` and `gcc`, then run:

```bash
meson compile -C build && build/dec2bin                                                
```

## Install
Just give executable permission to `install.sh` and run it:

```bash
chmod +x install.sh
./install.sh
```

![dec2bin preview image](preview.png)
