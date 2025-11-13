cmake_minimum_required(VERSION 3.28)
project(MinesweeperProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        SYSTEM
)

FetchContent_MakeAvailable(SFML)



add_executable(MinesweeperApp
        src/main.cpp
        src/WelcomeWindow.cpp

        src/Tile.cpp

        src/GameWindow.cpp
)

target_compile_features(MinesweeperApp PRIVATE cxx_std_17)
target_link_libraries(MinesweeperApp PRIVATE SFML::Graphics)
