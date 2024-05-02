cmake_minimum_required(VERSION 3.15)
project(tcp)
link_directories(${LIBPATH})
link_libraries(
    ${COMMONLIB}
    ${NETLIB}
    ${CODERLIB}
    ${RPCLIB}
)
file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(LIBRARY_OUTPUT_PATH ${LIBPATH})
add_library(${TCPLIB} STATIC ${src})