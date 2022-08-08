
# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (frame)


set(CMAKE_BUILD_TYPE "Debug") #Release 如果是发行版本就更换Debug为Release 
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-D_DEBUG) #添加D_DEBUG编译参数
endif()


#设置编译参数
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lpthread -std=gnu99 -g -o0") #
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -W -Wall -Wextra -g2 -ggdb -o0 -lpthread")

file(GLOB SOURCES "factory/*/.cc" "factory/*/*.cpp"
#	"dag/*/*.cpp" "dag/*.cpp"
	"common/*/*.cpp" "common/*.cpp")

FILE(GLOB_RECURSE SOURCES_HEADER "common/*.h" "common/*/*.h"
#	"dag/*/*/h" "dag/*.h"
	"common/*/*.h" "common/*.h")

INCLUDE_DIRECTORIES(./include)
#查找include目录下的所有*.hpp,*.h头文件,并路径列表保存到 INCLUDE_DIR_LIST 变量中
# 调用函数，指定参数
FIND_INCLUDE_DIR(INCLUDE_DIR_LIST ${PROJECT_SOURCE_DIR}/context)

#将INCLUDE_DIR_LIST中路径列表加入工程
include_directories(
        ${INCLUDE_DIR_LIST}
	${SOURCES_HEADER}
)

#将./下的所有*.cpp加入到SRC_LIST中
# AUX_SOURCE_DIRECTORY(./dag SRC_LIST) 
# AUX_SOURCE_DIRECTORY(./common SRC_LIST) 
# AUX_SOURCE_DIRECTORY(./factory SRC_LIST) 
# 添加 math 子目录
add_subdirectory(dag)


#添加静态库目录 它相当于g++命令的-L选项的作用，也相当于环境变量中增加LD_LIBRARY_PATH的路径的作用。
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdparty)
# 添加链接库 cmake代码说明：在生成名字为main的程序时将会去链接libcomm.a这个库。
target_link_libraries(Demo LEVELDB_LIB)  # 显示指定链接静态库

# 指定生成目标
add_executable(Demo ${SRC_LIST} ${SOURCES})





