cmake_minimum_required(VERSION 3.21)

project(HpdBoardTestHost VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Network SerialPort)

qt_standard_project_setup()

qt_add_executable(HpdBoardTestHost WIN32
    src/DspTcpClient.cpp
    src/DspTcpClient.h
    src/DspTestProtocol.cpp
    src/DspTestProtocol.h
    src/EthernetTcpTestClient.cpp
    src/EthernetTcpTestClient.h
    src/MainWindow.cpp
    src/MainWindow.h
    src/ScibSerialClient.cpp
    src/ScibSerialClient.h
    src/ZlgCanClient.cpp
    src/ZlgCanClient.h
    src/main.cpp
)

target_link_libraries(HpdBoardTestHost
    PRIVATE
        Qt6::Network
        Qt6::SerialPort
        Qt6::Widgets
)

set_target_properties(HpdBoardTestHost PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(WIN32)
    add_custom_command(TARGET HpdBoardTestHost POST_BUILD
        COMMAND powershell.exe -NoProfile -ExecutionPolicy Bypass
                -File "${CMAKE_CURRENT_SOURCE_DIR}/Tools/build_can_bridge32.ps1"
                -OutputDirectory "$<TARGET_FILE_DIR:HpdBoardTestHost>"
        VERBATIM
    )
endif()
