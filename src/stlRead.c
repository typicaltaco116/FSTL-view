#include "stlRead.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define STL_TRI_SIZE (50)

static void fileReadError(void)
{
	printf("stlRead: File Read Error\n");
	exit(-1);
}

void stlRead(const char *stlFilename, uint32_t *numTriangles, float **vertices)
{
	FILE *fid = fopen(stlFilename, "rb");
	if (!fid)
		fileReadError();

	fseek(fid, 80, SEEK_SET);
	
	size_t readCount;
	readCount = fread(numTriangles, sizeof(uint32_t), 1, fid);
	if (!readCount)
		fileReadError();

	char *buffer = (char*)malloc(*numTriangles * STL_TRI_SIZE);
	readCount = fread(buffer, STL_TRI_SIZE, *numTriangles, fid);
	if (readCount < *numTriangles) {
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

	*vertices = (float*)malloc(*numTriangles * 9 * sizeof(float));

	// Copy only the triangle vertices. Ignore normal vector and additionals.
	for (int i = 0; i < *numTriangles; ++i) {
		// Loop over the 9 floats per each triangle.
		float *temp = (float*)(buffer + STL_TRI_SIZE*i);
		for (int j = 0; j < 9; ++j) {
			*(*vertices + 9*i + j) = temp[j + 3]; // offset to avoid normals.
			if (isnan(*(*vertices + 9*i + j))) {
				printf("vertice is NAN\n");
			}
		}
	}
	free(buffer);
}
