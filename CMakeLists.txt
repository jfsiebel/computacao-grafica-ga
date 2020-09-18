cmake_minimum_required(VERSION 3.15)
project(computacao-grafica-ga)

set(CMAKE_CXX_STANDARD 17)

add_executable(computacao-grafica-ga src/main.cpp src/Face.cpp src/Face.h src/Group.cpp src/Group.h src/Mesh.cpp src/Mesh.h src/Obj3D.cpp src/Obj3D.h src/OBJReader.cpp src/OBJReader.h src/Material.cpp src/Material.h)

find_library(GLFW_LIB libglfw.3.4.dylib /usr/local/lib)
find_library(GLEW_LIB libGLEW.2.2.0.dylib /usr/local/lib)

target_link_libraries(computacao-grafica-ga ${GLFW_LIB})
target_link_libraries(computacao-grafica-ga ${GLEW_LIB})

target_link_libraries(computacao-grafica-ga "-framework OpenGL")
target_link_libraries(computacao-grafica-ga "-framework CoreFoundation")
target_link_libraries(computacao-grafica-ga "-framework CoreVideo")
target_link_libraries(computacao-grafica-ga "-framework IOKit")
target_link_libraries(computacao-grafica-ga "-framework Cocoa")

include_directories(/usr/local/include)
include_directories(/usr/local/lib)