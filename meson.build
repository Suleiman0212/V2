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
  'src/script/parser.cpp',
  'src/script/vm.cpp',

  'src/glad/glad.c',
]
include_dirs = ['include/']
depends = [dependency('glfw3')]

headers = [
  'include/util.hpp',
  'include/log.hpp',
  'include/window.hpp',

  'include/render/shader.hpp',
  'include/render/quad.hpp',
  'include/render/render.hpp',
  'include/render/texture.hpp',

  'include/script/ast.hpp',
  'include/script/cell.hpp',
  'include/script/lexer.hpp',
  'include/script/parser.hpp',
  'include/script/registry.hpp',
  'include/script/script.hpp',
  'include/script/vm.hpp',
]
install_headers(headers,
  subdir: 'v2',
)

shared_library('v2', sources,
  cpp_args: '-std=c++20',
  include_directories: include_dirs,
  dependencies: depends,
  install: true,
)
