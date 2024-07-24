# 指定cmake最小支持版本
cmake_minimum_required(VERSION 3.0)

# 项目名字
project(SearchEngine)

# 配置编译选项  ：-g、-O2
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#设定编译参数
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")

# 配置最终的可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

# 指定搜索的子目录
add_subdirectory(src)