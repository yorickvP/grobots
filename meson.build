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
ui = [
  'ui/GBApplication.cpp',
  'ui/GBMain.cpp',
  'ui/GBSDLApplication.cpp',
  'ui/GBSDLWindow.cpp',
  'ui/GBViewsApplication.cpp',
  'ui/GBWindow.cpp',
  'vendor/BBCSDL_gfx.c'
]

sdl = dependency('sdl2', version : '>=2.0')
sdl_ttf = dependency('SDL2_ttf', version : '>=2.0')
sdl_gfx = dependency('SDL2_gfx', version : '>=1.0')

executable('grobots',
           src + ['GBHeadless.cpp'],
           include_directories : ['brains', 'sides', 'simulation', 'support', 'views'],
           cpp_args: ['-DHEADLESS', '-Wno-sign-compare'],
           install : true)

# todo: only on emscripten
if meson.is_cross_build()
  executable('grobots_wasm',
             src + ui,
             include_directories : ['brains', 'sides', 'simulation', 'support', 'ui', 'views', 'vendor'],
             cpp_args: ['-DWITH_SDL', '-Wno-sign-compare', '-sUSE_SDL=2', '-sUSE_SDL_GFX=2', '-sUSE_SDL_TTF=2'],
             link_args: ['-sUSE_SDL=2', '-sUSE_SDL_GFX=2', '-sUSE_SDL_TTF=2', '-sEXIT_RUNTIME=1', '--embed-file=../Vera.ttf@Vera.ttf', '--embed-file=../commune-plus-2.gb@commune-plus-2.gb', '--embed-file=../the-lunacy.gb@the-lunacy.gb', '--embed-file=../VeraBd.ttf@VeraBd.ttf', '--embed-file=../the-horde.gb@the-horde.gb'],
             name_suffix: 'html',
             install : true)
else
  executable('grobots_sdl',
             src + ui,
             include_directories : ['brains', 'sides', 'simulation', 'support', 'ui', 'views', 'vendor'],
             cpp_args: ['-DWITH_SDL', '-Wno-sign-compare'],
             dependencies : [sdl, sdl_ttf, sdl_gfx],
             install : true)
endif
