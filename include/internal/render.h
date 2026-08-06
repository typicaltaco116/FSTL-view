#pragma once

void renderInit(const char *stlFilename, const char *vertexSourceFile, const char *geoSourceFile, const char* fragSourceFile);

void renderLoop(float aspectRatio, float horzDeltaRad, float vertDeltaRad);

void renderTerminate(void);
