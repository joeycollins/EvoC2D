# EvoC2D
Evolution simulator written in C and OpenGL. More info and demos at https://joeycollins.github.io/EvoC2DSite/

## Precompiled Release

An executable is in the /CompiledRelease/. Run this from within the directory.

## Compiling EvoC2D

EvoC2D was written for Windows machines. Requires opengl32.lib. This is installed by default with Visual Studio C++ development.

Requires CMake. If compiling from CMake-Gui, set the source directory to
the root of this repo and set the destination directory to the folder where
you would like the project to be build. Click configure and then generate. 

If compiling from the command-line there are many options, one such is as follows.

```console
C:/EvoC2D> mkdir build
C:/EvoC2D> cmake -B "C:/EvoC2D/build/"
C:/EvoC2D> cd build
C:/EvoC2D/build> cmake --build .
```

All dependencies of the project will be downloaded and compiled during this process. As of now, there are 
three: GLFW, CGLM, and freetype.

Open the project in your build folder with visual studio, right click the solution, goto Properties>Common Properties>Startup Project>Single Startup Project
and change the value from ALL_BUILD to evoc2d from the dropdown menu. Apply changes.

Run the program.
