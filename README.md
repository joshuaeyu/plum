# Plum - A simple graphics engine

![screenshot0.png](docs/images/screenshot0.png)

Plum is a simple real-time graphics engine built using OpenGL 4.1 and C++17. It features a deferred rendering pipeline and physically based shading. Users can instantiate lights, primitives, and models into the scene from the engine's GUI.

My goal in creating this engine was to synthesize and expand upon the concepts taught in [Learn OpenGL](https://learnopengl.com). This has been my first foray into 3D graphics programming with a low-level graphics API, so the program design and structure are still pretty raw (apologies!). I am still actively developing this (see [To Do](#to-do)) but plan to begin work on a Vulkan-based project once I feel like this has reached a nice stopping point.

## Requirements
* OpenGL 4.1+ support
* C++17 compiler support
* Make

## Dependencies
* [GLAD](https://github.com/Dav1dde/glad)
* [GLFW](https://www.glfw.org)
* [GLM](https://glm.g-truc.net/)
* [Assimp](https://assimp-docs.readthedocs.io/en/)
* [stb](https://github.com/nothings/stb)
* [Dear ImGui](https://github.com/ocornut/imgui)

## Running the Engine
Currently, this project requires headers (`*.h`, `*.hpp`) and binaries (e.g., `*.a`, `*.dylib`) of some of the dependencies listed above to be installed in the default include path(s) and linker search path(s), respectively:

| Dependency | In default include path(s) | In default linker search path(s) |
| ---------- | -------------------------- | -------------------------------- |
| GLAD       | `glad/glad.h`              | n/a                              |
| GLFW       | `GLFW/glfw3.h`             | `libglfw.3.dylib` or `libglfw.3.a` |
| GLM        | All `glm/*.hpp` headers    | n/a                              |
| Assimp     | All `assimp/*.h` and <br>`assimp/*.hpp` headers | `libassimp.dylib` (alias) and <br> `libassimp.5.4.1.dylib` |

Once the dependencies are in place, navigate to the `plum` directory and run `make` then `./main`.

    cd plum
    make
    ./main

## Features
### Renderer
* Deferred rendering (plus a forward rendering pass)
* Physically based rendering
    * Metallic-roughness material support
    * Direct and image-based lighting
* 3D model support - .obj, .gltf, .3mf
* Primitives - sphere, cube, rectangle
* Directional and point lights 
* Shadow mapping
* Skybox
* Post processing
    * SSAO (screen-space ambient occlusion)
    * HDR tone mapping
    * Bloom
    * FXAA (fast approximate antialiasing)
### Interface
* User-specifiable...
    * Shape templates (primitives with colored, textured, or hybrid surfaces)
    * Scene - skybox, light sources, nodes
* First-person freefly camera
* FPS counter

## To-do
* Rendering
    * Mesh instancing with glDraw*Instanced()
    * PBR specular-glossiness workflow support
    * Blending / transparency support
    * [Clustered shading](https://www.humus.name/Articles/PracticalClusteredShading.pdf)
    * Scene graph hierarchy
* Interface
    * Preview new scene nodes before publishing to scene
    * File system explorer to load assets from GUI
    * Save/load scene state
* Software design
    * Split .hpp files into proper .hpp headers and .cpp definitions
    * Abstract further

## References
* [Learn OpenGL](https://learnopengl.com)
* [glTF™ 2.0 Specification](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html)
* [docs.GL](https://docs.gl)
* [songho.ca](https://www.songho.ca/opengl/gl_sphere.html)
* [The Asset-Importer-Lib Documentation](https://assimp-docs.readthedocs.io/en/v5.3.0/)
* [Implementing FXAA](https://blog.simonrodriguez.fr/articles/2016/07/implementing_fxaa.html)

## Asset Sources
* [Learn OpenGL](https://learnopengl.com)
* [Khronos Group glTF Sample Models](https://github.com/KhronosGroup/glTF-Sample-Models)
* [Sketchfab](https://sketchfab.com/features/gltf)
* [Free3D](https://free3d.com)
* [HDRi Haven](https://hdri-haven.com)
* [Texturelabs](https://texturelabs.org)

