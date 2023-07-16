# OxArena
Quake-Like arena shooter made in Oxylus Engine.

## Running
- Download the exectuable for your OS and `assets.zip` from latest release.
- Extract `assets.zip` in the executable directory then run it.

## Building
- Install [Vulkan SDK](https://vulkan.lunarg.com/sdk/home). (Shader libraries are not needed).
- Run the root CMake script with a command like this:       
`cmake -S . -B ./build/ -G "Visual Studio 17 2022" -A x64` to generate Visual Studio files (or with any generator you like) into `build` folder.   
Or with `cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++` to generate for clang.    
- NOTE: If you don't have shaderc installed with VulkanSDK then you can pass `-DSHADERC_FROM_SOURCE=1` while configuring and build shaderc from source instead of getting it from the SDK.
- Then run this command to build it with CMake:   
`cmake --build ./build --config Release`   

