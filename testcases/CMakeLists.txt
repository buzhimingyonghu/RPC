cmake_minimum_required(VERSION 3.15)
project(test)

add_library(proto STATIC order.pb.cc)
target_link_libraries(proto protobuf)

include_directories(/usr/local/include/rpc)
link_directories(/usr/local/lib/)
link_libraries(
    mircorpc
    proto
    tinyxml
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
add_executable(test_rpc_server test_rpc_server.cc )
add_executable(test_rpc_client test_rpc_client.cc )
