cmake_minimum_required(VERSION 3.23)
project(TP_SistemasOperativos C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(TP_SistemasOperativos
        backend.c
        backend.h
        frontend.c
        frontend.h)
