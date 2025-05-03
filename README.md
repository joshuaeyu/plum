# Plum - A simple real-time graphics engine

![screenshot0.png](docs/images/screenshot0.png)

Plum is a simple real-time graphics engine built using OpenGL 4.1 and C++17. It features a deferred rendering pipeline and physically based shading. Users can create materials and instantiate models, primitives, and lights into the scene from the engine's GUI.

I am still actively developing this (see [To Do](#to-do)) and would like to implement more advanced graphics techniques either in this engine, in a separate ray tracing project, or using Vulkan. If you have any feedback please don't hesitate to let me know!

My goal in creating this engine was to synthesize the concepts taught in [Learn OpenGL](https://learnopengl.com) into a working, interactive software application, while also expanding upon these concepts.

## Requirements
* OpenGL 4.1+ support
* C++17 compiler support
* GNU Make

## Dependencies
* [GLAD](https://github.com/Dav1dde/glad)
* [GLFW](https://www.glfw.org)
* [GLM](https://glm.g-truc.net/)
* [Assimp](https://assimp-docs.readthedocs.io/en/)
* [stb](https://github.com/nothings/stb)
* [Dear ImGui](https://github.com/ocornut/imgui)

## Running the Engine
Clone or download the repository. From the root directory, run `make` then `./main`.

    git clone https://github.com/joshuaeyu/plum.git
    cd plum
    make
    ./main

## Features
### Renderer
* Deferred rendering
* Physically based rendering
    * Metallic-roughness workflow support
    * Direct and image-based lighting
* Directional and point lights
* Shadow mapping
* 3D model support (.gltf, .obj, .3mf)
* Primitives (sphere, cube, plane)
* Skybox (equirectangular map, six-sided cube map)
* Post processing
    * SSAO (screen-space ambient occlusion)
    * HDR tone mapping
    * Bloom
    * FXAA (fast approximate antialiasing)
* Scene graph
* Material system (only PBR metallic-roughness support currently)
* Asset management system
    * Hot reloading of shaders, images, models
### Interface
* User-specifiable...
    * Scene (nodes and components)
    * Materials
    * Asset import settings (for textures and models)
    * Skybox
* First-person freefly camera
* FPS counter
### Software design
* Abstraction of OpenGL objects
    * Vertex array and buffers: `Vao`, `Vbo`, `Ebo`, `Ubo`
        * `VertexArray` data structure with `Attribute` verbiage
    * Framebuffer and renderbuffer: `Fbo`, `Rbo`
    * `Tex` (distinct from `ImageAsset` and `Texture`)
    * `Program` (distinct from `ShaderAsset`)
* Entity-Component-System
    * Entity (`SceneNode`) - contains a `Transform` and child nodes
    * Component (`ComponentBase`) - `Primitive` mesh, `Model` mesh, or `Light`
    * System - `Renderer`
* Modularization of render logic
    * `Renderer`
    * `Environment`
    * `RenderModule`
    * `PostProcessing`
* Context management
    * Inputs and events system
    * Support for isolated `Demo` scenes
* Asset management
    * Intermediate representations of external assets for use by internal objects:
        * `ImageAsset` - used by `Texture`
        * `ShaderAsset` - used by `Program`
        * `ModelAsset` - used by `Model`
    * Prevents duplicate asset representation!
* Utility classes
    * `Time`
    * `Transform` - leverages quaternions; interface is loosely based on [Unity's Transform class](https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Transform.html)
    * `Path`, `File`, `Directory`

## To-do
* Rendering
    * Multiple materials, including PBR specular-glossiness workflow support
    * Blending / transparency support
    * [Clustered shading](https://www.humus.name/Articles/PracticalClusteredShading.pdf)
    * VAO batching
* Interface
    * Picking
    * Save/load scene state
    * Asset import settings
* Software
    * Move from GNU Make to CMake
    * Windows and Linux support
    * More robust use of UBO schemes
    * Cloning (duplication) of Material, SceneNode, Component

## References
* [Learn OpenGL](https://learnopengl.com)
* [docs.GL](https://docs.gl)
* [songho.ca](https://www.songho.ca/opengl/gl_sphere.html)
* [The Asset-Importer-Lib Documentation](https://assimp-docs.readthedocs.io/en/v5.3.0/)
* [glTF™ 2.0 Specification](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html)
* [Implementing FXAA](https://blog.simonrodriguez.fr/articles/2016/07/implementing_fxaa.html)
* [neo-mashiro/sketchpad](https://github.com/neo-mashiro/sketchpad) - inspiration for software architecture

## Asset Sources
* [Learn OpenGL](https://learnopengl.com)
* [Khronos Group glTF Sample Models](https://github.com/KhronosGroup/glTF-Sample-Models)
* [Sketchfab](https://sketchfab.com/features/gltf)
* [Free3D](https://free3d.com)
* [HDRi Haven](https://hdri-haven.com)
* [Texturelabs](https://texturelabs.org)

