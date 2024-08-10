project('earfull', ['cpp', 'c'],
          version: '0.0.1',
    meson_version: '>= 0.62.0',
  default_options: [ 'warning_level=2', 'werror=false', 'cpp_std=gnu++2a', ],
)

libnmdep = dependency('libnm')
giodep = dependency('gio-2.0')
sdldep = dependency('sdl2')

earfull_deps = [
    libnmdep,
    giodep,
    sdldep,
]

earfull_source_files = [
    'src/main.cpp',
    'src/backends/linux/nmbackend.cpp',
]

earfull_include_directories = [
    'src/imgui',
]

imgui_source_files = [
    'src/imgui/imgui.cpp',
    'src/imgui/imgui_draw.cpp',
    'src/imgui/backends/imgui_impl_sdl2.cpp',
    'src/imgui/backends/imgui_impl_sdlrenderer2.cpp',
    'src/imgui/imgui_tables.cpp',
    'src/imgui/imgui_widgets.cpp',
    'src/imgui/misc/cpp/imgui_stdlib.cpp',
]

executable('out', [earfull_source_files, imgui_source_files], dependencies: earfull_deps, include_directories: earfull_include_directories,)
