set(PROJECT_NAME OxArena)

# Source groups
file(GLOB src_root "src/*.h" "src/*.cpp")
source_group("src" FILES ${src_root})
file(GLOB src_systems "src/Systems/*.h" "src/Systems/*.cpp")
source_group("src/Systems" FILES ${src_systems})
file(GLOB src_ui "src/UI/*.h" "src/UI/*.cpp")
source_group("src/UI" FILES ${src_ui})
set(ALL_FILES ${src_root} ${src_systems} ${src_ui})

# Target
add_executable(${PROJECT_NAME} ${ALL_FILES})

set(ROOT_NAMESPACE OxArena)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)

# Target name
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG   "OxArena"
    TARGET_NAME_RELEASE "OxArena"
    TARGET_NAME_Distribution    "OxArena"
)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Debug-windows-x86_64/OxArena/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Release-windows-x86_64/OxArena/"
    OUTPUT_DIRECTORY_Distribution    "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Distribution-windows-x86_64/OxArena/"
)

# MSVC OxArena library

get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
  $<$<CONFIG:Debug>:
  MultiThreadedDebug
  >
  $<$<CONFIG:Release>:
  MultiThreaded
  >
  $<$<CONFIG:Distribution>:
  MultiThreaded
  >
  $<$<NOT:$<OR:$<CONFIG:Debug>,
  $<CONFIG:Release>,
  $<CONFIG:Distribution>
  >>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
  )
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Oxylus/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Oxylus/vendor"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Oxylus/vendor/GLFW/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Oxylus/vendor/ImGui"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Oxylus/vendor/glm"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Oxylus/vendor/entt"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Oxylus/vendor/ImGuizmo"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Oxylus/vendor/tinygltf"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Oxylus/vendor/ktx/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Oxylus/vendor/miniaudio"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Oxylus/vendor/tracy/public"
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "OX_DEBUG;"
        "_DEBUG;"
        "TRACY_ENABLE"
    ">"
    "$<$<CONFIG:Release>:"
        "OX_RELEASE;"
        "NDEBUG;"
        "TRACY_ENABLE"
    ">"
    "$<$<CONFIG:Distribution>:"
        "OX_DISTRIBUTION;"
        "NDEBUG"
    ">"
    "_HAS_EXCEPTIONS=0;"
    "UNICODE;"
    "_UNICODE"
)

# Compile and link options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Od;
            /Zi
        >
        $<$<CONFIG:Release>:
            /Ox;
            /Oi;
            /GF;
            /Gy;
            /Zi;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
        >
        $<$<CONFIG:Distribution>:
            /Ox;
            /Oi;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
        >
        /MP;
        /std:c++latest;
        /Gm-;
        /W3;
        /Y-
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /DEBUG;
        >
        $<$<CONFIG:Release>:
            /DEBUG;
            /OPT:REF;
            /OPT:ICF;
        >
        $<$<CONFIG:Distribution>:
            /OPT:REF;
            /OPT:ICF;
        >
        /SUBSYSTEM:CONSOLE
    )
    set(CMAKE_SHARED_LINKER_FLAGS_DISTRIBUTION ${CMAKE_SHARED_LINKER_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTION ${CMAKE_EXE_LINKER_FLAGS})
endif()

include(FetchContent)

# Link with oxylus.
FetchContent_Declare(
  Oxylus
  GIT_REPOSITORY https://github.com/Hatrickek/Oxylus-Engine.git
  GIT_TAG main
  SOURCE_SUBDIR "Oxylus"
)
FetchContent_MakeAvailable(Oxylus)
set(SHADERC_FROM_SOURCE TRUE)
target_link_libraries(${PROJECT_NAME} PRIVATE Oxylus)
target_include_directories(${PROJECT_NAME} PUBLIC ${Oxylus_SOURCE_DIR}/Oxylus/src)
