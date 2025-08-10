cmake_minimum_required(VERSION 3.10)

# Project name and language
project(consh CXX)

# Set C standard (optional but recommended)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable
add_executable(consh main.cpp)
