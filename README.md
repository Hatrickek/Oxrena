# OxArena
Quake-Like arena shooter made in Oxylus Engine.

## Building
- Install [Vulkan SDK](https://vulkan.lunarg.com/sdk/home). (Shader libraries are not needed).
- Run the root CMake script with a command like this:       
`cmake -S . -B ./build/ -G "Visual Studio 17 2022" -A x64` to generate Visual Studio files (or with any generator you like) into `build` folder.   
Or with `cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++` to generate for clang.    
- Then run this command to build it with CMake:   
`cmake --build ./build --config Release`   
