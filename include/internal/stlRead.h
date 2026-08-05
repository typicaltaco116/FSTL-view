#pragma once

#include <stdint.h>

void stlRead(const char *stlFilename, uint32_t *numTriangles, float **vertices, float *maxDist);
