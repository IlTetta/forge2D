cmake_minimum_required(VERSION 3.20)
project(forge2d LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# --- GLFW ---
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# --- GLM ---
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# --- GLAD (requires Python 3 at configure time) ---
# CMAKE_POLICY_VERSION_MINIMUM=3.5 lets CMake 4+ accept glad v0.1.36's old cmake_minimum_required(2.8)
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v0.1.36
)
set(GLAD_PROFILE   "core" CACHE STRING "" FORCE)
set(GLAD_API       "gl=3.3" CACHE STRING "" FORCE)
set(GLAD_GENERATOR "c"    CACHE STRING "" FORCE)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(glad)
unset(CMAKE_POLICY_VERSION_MINIMUM)

# --- OpenGL ---
find_package(OpenGL REQUIRED)

# --- forge2d executable ---
file(GLOB_RECURSE FORGE_SOURCES "src/*.cpp")

add_executable(forge2d ${FORGE_SOURCES})

target_include_directories(forge2d PRIVATE
    src
    ${glad_SOURCE_DIR}/include
)

target_link_libraries(forge2d PRIVATE
    glfw
    glad
    glm::glm
    OpenGL::GL
)
