#include "common.h"
#include "MmeDocument.h"

static int usage(const char* prog)
{
	fprintf(stderr,
		"Usage: %s [options] files...\n"
		"Options:\n"
		"  -o, --out=<file>     Specifies the name of the output file to generate\n"
		"  -n, --name=<name>    Specifies the base name of the generated identifiers\n"
		"  -s, --subchannel=<n> Specifies the index of the 3D subchannel (default 0)\n"
		"  -v, --version        Displays version information\n"
		, prog);
	return EXIT_FAILURE;
}

int main(int argc, char* argv[])
{
	const char *outFile = nullptr, *name = nullptr;
	uint8_t subchannel = 0;

	static struct option long_options[] =
	{
		{ "out",        required_argument, NULL, 'o' },
		{ "name",       required_argument, NULL, 'n' },
		{ "subchannel", required_argument, NULL, 's' },
		{ "help",       no_argument,       NULL, '?' },
		{ "version",    no_argument,       NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};

	int opt, optidx = 0;
	while ((opt = getopt_long(argc, argv, "o:?v", long_options, &optidx)) != -1)
	{
		switch (opt)
		{
			case 'o': outFile = optarg; break;
			case 'n': name = optarg; break;
			case 's': subchannel = atoi(optarg) & 7; break;
			case '?': usage(argv[0]); return EXIT_SUCCESS;
			case 'v': printf("%s - Built on %s %s\n", PACKAGE_STRING, __DATE__, __TIME__); return EXIT_SUCCESS;
			default:  return usage(argv[0]);
		}
	}

	if ((argc-optind) < 1)
		return usage(argv[0]);

	MmeDocument doc;
	for (int i = optind; i < argc; i ++)
		if (!doc.Load(argv[i]))
			return EXIT_FAILURE;

	FILE* fout = stdout;
	if (outFile)
	{
		fout = fopen(outFile, "w");
		if (!fout)
		{
			fprintf(stderr, "cannot open output file: %s\n", outFile);
			return EXIT_FAILURE;
		}
	}

	if (!name)
		name = "MmeMacro";

	doc.GenerateHeader(fout, name, subchannel);

	if (outFile)
		fclose(fout);

	return EXIT_SUCCESS;
}
