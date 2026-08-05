# FSTL-View
A simple cross-platform binary-STL viewer implemented in C using OpenGL.

## Dependencies
- CMake v3.16
- OpenGL v3.3
- GLFW3 v3.3
- cglm v0.9.6 (Included as submodule)

## Build Instructions
Create platform-specific build files.
```
cmake -B build
```
Execute build using cmake build.
```
cmake --build build
```

## User Guide
The binary generated in the bin folder takes the following arguments:
```
fstl-view <STL filename> <Shader path>
```
Example using the included Utah teapot STL.
```
fstl-view ../teapot.stl ../shaders
```
