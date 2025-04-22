To do:
- Camera X
- Texture X
- Inputs X
- Transform X
  - itself X
  - scene nodes X
  - camera X
- Window inputs (esc, grave) X
- Framebuffer callback X
- Reorganize directories X
- Clean up and fix Makefile X
- shared ptr reset issue X
- texture sampler issue X
- texture loading issues X
- Postprocessing X
  - FXAA X
  - HDR X
  - Bloom X
  - SSAO X

Remaining major:
- GUI WIP
  - renaming
- asset manager - textures, models WIP
  - stronger statefulness - gui knows on startup what was initialized via code X
- IBL specular map seams?? X

Remaining minor:
- window vs context (e.g., getTime) WIP
  - context needs to encapsulate gl and glfw. a lot of quirks.
- lights: reconcile lighting pass uniforms vs shadow pass uniforms WIP
- UBO scheme initialization
- some utility type for detecting changes?
- Model
  - gltf - do a few more examples
  - obj
- Texture
  - decouple file, image, texture, tex, texture view...
- Material
  - no gamma correction for user defined mats somehow
  - specular
  - Phong
- streamline modules, environment, postprocessing WIP
- rules of 0/3/5 WIP
- reorder class members
- Error handling and logging
- cloning
  - material
  - scenenode
  - component

When designing a 3D graphics engine, several design patterns can help manage complexity, improve maintainability, and ensure that the engine is modular and extensible. Here are some of the most useful design patterns to consider:

### 1. **Component-Entity-System (CES)**
   - **Problem**: Managing objects with diverse, complex behaviors and states.
   - **Solution**: This pattern separates game objects (entities) from the behaviors or attributes (components) and the logic that updates them (systems). It allows for easy extension and modification of behaviors without tightly coupling different parts of the engine.
   - **Use case in a 3D graphics engine**: 
     - Entities (e.g., a model, camera, light, or particle system) are separate from components (e.g., mesh, texture, animation, physics).
     - Systems (e.g., render system, physics system, animation system) process entities that have the necessary components.

### 2. **Factory Method**
   - **Problem**: Creating complex objects in a flexible and reusable way.
   - **Solution**: Instead of calling constructors directly, you define a method for creating objects, which allows for greater flexibility in instantiating different objects of the same type. This is useful for creating objects that require complex initialization (e.g., meshes, shaders, materials).
   - **Use case in a 3D graphics engine**:
     - Create complex resources like shaders, meshes, or materials using factory methods that encapsulate their creation logic. This decouples the rest of the code from the details of resource creation.

### 3. **Singleton**
   - **Problem**: Ensuring that a class has only one instance and providing a global point of access to it.
   - **Solution**: The Singleton pattern ensures that a class (e.g., the rendering system, input system, or resource manager) has only one instance, and it provides a global point of access to that instance.
   - **Use case in a 3D graphics engine**:
     - Commonly used for systems like the **Renderer**, **ResourceManager**, or **WindowManager**, where only one instance is needed for the entire engine.

### 4. **Observer**
   - **Problem**: Managing communication between objects that need to be notified of changes without tightly coupling them.
   - **Solution**: The Observer pattern allows objects (observers) to subscribe to an event or state change in another object (the subject). When the state of the subject changes, all subscribed observers are notified.
   - **Use case in a 3D graphics engine**:
     - This pattern can be used for managing updates between various systems like **UI updates**, **scene graph changes**, or **input handling** (e.g., when a player presses a key, multiple systems like the camera or character controller may need to react).

### 5. **Strategy**
   - **Problem**: Choosing between multiple algorithms or behaviors dynamically.
   - **Solution**: The Strategy pattern defines a family of algorithms or behaviors, encapsulates each one, and makes them interchangeable. It allows an object to change its behavior at runtime by switching between different strategies.
   - **Use case in a 3D graphics engine**:
     - In **rendering**, you could have different strategies for rendering a scene, such as forward rendering, deferred rendering, or ray tracing. You can switch between these strategies based on the available hardware or user preferences.
     - For **animation**, different strategies could handle keyframe animation vs skeletal animation.

