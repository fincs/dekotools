#include "common.h"
#include "DefDocument.h"

static int usage(const char* prog)
{
	fprintf(stderr,
		"Usage: %s [options] file\n"
		"Options:\n"
		"  -h, --header=<file>  Specifies the name of the C++ header file (.h) to generate\n"
		"  -m, --mme=<file>     Specifies the name of the dekomme file (.mme) to generate\n"
		"  -v, --version        Displays version information\n"
		, prog);
	return EXIT_FAILURE;
}

int main(int argc, char* argv[])
{
	const char *hFile = nullptr, *mmeFile = nullptr;

	static struct option long_options[] =
	{
		{ "header",     required_argument, NULL, 'h' },
		{ "mme",        required_argument, NULL, 'm' },
		{ "help",       no_argument,       NULL, '?' },
		{ "version",    no_argument,       NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};

	int opt, optidx = 0;
	while ((opt = getopt_long(argc, argv, "h:m:?v", long_options, &optidx)) != -1)
	{
		switch (opt)
		{
			case 'h': hFile = optarg; break;
			case 'm': mmeFile = optarg; break;
			case '?': usage(argv[0]); return EXIT_SUCCESS;
			case 'v': printf("%s - Built on %s %s\n", PACKAGE_STRING, __DATE__, __TIME__); return EXIT_SUCCESS;
			default:  return usage(argv[0]);
		}
	}

	if ((argc-optind) != 1)
		return usage(argv[0]);
	const char *defFile = argv[optind];

	DefDocument doc;
	if (!doc.Load(defFile))
		return EXIT_FAILURE;

	if (hFile)
	{
		fprintf(stderr, "%s: not implemented\n", hFile);
		// todo
	}

	if (mmeFile)
	{
		FILE* f = fopen(mmeFile, "w");
		if (!f)
		{
			fprintf(stderr, "could not open file: %s\n", mmeFile);
			return EXIT_FAILURE;
		}
		doc.EmitMme(f);
		fclose(f);
	}

	return EXIT_SUCCESS;
}
