cmake_minimum_required(VERSION 3.10)

project(Commanche2D)

option(ENGINE_EDITOR "Compile with editor" OFF)

set(IMGUI_DEFINE_MATH_OPERATORS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wfatal-errors -g -Wno-unused-variable")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_OBJC_FLAGS "${CMAKE_OBJC_FLAGS} -x objective-c")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(/W4 /EHsc)
endif()

add_definitions(-DPLATFORM_DESKTOP)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_subdirectory(libs/nativefiledialog-extended)
add_subdirectory(libs/native_message_box)
add_subdirectory(libs/flecs)

include_directories(
    "./src"
    "./libs"
    "./libs/angle/include"
    "./libs/imgui"
    "./libs/box2d/include"
    "./libs/imgui/backends"
    "./libs/SimpleJSON"
    "/opt/homebrew/include"
    "./libs/glad/include"
    "./libs/stb"
    "./libs/flecs/include"
    "./libs/nativefiledialog-extended/src/include"
    "./libs/native_message_box/include"
)

# Core Sources
file(GLOB ENGINE_SOURCES
    "./src/*.cpp"
    "./src/game/*.cpp"
    "./src/io/*.cpp"
    "./src/ecs/*.cpp"
    "./src/log/*.cpp"
    "./src/core/*.cpp"
    "./src/render/*.cpp"
    "./src/physics/*.cpp"
    "./src/systems/*.cpp"
    "./src/scene/*.cpp"
    "./src/common/*.cpp"
    "./libs/flecs/*.c"
    "./src/assetmgr/*.cpp"
    "./libs/SimpleJSON/*.cpp"
    "./libs/imgui/*.cpp"
    "./libs/box2d/src/*/*.cpp"
)

# Platform specific sources
if(APPLE)
        set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/platform/osx/*.mm PROPERTIES LANGUAGE OBJCXX)
        file(GLOB PLATFORM_SOURCES "./src/platform/OSX/*.mm")
        list(APPEND ENGINE_SOURCES ${PLATFORM_SOURCES})
endif()

set(SOURCES ${ENGINE_SOURCES})

# Rendering Backend

add_definitions(-DRENDER_BACKEND_OPENGL=1)
find_package(glfw3 CONFIG REQUIRED)
find_package(Freetype REQUIRED)

file(GLOB OPENGL_SOURCES 
	"./src/io/backends/opengl/*.cpp"
	"./src/render/backends/opengl/*.cpp"
	"./libs/glad/src/*.c"
	"./src/assetmgr/backends/opengl/*.cpp")

list(APPEND SOURCES ${OPENGL_SOURCES})


# Editor Sources
if(ENGINE_EDITOR)
	add_definitions(-DEDITOR=1)
	file(GLOB EDITOR_SOURCES
		"./src/editor/*.cpp"
		"./src/editor/log_sinks/*.cpp"
		"./src/editor/systems/*.cpp")

	file(GLOB EDITOR_DEPS
		"./libs/imgui/backends/imgui_impl_glfw.cpp"
		"./libs/imgui/backends/imgui_impl_opengl3.cpp")

	list(APPEND EDITOR_SOURCES ${EDITOR_DEPS})

	list(APPEND SOURCES ${EDITOR_SOURCES})
endif()
add_executable(${PROJECT_NAME} ${SOURCES})

set(COMMON_LIBS nfd glm::glm glfw assimp::assimp)

list(APPEND COMMON_LIBS Freetype::Freetype)

target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${COMMON_LIBS})

if(APPLE)
    set(APPLE_FRAMEWORKS 
        "-framework IOKit" 
        "-framework Appkit" 
        "-framework UniformTypeIdentifiers" 
        "-framework Cocoa"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE ${APPLE_FRAMEWORKS})
endif()
