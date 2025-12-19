cmake_minimum_required(VERSION 3.16)
project(compiler LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 源文件列表（你也可以用 GLOB，但推荐显式写出）
set(SOURCES
    src/lexical_analyzer.cpp
    src/error_reporter.cpp
    src/symbol_table.cpp
    src/token.cpp
    include/exception.hpp
    src/ll1_analyzer.cpp
    src/parse_tree.cpp
    src/ll1_parser.cpp
    src/slr1_analyzer.cpp
    src/slr1_parser.cpp
    src/main.cpp
)

# 头文件目录（告诉编译器 include/ 是合法搜索路径）
include_directories(${CMAKE_SOURCE_DIR}/include)

# 生成可执行文件
add_executable(compiler ${SOURCES})

# （可选）编译警告设置
if (MSVC)
    target_compile_options(compiler PRIVATE /W4)
else()
    target_compile_options(compiler PRIVATE -Wall -Wextra -pedantic)
endif()
