cmake_minimum_required(VERSION 4.1)
project(JuegoUNO)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "*.cpp")

add_executable(JuegoUNO
        main.cpp
        include/entity/Card.h
        include/entity/Player.h
        src/entity/Player.cpp
        include/structure/Stack.h
        src/structure/Stack.cpp
        include/structure/DLinkedList.h
        src/structure/DLinkedList.cpp
        include/structure/DCircularList.h
        src/structure/DCircularList.cpp
        src/logic/GeneratorDeck.cpp
        include/logic/Rules.h
        src/logic/Rules.cpp
        include/logic/GeneratorDeck.h
        include/logic/Game.h
        src/logic/Game.cpp
        include/utils/ConsoleColors.h
        include/view/GameUI.h
        src/view/GameUI.cpp
        include/logic/GameSettings.h
        src/logic/GameSettings.cpp
)