cmake_minimum_required(VERSION 3.0)

# netstack工程
project(netstack)

# 定义
add_definitions(-g -Wall)
include_directories(utils inc)

# 最终可执行文件
add_executable(netstack src/Main.cpp)
target_link_libraries(netstack stack utils)

# libnetstack.a静态库
aux_source_directory(src SRCS)
add_library(stack STATIC ${SRCS})
message("SRCS: " ${SRCS})

# libutils.a静态库
aux_source_directory(utils UTILS)
add_library(utils STATIC ${UTILS})
message("UTILS: " ${UTILS})

# 单元测试
enable_testing()
add_subdirectory(tests)
