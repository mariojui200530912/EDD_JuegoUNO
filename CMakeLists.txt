cmake_minimum_required(VERSION 4.1)
project(JuegoUNO)

set(CMAKE_CXX_STANDARD 20)

add_executable(JuegoUNO main.cpp
        include/entity/card.h
        include/structure/Stack.h
        src/logic/GeneratorDeck.cpp)
