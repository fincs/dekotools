
inc = include_directories('.')

dekomme_files = files(
	'dekomme.cpp',
	'MmeDocument.cpp',
)
dekomme_files += bison.process('MmeParser.ypp')
dekomme_files += flex.process('MmeLexer.lpp')

dekodef_files = files(
	'dekodef.cpp',
	'DefDocument.cpp',
	'DefBackendCpp.cpp',
	'DefBackendMme.cpp',
)
dekodef_files += bison.process('DefParser.ypp')
dekodef_files += flex.process('DefLexer.lpp')
