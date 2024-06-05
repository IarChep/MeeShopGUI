cmake_minimum_required(VERSION 3.18.0)
project(cmake-simulator-qml LANGUAGES CXX)

find_package(Qt4 4.7.4 COMPONENTS QtCore QtGui QtDeclarative QtNetwork REQUIRED)

set(QML_IMPORT_PATH "/home/user/QtSDK/Simulator/Qt/gcc/imports/" CACHE STRING "")

file(GLOB src_files "src/*.cpp")
file(GLOB header_files "src/*.h")

set(qrc_files
    resources/resources.qrc
)

add_executable(example ${src_files} ${header_files} ${qrc_files})

set_target_properties(example PROPERTIES AUTOMOC ON AUTORCC ON)

target_compile_features(example PRIVATE cxx_std_20)

target_link_libraries(example PRIVATE Qt4::QtCore Qt4::QtGui Qt4::QtDeclarative Qt4::QtNetwork)
