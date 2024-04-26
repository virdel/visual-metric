
set(CMAKE_TOOLCHAIN_FILE "E:/git/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
cmake_minimum_required(VERSION 3.0)
project(normalMetric)

include_directories("../dependency/include")
include_directories(${ZLIB_INCLUDE_DIRS})

find_library(glfw3
            glfw3
            PATHS "../dependency/lib")

find_library(assimp
            assimp-vc143-mtd
            PATHS "../dependency/lib")

find_package(ZLIB REQUIRED)


add_compile_definitions(_CRT_SECURE_NO_WARNINGS) 
add_compile_definitions(_USE_MATH_DEFINES) 

aux_source_directory(. SRCS)
set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_BUILD_TYPE "Release")
add_executable(normalMetric "${SRCS}")

if(glfw3)
  target_link_libraries(normalMetric PRIVATE ${glfw3}) # ʹ��find_library�ҵ��Ŀ�
else()
  message(FATAL_ERROR "Could not find MyStaticLib.lib")
endif()

if(assimp)
  target_link_libraries(normalMetric PRIVATE ${assimp}) # ʹ��find_library�ҵ��Ŀ�
else()
  message(FATAL_ERROR "Could not find MyStaticLib.lib")
endif()
if(ZLIB_FOUND)
    target_link_libraries(normalMetric PRIVATE ${ZLIB_LIBRARIES})
else()
    message(FATAL_ERROR "Could not find Zlib library")
endif()



file(GLOB ASSESTS  "*.vs" "*.fs")
message("Assets: ${ASSETS}")

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
message("当前构建类型是: ${CMAKE_BUILD_TYPE}")
    file(COPY ${ASSETS} DESTINATION "${CMAKE_BINARY_DIR}")
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
message("当前构建类型是: ${CMAKE_BUILD_TYPE}")
    file(COPY ${ASSETS} DESTINATION "${CMAKE_BINARY_DIR}")
else()
    message("当前构建类型是未知或自定义类型: ${CMAKE_BUILD_TYPE}")
endif()


