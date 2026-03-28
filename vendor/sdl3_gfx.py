# Custom SDL3_gfx port for emscripten
# SDL3_gfx is not in emscripten's built-in port list

import os

REV = '0bbee988bb0caa3e98a9d78c7a2d106925c8275a'
HASH = '5288362d72b0d4f56344c8445d3ae83e543b903bc3ec3410dcfca9d808fb50dc0c78c0616c2d5e0cd27c6ae128a0a36fa7e3ef33be6395e62726a2c75a6eed1c'
SUBDIR = f'SDL3_gfx-{REV}'

deps = ['sdl3']


def needed(settings):
  return False  # Only used via --use-port


def get_lib_name(settings):
  return 'libSDL3_gfx.a'


def get(ports, settings, shared):
  ports.fetch_project('sdl3_gfx', f'https://github.com/sabdul-khabir/SDL3_gfx/archive/{REV}.zip', sha512hash=HASH)

  def create(final):
    src_root = ports.get_dir('sdl3_gfx', SUBDIR)
    # Headers are at the repo root, install to SDL3_gfx/ in sysroot
    ports.install_headers(src_root, target='SDL3_gfx')
    flags = [
      '-sUSE_SDL=3',
      '-I' + src_root,
    ]
    ports.build_port(src_root, final, 'sdl3_gfx', flags=flags, srcs=[
      'SDL3_gfxPrimitives.c',
      'SDL3_rotozoom.c',
      'SDL3_framerate.c',
      'SDL3_imageFilter.c',
    ])

  return [shared.cache.get_lib(get_lib_name(settings), create, what='port')]


def clear(ports, settings, shared):
  shared.cache.erase_lib(get_lib_name(settings))


def process_dependencies(settings, cflags_only=False):
  settings.USE_SDL = 3


def process_args(ports):
  return []


def show():
  return 'sdl3_gfx (custom port; zlib license)'
