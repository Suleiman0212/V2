project('v2', ['c', 'cpp'])

sources = [
  'src/main.cpp',
  'src/log.cpp',
  'src/window.cpp',
  'src/gfx.cpp',
  'src/glad/glad.c',
]
include_dirs = ['include/']
depends = [dependency('glfw3')]

executable('v2', sources,
  cpp_args: '-std=c++20',
  include_directories: include_dirs,
  dependencies: depends,
)
