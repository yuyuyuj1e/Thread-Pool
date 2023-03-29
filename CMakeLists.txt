# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.0) 

# 设置项目名称
project(ThreadPool LANGUAGES CXX)

# 设置 C++11 标准
set(CMAKE_CXX_STANDARD 11)

# 引入头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加源文件到自定义的变量中
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)

# 设置可执行文件存放路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 构建静态库
add_library(threadpool_static STATIC ${SRC_LIST})
# 将 threadpool_static 重命名为 threadpool
set_target_properties(threadpool_static PROPERTIES OUTPUT_NAME "threadpool")
# cmake 在构建一个新的 target 时，会尝试清理掉使用这个名称的库，所以在构建 libthreadpool.so 时，就会清理掉 libthreadpool.a
set_target_properties(threadpool_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# 构建动态库
add_library(threadpool SHARED ${SRC_LIST})
set_target_properties(threadpool PROPERTIES OUTPUT_NAME "threadpool")
set_target_properties(threadpool PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# 静态库和动态库的路径
set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)

# 添加子目录
add_subdirectory(${PROJECT_SOURCE_DIR}/test)
