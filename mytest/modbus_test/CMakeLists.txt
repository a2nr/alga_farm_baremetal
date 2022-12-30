cmake_minimum_required(VERSION 3.14)

project(modbus_test LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

include(modbus.cmake)

add_library(plclib SHARED
  ../../lib/plclib/plclib.cpp
)

add_definitions(
    -DDEVICE_DEBUG
)

include_directories(
  ../../lib/device
  ../../lib/plclib
)

add_executable(modbus_test
  ../../lib/device/device.cpp
  main.cpp
)

target_link_libraries(modbus_test
    plclib
    modbus
    Qt${QT_VERSION_MAJOR}::Core)

install(TARGETS modbus_test
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
