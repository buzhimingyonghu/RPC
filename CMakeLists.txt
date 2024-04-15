cmake_minimum_required(VERSION 3.15)
project(test)
#静态库的生成路劲
set(LIBPATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
#可执行程序的存储目录
set(EXECPATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#库文件的名字
set(COMMONLIB common)
set(NETLIB net)
#头文件地址
set(INCLUDE_PATH_COMMON ${CMAKE_CURRENT_SOURCE_DIR}/rpc/common)
set(INCLUDE_PATH_NET ${CMAKE_CURRENT_SOURCE_DIR}/rpc/net)


add_subdirectory(rpc/common)
add_subdirectory(testcases)
add_subdirectory(rpc/net)



