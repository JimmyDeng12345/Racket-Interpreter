cmake_minimum_required(VERSION 3.15)
project(interpreter C)

set(CMAKE_C_STANDARD 99)

add_executable(interpreter scan.c scan.h parsesexp.h parsesexp.c parsefr.h parsefr.c sub_interp.c)