#include <internal/stlRead.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define STL_TRI_SIZE (50)

static void fileReadError(void)
{
	printf("stlRead: File Read Error\n");
	exit(-1);
}

stl_data_t stlRead(const char *stlFilename)
{
	FILE *fid = fopen(stlFilename, "rb");
	if (!fid)
		fileReadError();

	fseek(fid, 80, SEEK_SET);
	
	size_t readCount;
	uint32_t numTriangles;
	readCount = fread(&numTriangles, sizeof(uint32_t), 1, fid);
	if (!readCount)
		fileReadError();

	char *buffer = (char*)malloc(numTriangles * STL_TRI_SIZE);
	readCount = fread(buffer, STL_TRI_SIZE, numTriangles, fid);
	if (readCount < numTriangles) {
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

	stl_data_t data;
	data.polyCount = numTriangles;
	data.vertices = (float*)malloc(numTriangles * 3 * 3 * sizeof(float));
	data.normals = (float*)malloc(numTriangles * 3 * sizeof(float));

	for (int i = 0; i < numTriangles; ++i) {
		char* blockStart = buffer + i * STL_TRI_SIZE;
		memcpy(data.normals + i, blockStart, 3 * sizeof(float));
		memcpy(data.vertices + i * 9, (float*)blockStart + 3, 9 * sizeof(float));
	}

	free(buffer);

	return data;
}
