cmake_minimum_required(VERSION 3.15)
project(test)
include_directories(
    ${INCLUDE_PATH_COMMON}
    ${INCLUDE_PATH_NET}
)
link_directories(${LIBPATH})
link_libraries(${COMMONLIB})
link_libraries(${NETLIB})
set(EXECUTABLE_OUTPUT_PATH ${EXECPATH})
add_executable(test_eventloop test_eventloop.cpp)
