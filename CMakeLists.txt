cmake_minimum_required(VERSION 3.18.0)
project(cmake-simulator-qml LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_N9 "Build MeeShop for n9" off)

find_package(Qt4 4.7.4 COMPONENTS QtCore QtGui QtDeclarative QtNetwork QtSvg REQUIRED)
find_package(X11 REQUIRED)
find_package(nlohmann_json REQUIRED)

if(BUILD_N9)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -Wl,--dynamic-linker,/lib/ld-linux.so.3")
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

set(qrc_files
    resources/resources.qrc
)

add_executable(meeshop ${SOURCES} ${HEADERS} ${qrc_files})

set_target_properties(meeshop PROPERTIES AUTOMOC ON AUTORCC ON)

target_link_libraries(meeshop PRIVATE Qt4::QtCore Qt4::QtGui Qt4::QtDeclarative Qt4::QtNetwork Qt4::QtSvg nlohmann_json::nlohmann_json X11::X11)
