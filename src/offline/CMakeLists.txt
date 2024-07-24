# 定义了一个变量SRC_LIST, 包含当前目录下的所有源文件 
aux_source_directory(. SRC_LIST)
aux_source_directory(.. PUBLIC_SRC_LIST)

# 指定生成的可执行文件
add_executable(offline ${SRC_LIST} ${PUBLIC_SRC_LIST})

# 指定可执行文件链接时所依赖的库文件
target_link_libraries(offline boost_system log4cpp pthread)