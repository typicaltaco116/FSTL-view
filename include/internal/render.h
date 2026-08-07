#pragma once

void renderInit(const char *stlFilename, const char *shaderPath);

void renderLoop(float aspectRatio, float horzDeltaRad, float vertDeltaRad, float deltaScale);

void renderTerminate(void);
