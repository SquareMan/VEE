VEE
=====
Vulkan Experimental Engine
-----

### Getting Started
*Note:* Currently the engine is not quite ready to cleanly support creating a game project that uses the engine. These instructions are for building the engine itself as well as the **HelloTriangle** testbed application.


#### System Dependencies
* cmake
* Vulkan SDK

#### Build Steps
```
mkdir build
cmake -B build -S . 
cmake --build build
cd build/Source/HelloTriangle
./HelloTriangle
```
* Debug builds (`-DCMAKE_BUILD_TYPE=Debug`) will contain the Editor and Release builds (`-DCMAKE_BUILD_TYPE=Release`) will not. This will become more robust in the future.

### Profiling

Profiling with Tracy is currently enabled by default for Debug builds. This is not ideal and profiling optimized builds will be more straightforward in the future.
