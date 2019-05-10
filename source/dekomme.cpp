#include "common.h"
#include "MmeDocument.h"

int main(int argc, char* argv[])
{
	printf("Hello, world!\n");
	if (argc > 1)
	{
		MmeDocument doc;
		if (!doc.Load(argv[1]))
			return EXIT_FAILURE;
		doc.Print();
	}
	return EXIT_SUCCESS;
}
