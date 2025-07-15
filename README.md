VEE
=====
Vulkan Experimental Engine
-----

### Getting Started
*Note:* Currently the engine is not quite ready to cleanly support creating a game project that uses the engine. These instructions are for building the engine itself as well as the **HelloTriangle** testbed application.


#### System Dependencies
* clang - other compilers may work, but are not supported out of the box.
* cmake
* Vulkan SDK

#### Build Steps
* Debug Build
```
cmake --preset=Debug-Clang
cmake --build --preset=Debug
cd build/build-Debug-Clang/Source/HelloTriangle
./HelloTriangle
```
* Development Build
```
cmake --preset=Development-Clang
cmake --build --preset=Development
cd build/biuld-Development-Clang/Source/HelloTriangle
./HelloTriangle
```
* Shipping Build
```
cmake --preset=Release-Clang
cmake --build --preset=Release
cd build/build-Release-Clang/Source/HelloTriangle 
./HelloTriangle
```
* *Note*: Currently only debug builds will contain the Editor. This will become more robust in the future.

### Profiling

Profiling with Tracy is currently enabled by default for Debug builds. This is not ideal and profiling optimized builds will be more straightforward in the future.
