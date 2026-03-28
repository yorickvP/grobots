# Custom SDL3_ttf port for emscripten
# Adapted from sdl2_ttf.py - Emscripten Authors (MIT license)
# SDL3_ttf is not yet in emscripten's built-in port list as of 4.0.12

import os
import glob

TAG = 'release-3.2.2'
HASH = 'c5f34d1b79492e0341c91687cde9ec315f5d6544c7ebaa7ef5d092e77ccfc363a0e702ba9c43bfa0926c54420843ccfb98b81362985fd7b4a67d09a7852b90ba'
SUBDIR = f'SDL_ttf-{TAG}'

deps = ['freetype', 'sdl3', 'harfbuzz']

variants = {'sdl3_ttf-mt': {'PTHREADS': 1}}


def needed(settings):
  return False  # Only used via --use-port


def get_lib_name(settings):
  return 'libSDL3_ttf' + ('-mt' if settings.PTHREADS else '') + '.a'


def get(ports, settings, shared):
  ports.fetch_project('sdl3_ttf', f'https://github.com/libsdl-org/SDL_ttf/archive/{TAG}.zip', sha512hash=HASH)

  def create(final):
    src_root = ports.get_dir('sdl3_ttf', SUBDIR)
    # Install headers from include/SDL3_ttf/ to SDL3_ttf/ in sysroot
    header_dir = os.path.join(src_root, 'include', 'SDL3_ttf')
    ports.install_headers(header_dir, target='SDL3_ttf')
    flags = [
      '-DTTF_USE_HARFBUZZ=1',
      '-sUSE_SDL=3',
      '-sUSE_FREETYPE=1',
      '-sUSE_HARFBUZZ=1',
      '-I' + os.path.join(src_root, 'include'),
      '-I' + os.path.join(src_root, 'src'),
    ]
    if settings.PTHREADS:
      flags += ['-pthread']
    src_dir = os.path.join(src_root, 'src')
    ports.build_port(src_dir, final, 'sdl3_ttf', flags=flags, srcs=[
      'SDL_ttf.c',
      'SDL_hashtable.c',
      'SDL_hashtable_ttf.c',
      'SDL_renderer_textengine.c',
      'SDL_surface_textengine.c',
    ])

  return [shared.cache.get_lib(get_lib_name(settings), create, what='port')]


def clear(ports, settings, shared):
  shared.cache.erase_lib(get_lib_name(settings))


def process_dependencies(settings, cflags_only=False):
  settings.USE_SDL = 3
  settings.USE_FREETYPE = 1
  settings.USE_HARFBUZZ = 1


def process_args(ports):
  return ['-DTTF_USE_HARFBUZZ=1']


def show():
  return 'sdl3_ttf (custom port; zlib license)'
