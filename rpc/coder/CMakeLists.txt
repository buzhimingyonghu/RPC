cmake_minimum_required(VERSION 3.15)
project(coder)
link_directories(${LIBPATH})
link_libraries(
    ${COMMONLIB}
    ${TCPLIB}
    )
file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

set(LIBRARY_OUTPUT_PATH ${LIBPATH})
add_library(${CODERLIB} STATIC ${src})