# 指定搜索的子目录
aux_source_directory(. SRC_LIST)
aux_source_directory(./Public PUBLIC_SRC_LIST)
aux_source_directory(./Reactor REACTOR_SRC_LIST)
aux_source_directory(./Key KEY_SRC_LIST)
aux_source_directory(./Web WEB_SRC_LIST)
aux_source_directory(../lib LIB_SRC_LIST)

# 指定生成的可执行文件
add_executable(server ${SRC_LIST} ${PUBLIC_SRC_LIST} ${REACTOR_SRC_LIST} ${KEY_SRC_LIST} ${WEB_SRC_LIST} ${LIB_SRC_LIST})

# 指定可执行文件链接时所依赖的库文件
target_link_libraries(server boost_system log4cpp pthread json)