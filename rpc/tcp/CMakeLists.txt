cmake_minimum_required(VERSION 3.15)
project(tcp)
include_directories(
    ${INCLUDE_PATH_COMMON}
    ${INCLUDE_PATH_NET}
)
link_directories(${LIBPATH})
link_libraries(
    ${COMMONLIB}
    ${NETLIB}
)
file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(LIBRARY_OUTPUT_PATH ${LIBPATH})
add_library(${TCPLIB} STATIC ${src})