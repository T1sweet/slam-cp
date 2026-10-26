# 要求cmake版本大于等于3.10.0
cmake_minimum_required(VERSION 3.10)

# 声明项目名称为slam_project
project(slam_project)

# 添加编译参数:警告信息、标准为C++11 优化等级为O2 带调试信息-g
# 注意1：要进行debug调试，必须要加上-g参数，否则gdb无法调试
# 注意2：优化等级-O2会影响调试，建议在调试阶段不要加上-O2参数，否则有些函数就进不去了
# add_compile_options(-Wall -std=c++14 -g)
# 修改为： 强制使用 C++14 标准 (解决 sigslot 的 enable_if 报错)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 强制使用C++14标准
add_compile_options(-Wall -g)

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories("/usr/include/eigen3")    # 添加Eigen库的头文件目录

# 注意：eigen库比较好找，容易可以直接找到，可以写死位置
# 但Pangolin库比较难找，而且更换系统后又会变化，所以需要用动态查找的方式
# ================== 寻找并添加依赖库 ==================
find_package(Pangolin REQUIRED)  # 只找 Pangolin（Eigen 已经写死，Pangolin 会自动带出 OpenGL 和 epoxy）
include_directories(${Pangolin_INCLUDE_DIRS})   # 添加Pangolin到头文件目录

# 生成可执行文件
add_executable(main main.cpp)

# ================== 链接库 ==================
target_link_libraries(main
    ${Pangolin_LIBRARIES}
)