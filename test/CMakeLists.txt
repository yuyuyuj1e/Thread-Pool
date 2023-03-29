# 添加源文件到自定义的变量中
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ TEST_LIST)

# 指定生成可执行文件
add_executable(normal_test ${SRC_LIST} ${TEST_LIST})
add_executable(shared_test ${SRC_LIST} ${TEST_LIST})

# 测试线程池
target_link_libraries(normal_test PRIVATE pthread)

# 测试动态库
link_directories(${CMAKE_SOURCE_DIR}/bin)
target_link_libraries(shared_test PRIVATE pthread threadpool)
