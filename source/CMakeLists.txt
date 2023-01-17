# Specify the minimum version.
cmake_minimum_required(VERSION 3.5)

find_package(Lua51 REQUIRED)

include_directories(${LUA_INCLUDE_DIR})

# Specify the project info.
project(melkor VERSION 1.0.0 DESCRIPTION "C/Lua WAD Library")

# Declare the library target.
add_library(${PROJECT_NAME} SHARED
    wad.cc
    wad.hh
    wad.lua.cc
    wad.lua.hh
)

# Configure the directories to search for header files.
target_include_directories(${PROJECT_NAME} PRIVATE source)

# Link lua libraries
target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})

# Set the version property.
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

# Set the shared object version property to the project's major version.
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

# Set the public header property to the one with the actual API.
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER source/convert.h)
