cmake_minimum_required(VERSION 3.20)
project(lw_1)

set(CMAKE_CXX_STANDARD 11)

# Добавьте все ваши исходные файлы
add_executable(lw_1 lw.cpp)
# или если файлов несколько:
# add_executable(lw_1 main.cpp file1.cpp file2.cpp)