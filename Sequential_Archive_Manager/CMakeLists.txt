cmake_minimum_required(VERSION 3.14)

project(Sequential_Archive_Manager LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wdeprecated -Werror")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(SOURCES
    main.cpp
    Main_Window.cpp
    Viewer.cpp
    Error_Messages.cpp
    Command_Line_Runner.cpp
    ../../C_Common_Code/Qt/Qt_App_Theme_Fixes/Fusion_Fixes.cpp
)

set(HEADERS
    Main_Window.h
    Viewer.h
    Common_Strings.h
    Error_Messages.h
    Version.h
    Command_Line_Runner.h
    ../../C_Common_Code/Qt/Qt_App_Theme_Fixes/Fusion_Fixes.h
)

set(FORMS
    Main_Window.ui
    Viewer.ui
)

set(RC_FILE Sequential_Archive.rc)

# Remove WIN32 if you want the console window
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${FORMS} ${RC_FILE})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
