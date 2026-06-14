# forge2D

A lightweight 2D game engine written in **C++17**, built on top of **OpenGL 3.3 Core**.  
University / portfolio project — developed incrementally over 8 weeks.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++](https://img.shields.io/badge/C%2B%2B-17-informational.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3%20Core-green.svg)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-red.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)

---

## Features

- [x] Game loop with **fixed-step update** (60 Hz) and variable render
- [x] Configurable **window** via GLFW
- [x] OpenGL 3.3 Core context via GLAD
- [x] Colored, leveled **logger** with timestamp and file/line info
- [x] **Shader** abstraction: load from GLSL file, uniform setters
- [x] **Texture** abstraction: PNG/JPG loading via stb_image, raw pixel upload
- [x] **Batch renderer**: all quads flushed in a single draw call
- [x] **Orthographic Camera 2D**: pan, zoom, screen-to-world conversion
- [x] **Input system**: keyboard (down / pressed / released) + mouse + scroll delta
- [x] **Entity-Component system**: lightweight, template-based `addComponent` / `getComponent`
- [x] Built-in components: `Transform`, `SpriteRenderer`
- [x] **SceneManager**: deferred scene transitions
- [x] **Spritesheet animation**: frame-based `AnimationController` with named clips and loop support
- [x] **TileMap**: programmatic setup + Tiled JSON loading (`nlohmann/json`)
- [x] **Physics**: AABB detection, impulse resolution, position correction, layer filtering
- [x] Physics components: `Rigidbody2D`, `BoxCollider` (solid + trigger)
- [x] **Platformer demo**: playable mini-game exercising every engine system
- [x] **API documentation**: all public headers documented with Doxygen

---

## Demo

The repo includes a playable platformer that exercises every engine system:
collect all 5 coins scattered across 6 platforms and reach the green EXIT trigger.

**Controls:**

| Key | Action |
|---|---|
| `A` / `D` or Arrow keys | Move left / right |
| `Space` / `Up` | Jump (buffered input + coyote time) |
| `R` | Restart level |

---

## Architecture

```
forge2D/
├── src/
│   ├── core/
│   │   ├── Application.h/.cpp      # game loop (fixed update + variable render)
│   │   ├── Window.h/.cpp           # GLFW window + OpenGL context
│   │   ├── Camera2D.h/.cpp         # orthographic camera: pan, zoom, screenToWorld
│   │   └── Log.h/.cpp              # leveled, colored logger
│   ├── renderer/
│   │   ├── Shader.h/.cpp           # GLSL shader loader + uniform API
│   │   ├── Texture.h/.cpp          # texture upload (file or raw pixels)
│   │   ├── VertexBuffer.h/.cpp     # VBO abstraction
│   │   ├── IndexBuffer.h/.cpp      # IBO abstraction
│   │   ├── Renderer2D.h/.cpp       # batch renderer: drawQuad, drawSubTexture
│   │   └── TileMap.h/.cpp          # tile map: programmatic + Tiled JSON
│   ├── input/
│   │   └── Input.h/.cpp            # polling-based input (keyboard, mouse, scroll)
│   ├── scene/
│   │   ├── Component.h             # base class with onUpdate / onRender hooks
│   │   ├── Entity.h/.cpp           # entity: owns components via unique_ptr
│   │   ├── Transform.h             # position, scale, rotation + RTS matrix
│   │   ├── SpriteRenderer.h/.cpp   # renders a textured/colored quad at Transform
│   │   ├── AnimationController.h/.cpp  # frame-based animation on a spritesheet
│   │   ├── Scene.h/.cpp            # container with createEntity, lifecycle hooks
│   │   └── SceneManager.h/.cpp     # global scene management + deferred transitions
│   ├── physics/
│   │   ├── AABB.h                  # axis-aligned bounding box struct
│   │   ├── BoxCollider.h/.cpp      # collider component + onCollision callback
│   │   ├── Rigidbody2D.h/.cpp      # velocity, gravity, mass
│   │   └── PhysicsWorld.h/.cpp     # step(): O(n²) detection, impulse resolution
│   └── main.cpp                    # ForgeApp + PlatformerScene (playable demo)
├── assets/
│   └── shaders/                    # GLSL vertex + fragment shaders
├── Doxyfile                        # Doxygen configuration
└── CMakeLists.txt
```

---

## Dependencies

All fetched automatically by CMake at configure time; no manual installation needed.

| Library | Version | Purpose |
|---|---|---|
| [GLFW](https://github.com/glfw/glfw) | 3.4 | Window creation + input |
| [GLAD](https://github.com/Dav1dde/glad) | 0.1.36 | OpenGL function loader |
| [GLM](https://github.com/g-truc/glm) | 1.0.1 | Math: vectors, matrices (header-only) |
| [stb_image](https://github.com/nothings/stb) | master | Image loading PNG/JPG (single header) |
| [nlohmann/json](https://github.com/nlohmann/json) | 3.11.3 | JSON parsing for Tiled tilemaps (header-only) |

> **Note:** GLAD requires **Python 3** to be available at CMake configure time (used to generate OpenGL bindings).

---

## Build

**Requirements:** CMake 3.20+, Git, Python 3, and a C++17-capable compiler (GCC / Clang / MSVC).

```bash
# Configure
cmake -B build -S .

# Build
cmake --build build --config Release

# Run (Linux / macOS)
./build/forge2d

# Run (Windows)
.\build\Release\forge2d.exe
```

You can also use a specific generator:

```bash
# Ninja (recommended — fastest)
cmake -B build -S . -G "Ninja"

# MinGW on Windows
cmake -B build -S . -G "MinGW Makefiles"

# Visual Studio 2022
cmake -B build -S . -G "Visual Studio 17 2022"
```

---

## Documentation

All public headers are documented with Doxygen. To regenerate:

```bash
doxygen Doxyfile
```

Then open `docs/html/index.html` in a browser.

---

## License

[MIT](LICENSE)
