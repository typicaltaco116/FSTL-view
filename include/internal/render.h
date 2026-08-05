#pragma once

void renderInit(const char *stlFilename, const char *vertexSourceFile, const char* fragSourceFile);

void renderLoop(float aspectRatio, float deltaRad);

void renderTerminate(void);
