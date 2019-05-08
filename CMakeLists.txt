cmake_minimum_required(VERSION 3.12)
project(tank_assignment)



include_directories(../common)


set(CMAKE_CXX_STANDARD 14)

add_executable(tank_assignment
        main.cpp
        ../common/Shader.cpp
        ../common/Vector.cpp
        ../common/Matrix.cpp
        ../common/Mesh.cpp
        ../common/SphericalCameraManipulator.cpp
        ../common/Texture.cpp bullet.cpp bullet.h)


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)


target_link_libraries(tank_assignment GL ${GLUT_LIBRARY} GLEW -L/usr/lib/nvidia-352/)






