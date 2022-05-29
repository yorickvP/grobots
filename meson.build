project('grobots', 'c', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

src = [
  'brains/GBBrain.cpp',
  'brains/GBBrainSpec.cpp',
  'brains/GBStackBrain.cpp',
  'brains/GBStackBrainOpcodes.cpp',
  'brains/GBStackBrainPrimitives.cpp',
  'brains/GBStackBrainSpec.cpp',
  'sides/GBHardwareSpec.cpp',
  'sides/GBRobotType.cpp',
  'sides/GBScores.cpp',
  'sides/GBSide.cpp',
  'sides/GBSideReader.cpp',
  'simulation/GBDecorations.cpp',
  'simulation/GBFood.cpp',
  'simulation/GBHardwareState.cpp',
  'simulation/GBMessages.cpp',
  'simulation/GBObject.cpp',
  'simulation/GBObjectWorld.cpp',
  'simulation/GBRobot.cpp',
  'simulation/GBSensorShot.cpp',
  'simulation/GBShot.cpp',
  'simulation/GBWorld.cpp',
  'support/GBColor.cpp',
  'support/GBDeletionReporter.cpp',
  'support/GBErrors.cpp',
  'support/GBFinePoint.cpp',
  'support/GBFontManager.cpp',
  'support/GBGraphics.cpp',
  'support/GBMilliseconds.cpp',
  'support/GBModel.cpp',
  'support/GBNumber.cpp',
  'support/GBRandomState.cpp',
  'support/GBSound.cpp',
  'support/GBStringUtilities.cpp',
  'views/GBAboutBox.cpp',
  'views/GBDebugger.cpp',
  'views/GBListView.cpp',
  'views/GBMiniMap.cpp',
  'views/GBPortal.cpp',
  'views/GBRobotTypeView.cpp',
  'views/GBRosterView.cpp',
  'views/GBScoresView.cpp',
  'views/GBSideDebugger.cpp',
  'views/GBTournamentView.cpp',
  'views/GBView.cpp',
  'views/GBMenu.cpp',
  'views/GBMultiView.cpp',
]
src_ui = [
  'ui/GBApplication.cpp',
  'ui/GBMain.cpp',
  'ui/GBSDLApplication.cpp',
  'ui/GBSDLWindow.cpp',
  'ui/GBViewsApplication.cpp',
  'ui/GBWindow.cpp',
  'vendor/BBCSDL_gfx.c'
]
include = ['brains', 'sides', 'simulation', 'support', 'views']
include_ui = ['ui', 'views', 'vendor']

if get_option('optimization') == '3'
   add_project_arguments('-flto', language: 'cpp')
endif

# FIXME: fix these warnings
add_project_arguments('-Wno-sign-compare', language: 'cpp')
add_project_arguments('-Wno-shift-negative-value', language: 'cpp')
add_project_arguments('-Wno-missing-field-initializers', language: 'cpp')

has_wine = meson.get_compiler('cpp').has_header('windows.h') and target_machine.system() != 'windows'

executable('grobots_headless',
           src + ['GBHeadless.cpp'],
           include_directories : include,
           cpp_args: ['-DHEADLESS'],
           name_suffix: has_wine ? 'exe' : '',
           install : true)

if (target_machine.system() == 'windows' or has_wine)
  executable('grobots',
             src + [
               'ui/GBApplication.cpp',
               'ui/GBMain.cpp',
               'ui/GBViewsApplication.cpp',
               'ui/GBWindow.cpp'
             ],
             include_directories : include + include_ui + ['win'],
             cpp_args: has_wine ? ['-DWINDOWS=1', '-DNOMINMAX'] : [],
             link_args: has_wine ? ['-mno-cygwin', '-mwindows'] : [],
             name_suffix: 'exe',
             install: true)
  # todo: wine install .exe.so files

elif target_machine.system() == 'emscripten'
  # todo: -fwasm-exceptions
  executable('grobots_wasm',
             src + src_ui,
             include_directories : include + include_ui,
             cpp_args: ['-DWITH_SDL', '-sUSE_SDL=2', '-sUSE_SDL_GFX=2', '-sUSE_SDL_TTF=2', '-fexceptions'],
             link_args: ['-sUSE_SDL=2', '-sUSE_SDL_GFX=2', '-sUSE_SDL_TTF=2', '-sEXIT_RUNTIME=1', '--embed-file=../Vera.ttf@Vera.ttf', '--embed-file=../commune-plus-2.gb@commune-plus-2.gb', '--embed-file=../the-lunacy.gb@the-lunacy.gb', '--embed-file=../VeraBd.ttf@VeraBd.ttf', '--embed-file=../the-horde.gb@the-horde.gb', '-fexceptions'],
             name_suffix: 'html',
             install : true)
else
  # SDL
  sdl = dependency('sdl2', version : '>=2.0')
  sdl_ttf = dependency('SDL2_ttf', version : '>=2.0')
  sdl_gfx = dependency('SDL2_gfx', version : '>=1.0')

  executable('grobots_sdl',
             src + src_ui,
             include_directories : ['brains', 'sides', 'simulation', 'support', 'ui', 'views', 'vendor'],
             cpp_args: ['-DWITH_SDL'],
             dependencies : [sdl, sdl_ttf, sdl_gfx],
             install : true)
  install_data(['Vera.ttf', 'VeraBd.ttf'], install_dir: 'share/grobots/fonts')
endif
