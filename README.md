# Game Engine

This project was made for the purpose of my learning experience and is not intended to actually be used. It is still very, very incomplete.

## Introduction

This project is a game engine I started working on over the summer (2019) to learn more about game development and how game engines work. So far, most of the focus on this project has been on the 3D graphics aspect, and not much attention has been given to other aspects, such as physics, audio, networking, and gameplay systems yet. Because this is not intended for use, all external dependencies have been precompiled for Windows 32-bit in [extlibs](https://github.com/MiniGee/GameEngine/tree/master/extlibs).

## Summary

This is a quick summary of the different sections of the game engine. Feel free to check out the actual code: [GameEngine/Source](https://github.com/MiniGee/GameEngine/tree/master/GameEngine/Source)

### [Core](https://github.com/MiniGee/GameEngine/tree/master/GameEngine/Source/Core)

The core classes include various utility and memory management functionality. It includes functions for doing aligned memory allocations, a few data containers, timing systems, type reflection, a logging system, and a basic macro library. These are just the basic things all the other system depends on. The most important data container in this module is probably the object pool. In a normal object pool, a the size of the pool has to be known beforehand. One option to enable a dynamically sized object pool is to double the capacity once all the space in the pool has been used up. But, this would require allocating a whole new chunk of memory, then copying everything over to the new chunk, much like std::vector. The problem with this is that any pointers to the objects will be unusable, as it would now be pointing at freed memory. The solution and the way [ObjectPool](https://github.com/MiniGee/GameEngine/blob/master/GameEngine/Source/Core/ObjectPool.h) was implemented was to use "pages". Once one page with a certain number of slots is used up, a new page of the same size is allocated, and that new page now acts as the "current page" and all future allocations will use the new page. The pages are connected in a way similar to a linked list so that freeing an object from the pool made possible by finding the page a certain pointer points to. The [ObjectPool](https://github.com/MiniGee/GameEngine/blob/master/GameEngine/Source/Core/ObjectPool.h) is used for resource management because when allocating more space, all existing pointers that store data in the pool will still be useable, making it a good choice for resource management with pointers.

### [Engine](https://github.com/MiniGee/GameEngine/tree/master/GameEngine/Source/Engine)

The engine category include classes that provide the most basic requirements for the engine, such as creating a window, etc. Windowing and input is handled by GLFW for cross platform compatability. The rest of the engine category mostly handles the game loop, transition between scenes, and initializes anything that is needed (such as the logging system).

### [Math](https://github.com/MiniGee/GameEngine/tree/master/GameEngine/Source/Math)

Includes basic linear algebra library with all the required math needed for 3D graphics, such as vectors, matrices, and quaternions. It also includes a few convenient constants (PI) and functions (Degrees to Radians). Common matrix transform functions, are also provided, such as the creation of the transform, view, and projection matrix. Matrix inversion is only provided for 4x4 matrices so far.

### [Resource](https://github.com/MiniGee/GameEngine/tree/master/GameEngine/Source/Resource)

Has basic functionality for resource loading and management. It includes an XML parser, which uses [RapidXML](https://sourceforge.net/projects/rapidxml/files/rapidxml/rapidxml%201.13/rapidxml-1.13.zip/download). In this engine, anything can be considered a resource, and each data type is handled by its own object pool. The object pool is used for resource management because pointers aren't invalidated when more space is allocated for the pool. Another option that could have been used for management is resource handles, which map a certain ID to a memory location. I chose to use the object pool because I liked the idea of being able to directly access an object through a pointer.

### [Scene](https://github.com/MiniGee/GameEngine/tree/master/GameEngine/Source/Scene)

The scene directory includes the [Scene](https://github.com/MiniGee/GameEngine/blob/master/GameEngine/Source/Scene/Scene.h) class, which holds all the "managers" that are used, such as the renderer. It also includes the [Transformable](https://github.com/MiniGee/GameEngine/blob/master/GameEngine/Source/Scene/Transformable.h) class, which is basically adds spatial functionality. The rest of the classes aid in gameplay logic. Originally, I wanted to implement the Entity Component System (ECS) model, (which I successfully did in another project) but I decided not to because I couldn't find a way to fit the other systems (like the renderer) to work with it. ECS, for those who don't know, is just a way of organizing your code and gameplay logic in a way that is cache-friendly. In most modern computers, reading from memory (accessing data) is relatively slow, so data that is retrieved from RAM is stored in caches, which has much faster access speeds. Maxmizing cache usage is very important to having a performant application, and the way developers normally do that is by taking advantage of the fact that memory is read in chunks (a cache line), and that the CPU can prefetch data. By keeping data in a contiguous array, you can maximize cache usage. ECS is designed to keep game components that have the same type in the same array. This allows for maximal cache usage and easier use of parallelization.
Because the concept was too hard to fit in my OOP engine (as well as my OOP way of thinking), I decided to settle for a hybrid version of ECS, where GameObjects are grouped together rather than components. This allows for the GameSystems to declare which types of GameObjects they need when doing their update.
Spatial organization structures, such as octrees, or grids haven't been implemented yet because there has not been a need yet. I do intend to add some form of scene structure in the future though. Because of the lack of scene structure, frustum culling and occlusion culling haven't been implemented yet either.

### [Graphics](https://github.com/MiniGee/GameEngine/tree/master/GameEngine/Source/Graphics)

This section is where I have spent the most of my time. For the graphics library, I used OpenGL v3.3. The graphics system in this engine consists of a renderer, a basic extendable post-processing system, OpenGL wrappers, the "camera" class, and various higher level classes, such as the model, lights, materials, terrain, skyboxes, and a pretty cool atmospheric scattering effect.
The renderer uses several render passes to create the final image, but so far, only the default scene pass is functional. The renderer uses a deferred lighting model, which was mainly implemented for the atmosphere effect, which will be talked about later.
Here is a short summary of some of the graphical features
* Model loading is done through the use of the [Assimp](http://www.assimp.org/) library, and most of the loading process is basic. Textured models and animations are not supported yet.
* Up to 3 dimensional textures and framebuffers are supported.
* Shaders are loaded using XML configuration files, and can be applied to a material
* Materials are paired with meshes, and a model can hold multiple mesh-material pairs
* At the moment, index arrays (when rendering with vertex arrays) aren't supported yet because I favored a low-poly look during development, so each vertex needs to have the same normal vector as the face it is part of. Adding this feature should be pretty quick and is planned in the future
* Custom post-processing effects can be added by overriding the PPEffect class, and adding it to the effects queue

#### Terrain

As stated above, low-poly terrain is used, but adding smooth shading in the future is very trivial. The method used for rendering the terrain is very uncommon. A single square plane mesh is created, with the center (where the camera is) has high resolution, and the edges of the plane are low resolution (in terms of vertices). This acts as a sort of LOD method, and by centering the mesh on the camera, there is no need to do extra processing for LOD chunks. Take the position of each vertex, scale it according to the size of the heightmap, sample from the heightmap, and you have a heightmap-based terrain. The only problem now is that whenever the camera moves, the terrain will have a rippling effect. To fix that, assign each vertex a "resolution" number, and round the position of that vertex to the nearest "resolution" point. For example, if the size of the squares in the center of the plane had a size of 1 (resolution of 1), the horizontal xz coordinates of the vertex would round to whole numbers, and if the resolution were 2, the vertices would round to even numbers. For a better explanation, visit this [article](http://casual-effects.blogspot.com/2014/04/fast-terrain-rendering-with-continuous.html). The problem now is that when moving, the LOD transitions are sudden and block because of the low-poly look. The normals can't be calculated from the height map because smooth lighting isn't used, so the best way to make a smooth transition between LOD levels is to interpolate the positions of the vertices between the two levels, based on distance from the camera. For heightmap generation, simplex noise is used.

#### Precomputed Atmospheric Scattering

This effect is probably my favorite part of the engine so far. This atmosphere effect is a very realistic looking effect that models how light is scattered as it travels from space to the ground. This effect is based on the work of Eric Bruneton and the research paper can be found [here](https://hal.inria.fr/inria-00288758/en). It took me a very long time to read and understand all the concepts and the math that was involved with this topic, as it included math and physics that is pretty far beyond my current level (plus, all the confusing math notation didn't help). After 2 or 3 weeks of struggling with the idea, I finally understood enough to implement it in my game engine. Note that a some of the longer sections of code were taken from his demo files. The basic idea of scattering, without any math, is that when light travels through the atmosphere, there are air molecules at every point along the path. A certain percentage of light will be scattered away at every point. The amount of light that remains without being scattered from point A to point B is called the transmittance. At every point, along that path, light that is scattered away from the original path will start travelling along another path. As you can guess, light will scatter several times along its trip, and that is called multiple scattering. In my project, only single scattering is implemented, where light from the sun only scatters once. It took a lot of work, but the visual effects look pretty cool.

### Game

This folder isn't inteded to part of the engine, it is just there for development and testing. It includes a basic scene, a few systems, and a game object.

## Pictures
