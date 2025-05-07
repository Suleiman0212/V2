project('v2', ['c', 'cpp'])

sources = [
  'src/util.cpp',
  'src/log.cpp',
  'src/window.cpp',

  'src/render/shader.cpp',
  'src/render/quad.cpp',
  'src/render/render.cpp',
  'src/render/texture.cpp',

  'src/script/lexer.cpp',
  'src/script/script.cpp',
  'src/script/registry.cpp',
  'src/script/std_library.cpp',
  'src/script/parser.cpp',
  'src/script/vm.cpp',

  'src/glad/glad.c',
]
include_dirs = ['include/']
depends = [dependency('glfw3')]

subdir('include/')

shared_library('v2', sources,
  cpp_args: '-std=c++20',
  include_directories: include_dirs,
  dependencies: depends,
  install: true,
)
