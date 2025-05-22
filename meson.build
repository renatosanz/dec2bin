project(
  'dec2bin',
  'c',
  version: '0.1',
  license: 'GPL-3.0-or-later',
  default_options: [
    'warning_level=2',
    'werror=false',
    'c_std=gnu11',
  ],
)

# Directorios
src_dir = 'src'
data_dir = 'data'
include_dir = include_directories('include')

# Dependencias
gtk4 = dependency('gtk4')
gnome = import('gnome')
libadwaita = dependency('libadwaita-1')
m = meson.get_compiler('c').find_library('m', required: false)

# Fuentes
sources = run_command(
  'find', src_dir, '-name', '*.c', 
  check: true
).stdout().strip().split('\n')

# Archivos Blueprint
blueprint_files = run_command(
  'find', 'data/ui', '-name', '*.blp', 
  check: true
).stdout().strip().split('\n')

blueprints = custom_target('blueprints',
  input: files(blueprint_files),
  output: '.',
  command: [
    find_program('blueprint-compiler'),
    'batch-compile',
    '@OUTPUT@',
    '@CURRENT_SOURCE_DIR@',
    '@INPUT@'
  ],
)

# Recursos
resources_xml = files(data_dir / 'resources.gresource.xml')
resources = gnome.compile_resources(
  'resources',
  resources_xml,
  dependencies: blueprints,
  source_dir: data_dir,
  c_name: 'resources'
)

# Binario principal
executable(
  'dec2bin',
  sources,
  resources,
  include_directories: include_dir,
  dependencies: [gtk4, libadwaita, m],
  install: true,
  install_dir: get_option('bindir')
)

# Instalar iconos en múltiples tamaños (opcional pero recomendado)
icon_sizes = ['16x16', '24x24', '32x32', '48x48', '256x256', 'scalable']
# foreach size : icon_sizes
  install_data(
    'data/icons/dec2bin.png',
    install_dir: join_paths(get_option('datadir'), 'icons/hicolor/48x48/apps'),
    rename: 'dec2bin.png'
  )
# endforeach

# Instalar archivo .desktop
install_data(
  'data/dec2bin.desktop',
  install_dir: join_paths(get_option('datadir'), 'applications')
)

# # Instalar metainfo (opcional pero recomendado para Flathub)
# if get_option('metainfo')
#   install_data(
#     'data/org.example.dec2bin.metainfo.xml',
#     install_dir: join_paths(get_option('datadir'), 'metainfo')
#   )
# endif
