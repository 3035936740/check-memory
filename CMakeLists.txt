﻿# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#

cmake_minimum_required (VERSION 3.16)

#vcpkg
# 手动设置到你的vcpkg根目录
set(VCPKG_ROOT "/./home/ubuntu/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
CACHE STRING "Vcpkg toolchain file")

# cmake版本大于3.12使用C++20的标准

option(OPEN_CXX_20_VERSION "开启C++20" ON)

if (OPEN_CXX_20_VERSION)
	#if (CMAKE_VERSION VERSION_GREATER 3.12)
	  set(CMAKE_CXX_STANDARD 20)
	#endif()
endif()
project (timed VERSION 0.0.1 LANGUAGES C CXX)

# 输出项目信息
message(STATUS ${PROJECT_NAME} ${PROJECT_VERSION_MAJOR} ${PROJECT_VERSION_MINOR})

# 指定最终的可执行文件的位置
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

##### 添加三方库
# 设置第三方库编译文件输出路径
set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# 定义三方库变量及路径
set(CXX_FORMAT_PATH ./3rdparty/fmt-master)
set(SPDLOG_PATH     ./3rdparty/spdlog-1.x)
set(NLOHMANN_JSON_PATH     ./3rdparty/json-develop)

# 将三方库头文件加入项目
include_directories(${CXX_FORMAT_PATH}/include)
include_directories(${SPDLOG_PATH}/include)
include_directories(${NLOHMANN_JSON_PATH}/single_include)

# 将三方库加入项目作为子项目
add_subdirectory(${CXX_FORMAT_PATH})
add_subdirectory(${SPDLOG_PATH})
add_subdirectory(${NLOHMANN_JSON_PATH})
##### 结束

include_directories(./include)

# 将源代码添加到此项目的可执行文件。
add_executable (${PROJECT_NAME} "./timed.cpp" "./timed.h" "common/utils.h"  "./exception/standard_exception.h" "./exception/file_exception.h")

# 将配置文件放置到二进制文件目录
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# 链接第三方库(必须在add_executable后面)
#find_package(jsoncpp CONFIG REQUIRED)
#JsonCpp::JsonCpp
target_link_libraries(${PROJECT_NAME} fmt spdlog nlohmann_json)