# 创建示例程序
add_executable(basic_usage basic_usage.cpp)
target_link_libraries(basic_usage PRIVATE ccjson)

add_executable(json_file_io json_file_io.cpp)
target_link_libraries(json_file_io PRIVATE ccjson)

add_executable(json_to_struct json_to_struct.cpp)
target_link_libraries(json_to_struct PRIVATE ccjson)

add_executable(test_json_suit test_json_suit.cc)
target_link_libraries(test_json_suit PRIVATE ccjson)

# 复制示例文件到构建目录
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/twitter.json
               ${CMAKE_BINARY_DIR}/examples/twitter.json COPYONLY)