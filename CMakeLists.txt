cmake_minimum_required(VERSION 3.27)
project(8080Disassembler C)

set(CMAKE_C_STANDARD 11)

add_executable(8080Disassembler 8080dis.c
        8080emu.h
)
add_executable(8080Emulator 8080emu.c)