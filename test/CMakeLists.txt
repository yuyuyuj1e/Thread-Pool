# 添加源文件到自定义的变量中
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ TEST_LIST)

# 该命令必须放在生成可执行文件之前
# 指定要链接的动态库的路径
link_directories(${PROJECT_SOURCE_DIR}/bin)

# 指定生成可执行文件
add_executable(normal_test ${SRC_LIST} ${TEST_LIST})
add_executable(shared_test ${SRC_LIST} ${TEST_LIST})

# 测试线程池
target_link_libraries(normal_test PRIVATE pthread jsoncpp)

# 测试动态库
target_link_libraries(shared_test PRIVATE pthread threadpool jsoncpp)
