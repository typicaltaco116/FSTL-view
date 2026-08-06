#pragma once

#include <stdint.h>

typedef struct {
	uint32_t polyCount;
	float *vertices;
	float *normals;
} stl_data_t;

stl_data_t stlRead(const char *stlFilename);
