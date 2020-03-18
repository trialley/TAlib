# 公共股部分
cmake_minimum_required (VERSION 2.8.7)
project (TAlib)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra")
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/library)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(SOURCES_DIRECTORY ${PROJECT_SOURCE_DIR}/src)
set(INCLUDES_DIRECTORY ${PROJECT_SOURCE_DIR}/src)
set(EXAMPLE_DIRECTORY ${PROJECT_SOURCE_DIR}/example)

include_directories(${INCLUDES_DIRECTORY})
link_directories("${PROJECT_SOURCE_DIR}/library")

set(LOGGER__DIRECTORY ${SOURCES_DIRECTORY}/Logger)

foreach(dir ${LOGGER__DIRECTORY})
  # get directory sources and headers
  file(GLOB s_${dir} "${dir}/*.cpp")
  file(GLOB h_${dir} "${dir}/*.h")

  # set muiduo_network sources
  set(logger_sources ${s_${dir}} ${h_${dir}})
endforeach()
add_library(logger ${logger_sources})
target_link_libraries(logger pthread)


set(TALIB__DIRECTORY ${SOURCES_DIRECTORY}/)

foreach(dir ${TALIB__DIRECTORY})
  # get directory sources and headers
  file(GLOB s_${dir} "${dir}/*/*.cpp")
  file(GLOB h_${dir} "${dir}/*.h")

  # set muiduo_network sources
  set(talib_sources ${s_${dir}} ${h_${dir}})
endforeach()
add_library(TAlib ${talib_sources})
target_link_libraries(TAlib pthread mysqlclient  tcmalloc )
# 测试程序部分

add_subdirectory(${EXAMPLE_DIRECTORY}/)
