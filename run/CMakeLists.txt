add_library(run run.cpp)

target_link_libraries(run PRIVATE argparser)
target_link_libraries(run PRIVATE commands)
target_link_libraries(run PRIVATE encode_file)

target_include_directories(run PUBLIC ${PROJECT_SOURCE_DIR})