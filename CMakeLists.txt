cmake_minimum_required(VERSION 3.1)
project(HapaxLegomena VERSION 1.0)

add_executable(main source/main.cpp)

target_compile_features(main PRIVATE cxx_std_17)


