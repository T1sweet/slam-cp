# 要求cmake版本大于等于3.10.0
cmake_minimum_required(VERSION 3.10)

# 声明项目名称为slam_project
project(slam_project)

# 添加编译参数:警告信息、标准为C++11 优化等级为O2 带调试信息-g
# 注意1：要进行debug调试，必须要加上-g参数，否则gdb无法调试
# 注意2：优化等级-O2会影响调试，建议在调试阶段不要加上-O2参数，否则有些函数就进不去了
add_compile_options(-Wall -std=c++11 -g)

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 生成可执行文件
add_executable(main main.cpp ${PROJECT_SOURCE_DIR}/src/libHelloSLAM.cpp)

# # 生成静态库文件
# add_library(HelloSLAM ${PROJECT_SOURCE_DIR}/src/libHelloSLAM.cpp)

# # 生成动态库文件
# add_library(HelloSLAM_shared SHARED ${PROJECT_SOURCE_DIR}/src/libHelloSLAM.cpp)


