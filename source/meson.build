
inc = include_directories('.')

dekomme_files = files(
	'dekomme.cpp',
	'MmeDocument.cpp',
)
dekomme_files += bison.process('MmeParser.ypp')
dekomme_files += flex.process('MmeLexer.lpp')
