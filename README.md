# forge2D

A lightweight 2D game engine written in C++17, built on top of OpenGL 3.3 Core.  
University / portfolio project — developed incrementally over 8 weeks.

## Features (in progress)

- [x] Game loop with fixed-step update (60 Hz) and variable render
- [x] Configurable window via GLFW
- [x] OpenGL 3.3 Core context via GLAD
- [x] Colored, leveled logger with timestamp and file/line info
- [x] Shader & Texture abstraction
- [x] Batch renderer
- [ ] Orthographic 2D camera
- [ ] Input system (keyboard + mouse)
- [ ] Entity-Component system
- [ ] Spritesheet animation & Tilemap
- [ ] AABB collision detection
- [ ] Demo mini-game

## Dependencies

All fetched automatically by CMake at configure time.

| Library | Purpose |
|---|---|
| [GLFW 3.4](https://github.com/glfw/glfw) | Window creation + input |
| [GLAD 0.1.36](https://github.com/Dav1dde/glad) | OpenGL function loader |
| [GLM 1.0.1](https://github.com/g-truc/glm) | Math (vectors, matrices) |

> GLAD requires **Python 3** at configure time to generate the OpenGL bindings.

## Build

**Requirements:** CMake 3.20+, Git, Python 3, GCC / Clang / MSVC

```bash
cmake -B build -S . -G "Ninja"   # or "MinGW Makefiles", "Visual Studio 17 2022", …
cmake --build build
./build/forge2d
```

On Windows with MinGW/MSYS2, replace the last line with:

```powershell
.\build\forge2d.exe
```

## Project structure

```
forge2D/
├── src/
│   ├── core/
│   │   ├── Application.h / .cpp   # game loop
│   │   ├── Window.h / .cpp        # GLFW window + OpenGL context
│   │   └── Log.h / .cpp           # leveled, colored logger
│   └── main.cpp
├── assets/                        # textures, shaders, tilemaps (future)
└── CMakeLists.txt
```

## License

MIT
