cmake_minimum_required(VERSION 3.5)
project(VowelDeformer VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(main
   source/main.cpp
   source/PhoneticAlphabet.cpp
)

# Include directories for header files
target_include_directories(main
    PRIVATE
    ${PROJECT_SOURCE_DIR}/source  # Assuming headers are in the same directory as the source
)


