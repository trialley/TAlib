cmake_minimum_required (VERSION 2.8.7)
project (AsyncLogging_test)

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




add_executable(example_server  ${SOURCES_DIRECTORY}/example_server.cpp)
target_link_libraries(example_server TAlib
                                     pthread)

add_executable(AsyncLogging_test  ${SOURCES_DIRECTORY}/AsyncLogging_test.cpp)
target_link_libraries(AsyncLogging_test    TAlib
                                            pthread
                                           )


add_executable(httpserver_test ${SOURCES_DIRECTORY}/httpserver.cpp)
target_link_libraries(httpserver_test    
								            TAlib
                                            pthread
                                           )