### 6. **State**
   - **Problem**: Managing states that change over time and ensuring that different behaviors are triggered based on the object's current state.
   - **Solution**: The State pattern allows an object to change its behavior when its internal state changes, making it appear as if the object has changed its class.
   - **Use case in a 3D graphics engine**:
     - This can be useful for controlling game object behavior or even rendering pipeline states. For instance, you might use state machines to manage the states of characters (idle, walking, jumping), and rendering states (depth test, alpha blend mode, etc.).

### 7. **Decorator**
   - **Problem**: Extending functionality without modifying existing code.
   - **Solution**: The Decorator pattern allows you to add functionality to an object dynamically, without altering its structure. This is especially useful for modifying or enhancing existing objects with new features.
   - **Use case in a 3D graphics engine**:
     - You can use decorators to add additional behavior to objects such as **render effects** (shaders, post-processing effects), or to modify **materials** (e.g., adding reflections or transparency effects to an existing material).

### 8. **Prototype**
   - **Problem**: Creating new objects by copying or cloning existing ones rather than instantiating them from scratch.
   - **Solution**: The Prototype pattern allows you to create a new object by cloning an existing object, which can be more efficient if the object is complex or if multiple similar objects are required.
   - **Use case in a 3D graphics engine**:
     - This is helpful for **cloning objects** in scenes (e.g., creating many copies of a tree in a forest) or **terrain generation** where you might copy base objects and tweak them slightly for variation.

### 9. **Command**
   - **Problem**: Decoupling requests from their execution, which allows commands to be queued, undone, or repeated.
   - **Solution**: The Command pattern encapsulates a request as an object, allowing the parameterization of clients with different requests, queuing of requests, and logging of the request history.
   - **Use case in a 3D graphics engine**:
     - It can be used for implementing **undo/redo functionality** or for handling **input events**, such as pressing keys or mouse buttons, and abstracting them into commands that can be executed or canceled.

### 10. **Flyweight**
   - **Problem**: Managing memory efficiently when a large number of similar objects are required.
   - **Solution**: The Flyweight pattern helps reduce memory usage by sharing common parts of objects (the intrinsic state) rather than creating duplicates. This can be useful when dealing with many objects that have the same behavior or state.
   - **Use case in a 3D graphics engine**:
     - This is useful for sharing common **textures**, **models**, or other resources across multiple objects in a scene, reducing memory usage when many objects use the same data (e.g., using one texture for all trees in a forest).

### 11. **Scene Graph**
   - **Problem**: Organizing hierarchical objects in a scene to manage transformations, rendering, and relationships between objects.
   - **Solution**: A scene graph organizes objects in a hierarchical tree structure, with each node representing an object (e.g., a mesh, light, camera). This structure helps to manage relationships and transformations efficiently.
   - **Use case in a 3D graphics engine**:
     - A **scene graph** can represent hierarchical relationships (e.g., parent-child) between objects, such as a character with multiple body parts or a camera that follows a moving object.

### 12. **Abstract Factory**
   - **Problem**: Creating families of related objects without specifying their concrete classes.
   - **Solution**: The Abstract Factory pattern provides an interface for creating families of related or dependent objects. It allows for creating objects that belong to a specific category (e.g., rendering systems, shaders, materials) without knowing their concrete types.
   - **Use case in a 3D graphics engine**:
     - A factory that can create different **rendering pipelines** (e.g., DirectX, OpenGL, Vulkan) or **shader programs** depending on the platform.

---

### Key Takeaways:
- **Component-Entity-System (CES)** is great for flexibility and scalability when dealing with complex objects and behaviors.
- **Factory Method**, **Singleton**, and **Abstract Factory** are useful for creating, managing, and sharing resources like textures, meshes, and shaders.
- **State**, **Strategy**, and **Observer** are particularly useful for controlling object behaviors and reacting to changes in real-time (e.g., input handling, animation, state transitions).
- **Flyweight** and **Prototype** are crucial for memory management and optimization when dealing with large numbers of objects.

By combining these patterns appropriately, you can create a flexible, maintainable, and performant 3D graphics engine that can easily adapt to changing requirements.