cmake_minimum_required(VERSION 3.15)
project(rpc)

link_directories(${LIBPATH})
link_libraries(
    ${COMMONLIB}
    ${TCPLIB}
    ${CODERLIB}
    ${NETLIB}
    )
file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(LIBRARY_OUTPUT_PATH ${LIBPATH})
add_library(${RPCLIB} STATIC ${src})