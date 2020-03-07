# 1. 声明要求的cmake最低版本
cmake_minimum_required( VERSION 2.8 )
# 2. 添加c++11标准支持
set( CMAKE_CXX_FLAGS "-std=c++11" )
# 3. 声明一个cmake工程
PROJECT(WebServer)
MESSAGE(STATUS "Project: SERVER")
#打印相关消息消息
# 4. 头文件
file(GLOB WebServer_INC "*.h" "./*/*.h")
include_directories(${WebServer_INC})
# 5. 通过设定SRC变量，将源代码路径都给SRC，如果有多个，可以直接在后面继续添加
file(GLOB WebServer_SRC
    "*.cc" "./*/*.cc"
)

# 6. 创建共享库/静态库
# 设置路径（下面生成共享库的路径）

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# 即生成的共享库在工程文件夹下的lib文件夹中

set(LIB_NAME WebServer_lib)

# 创建共享库（把工程内的cpp文件都创建成共享库文件，方便通过头文件来调用）
# 这时候只需要cpp，不需要有主函数 
# ${PROJECT_NAME}是生成的库名 表示生成的共享库文件就叫做 lib工程名.so
# 也可以专门写cmakelists来编译一个没有主函数的程序来生成共享库，供其它程序使用
# SHARED为生成动态库，STATIC为生成静态库

add_library(${LIB_NAME} STATIC ${WebServer_SRC})

# 7. 链接库文件
# 把刚刚生成的${LIB_NAME}库和所需的其它库链接起来
# 如果需要链接其他的动态库，-l后接去除lib前缀和.so后缀的名称，以链接
# libpthread.so 为例,-lpthread
# 8. 编译主函数，生成可执行文件
# 先设置路径

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/)

# 可执行文件生成

add_executable(${PROJECT_NAME} main.cc)
target_link_libraries( ${LIB_NAME}  pthread
										)
# 这个可执行文件所需的库（一般就是刚刚生成的工程的库咯）
target_link_libraries( ${PROJECT_NAME}  pthread
										${LIB_NAME})