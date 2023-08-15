cmake_minimum_required(VERSION 3.14)

project(Sequential_Archive LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wdeprecated -Werror")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(SOURCES
    Sequential_Archive.cpp
    Packer.cpp
    Scrambler.cpp
    Reader.cpp
)

set(HEADERS
    Sequential_Archive.h
    Sequential_Archive_Interface.h
    Packer.h
    Scrambler.h
    Common_Data.h
    Reader.h
)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
