cmake_minimum_required (VERSION 2.8.7)
project (AsyncLogging_example)

###
# variables
###
set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR}/../)
set(SOURCES_DIRECTORY ./)
set(INCLUDES_DIRECTORY ../src)

###
# includes
###
include_directories(
    .
    ${INCLUDES_DIRECTORY}
)

###
# libraries
###
link_directories("${PROJECT_ROOT_DIR}/library")

###
# outputs
###

set(LIBRARY_OUTPUT_PATH ${PROJECT_ROOT_DIR}/library)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_ROOT_DIR}/bin)

###
# compilation options
###
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra -O3")




add_executable(tcp_server_example  ${SOURCES_DIRECTORY}/tcp_server_example.cpp)
target_link_libraries(tcp_server_example TAlib
                                     pthread)

add_executable(AsyncLogging_example  ${SOURCES_DIRECTORY}/AsyncLogging_example.cpp)
target_link_libraries(AsyncLogging_example    TAlib
                                            pthread
                                           )


add_executable(http_server_example ${SOURCES_DIRECTORY}/http_server_example.cpp)
target_link_libraries(http_server_example    
								            TAlib
                                            pthread
                                           )