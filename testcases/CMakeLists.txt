cmake_minimum_required(VERSION 3.15)
project(test)

set(LIBRARY_OUTPUT_PATH ${LIBPATH})
add_library(proto STATIC order.pb.cc)
target_link_libraries(proto protobuf)


link_directories(${LIBPATH})
link_libraries(
    ${COMMONLIB}
    ${TCPLIB}
    ${CODERLIB}
    ${NETLIB}
    ${RPCLIB}
    proto
)

set(EXECUTABLE_OUTPUT_PATH ${EXECPATH})
add_executable(test_rpc_server test_rpc_server.cpp )
add_executable(test_rpc_client test_rpc_client.cpp )
