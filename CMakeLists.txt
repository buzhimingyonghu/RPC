cmake_minimum_required(VERSION 3.15)
project(test)
#静态库的生成路劲
set(LIBPATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

#库文件的名字

set(MIRCORPC mircorpc)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#头文件地址
set(PATH_COMMON ${CMAKE_CURRENT_SOURCE_DIR}/rpc/common)
set(PATH_NET ${CMAKE_CURRENT_SOURCE_DIR}/rpc/net)
set(PATH_TCP ${CMAKE_CURRENT_SOURCE_DIR}/rpc/tcp)
set(PATH_CODER ${CMAKE_CURRENT_SOURCE_DIR}/rpc/coder)
set(PATH_RPC ${CMAKE_CURRENT_SOURCE_DIR}/rpc/rpc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rpc)

file(GLOB SOURCES  
${PATH_COMMON}/*.cc
${PATH_NET}/*.cc
${PATH_TCP}/*.cc
${PATH_CODER}/*.cc
${PATH_RPC}/*.cc
)
link_libraries(tinyxml)
set(LIBRARY_OUTPUT_PATH ${LIBPATH})
add_library(${MIRCORPC} STATIC ${SOURCES})


