# 指定搜索的子目录
aux_source_directory(. SRC_LIST)

# 指定生成的可执行文件
add_executable(client ${SRC_LIST})

# 指定可执行文件链接时所依赖的库文件
target_link_libraries(client json)