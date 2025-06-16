# 添加性能测试
add_executable(performance_test performance_test.cpp)
target_link_libraries(performance_test PRIVATE ccjson)

# 添加平台特定的库
if(WIN32)
    target_link_libraries(performance_test PRIVATE psapi)
else()
    target_link_libraries(performance_test PRIVATE rt)
endif() 

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/twitter.json
               ${CMAKE_BINARY_DIR}/test/twitter.json COPYONLY)