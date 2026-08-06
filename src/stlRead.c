#include <internal/stlRead.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STL_TRI_SIZE (50)

static void fileReadError(void)
{
	printf("stlRead: File Read Error\n");
	exit(-1);
}

void stlRead(const char *stlFilename, uint32_t *numTriangles, char **data)
{
	FILE *fid = fopen(stlFilename, "rb");
	if (!fid)
		fileReadError();

	fseek(fid, 80, SEEK_SET);
	
	size_t readCount;
	readCount = fread(numTriangles, sizeof(uint32_t), 1, fid);
	if (!readCount)
		fileReadError();

	*data = (char*)malloc(*numTriangles * STL_TRI_SIZE);
	readCount = fread(*data, STL_TRI_SIZE, *numTriangles, fid);
	if (readCount < *numTriangles) {
		// MISC ERROR HANDLING.
		fpos_t pos;
		fgetpos(fid, &pos);
		printf("pos = %d\n", pos);
		if (feof(fid))
			printf("EOF\n");
		if (ferror(fid))
			printf("ERROR\n");
		printf("readCount = %d\n", readCount);
		fclose(fid);
		fileReadError();
	}

	fclose(fid);
}
