add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE argparser)
target_link_libraries(${PROJECT_NAME} PRIVATE commands)
target_link_libraries(${PROJECT_NAME} PRIVATE encode_file)
target_link_libraries(${PROJECT_NAME} PRIVATE run)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})