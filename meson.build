project('dekotools', 'cpp',
	version: '1.0.0',
	default_options: [ 'buildtype=release', 'strip=true', 'b_ndebug=if-release', 'cpp_std=c++14' ],
)

prog_bison = find_program('bison', native: true)
prog_flex = find_program('flex', native: true)

bison = generator(prog_bison,
	output : [ '@BASENAME@.cpp', '@BASENAME@.hpp' ],
	arguments : [ '-o', '@OUTPUT0@', '--defines=@OUTPUT1@', '@INPUT@' ],
)

flex = generator(prog_flex,
	output : [ '@BASENAME@.cpp', '@BASENAME@.hpp' ],
	arguments : [ '-o', '@OUTPUT0@', '--header-file=@OUTPUT1@', '@INPUT@' ],
)

pre_args = [
	'-DPACKAGE_STRING="@0@ @1@"'.format(meson.project_name(), meson.project_version()),
]

foreach a : pre_args
	add_project_arguments(a, language : ['c', 'cpp'])
endforeach

subdir('source')

dekomme = executable(
	'dekomme',
	dekomme_files,
	include_directories: inc,
	install: true,
)
