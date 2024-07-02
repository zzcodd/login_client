cmake_minimum_required(VERSION 3.0)
project(PASSWD_MYSQL)

aux_source_directory(. SRCS)
add_executable(myLoginClient ${SRCS})

set(MYSQL_LIBRARY "/usr/lib/x86_64-linux-gnu/libmysqlclient.so") # 根据你的系统修改这个路径
target_link_libraries(myLoginClient ${MYSQL_LIBRARY})




