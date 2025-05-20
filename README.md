 # Terrain Generator with Raylib

A 2D procedural terrain generator implemented in C++ using Raylib. This project demonstrates terrain generation using classic algorithms like Diamond-Square and Perlin Noise.

## Features

- **Diamond-Square Algorithm**
  - Generates fractal heightmaps with configurable size (2^n+1)
  - Adjustable roughness parameter
  - Height-based terrain coloring

- **Perlin Noise**
  - 2D noise generation for natural-looking landscapes
  - Configurable frequency and amplitude
  - Smooth interpolation between points

## Building

### Windows
You have two options:

1. **Using MinGW-W64:**
   - Double click `build-MinGW-W64.bat`
   - Open terminal in the project folder
   - Run `make`

2. **Using Visual Studio 2022:**
   - Run `build-VisualStudio2022.bat`
   - Open the generated `.sln` file
   - Build the project in Visual Studio

### Linux
```bash
cd build
./premake5 gmake2
cd ..
make
```

### macOS
```bash
cd build
./premake5.osx gmake2
cd ..
make
```

## Requirements

- C++ compiler (GCC, MinGW-W64, or MSVC)
- Raylib 4.0 or later
- For Windows/MinGW users: Use w64devkit from https://github.com/skeeto/w64devkit/releases

## Output

The compiled executable will be placed in the `bin` directory.

## License

Copyright (c) 2025 TheUnrealZaka & Jowey7

This software is provided "as-is", without any express or implied warranty. See the full license text in the source code for more details.