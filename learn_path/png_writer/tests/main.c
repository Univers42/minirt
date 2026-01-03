/* Minimal smoke-test main for the png_writer project.
   Usage: ./png_test [path/to/file]
*/
#include <stdio.h>
#include <stdlib.h>

static long filesize(const char *path)
{
	FILE *f = fopen(path, "rb");
	if (!f)
		return -1;
	if (fseek(f, 0, SEEK_END) != 0)
	{
		fclose(f);
		return -1;
	}
	long s = ftell(f);
	fclose(f);
	return s;
}

int main(int argc, char **argv)
{
	printf("png_writer smoke-test executable\n");
	if (argc > 1)
	{
		long s = filesize(argv[1]);
		if (s < 0)
		{
			fprintf(stderr, "Failed to stat file: %s\n", argv[1]);
			return 1;
		}
		printf("File: %s, size: %ld bytes\n", argv[1], s);
	}
	else
	{
		printf("No file provided. Run: ./png_test <file.png>\n");
	}
	return 0;
}
