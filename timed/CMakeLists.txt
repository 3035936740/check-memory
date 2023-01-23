# CMakeList.txt: timed 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.16)

# 将源代码添加到此项目的可执行文件。
add_executable (timed "timed.cpp" "timed.h" "common/utils.h"  "exception/standard_exception.h" "exception/file_exception.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET timed PROPERTY CXX_STANDARD 20)
endif()

#target_include_directories(timed public )

# TODO: 如有需要，请添加测试并安装目标。
