cmake_minimum_required(VERSION 3.15)
project(test)

link_directories(${LIBPATH})
link_libraries(
    ${COMMONLIB}
    ${NETLIB}
    ${TCPLIB}
)
set(EXECUTABLE_OUTPUT_PATH ${EXECPATH})
add_executable(test_tcp test_tcp.cpp)
add_executable(test_client test_client.cpp)
