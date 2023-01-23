# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

# cmake版本大于3.12使用C++20的标准

option(OPEN_CXX_20_VERSION "开启C++20" ON)

if (OPEN_CXX_20_VERSION)
	if (CMAKE_VERSION VERSION_GREATER 3.12)
	  set(CMAKE_CXX_STANDARD 20)
	endif()
endif()
project ("timed" VERSION 0.0.1 LANGUAGES C CXX)

# 输出项目信息
message(STATUS ${PROJECT_NAME} ${PROJECT_VERSION_MAJOR} ${PROJECT_VERSION_MINOR})

# 添加三方库
include_directories(./include)

# 包含子项目。
add_subdirectory ("timed")