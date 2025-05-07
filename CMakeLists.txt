cmake_minimum_required(VERSION 3.18.0)

project(MeeShop LANGUAGES CXX)

option(BUILD_N9 "Build MeeShop for n9" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Enforce the standard
set(QML_IMPORT_PATH "/home/iarchep/QtSDK/Simulator/Qt/gcc/imports" CACHE STRING "Path to QML imports") # Added a description

find_package(Qt4 4.7.4 COMPONENTS QtCore QtGui QtDeclarative QtNetwork QtSvg REQUIRED)
find_package(X11 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

set(RESOURCES
    resources/resources.qrc
)

add_executable(meeshop ${SOURCES} ${HEADERS} ${RESOURCES})

set_target_properties(meeshop PROPERTIES AUTOMOC ON AUTORCC ON)

target_link_libraries(meeshop PRIVATE Qt4::QtCore Qt4::QtDBus Qt4::QtGui Qt4::QtDeclarative Qt4::QtNetwork Qt4::QtSvg nlohmann_json::nlohmann_json Threads::Threads X11::X11 util)

if (BUILD_N9)
    pkg_check_modules(boostable ${CMAKE_SYSROOT}/usr/lib/pkgconfig/qdeclarative-boostable.pc REQUIRED)
    target_compile_definitions(meeshop PRIVATE HARMATTAN_BOOSTER)
    target_compile_options(meeshop PRIVATE ${boostable_CFLAGS})
    target_include_directories(meeshop PRIVATE ${CMAKE_SYSROOT}${boostable_INCLUDE_DIRS})
    target_link_options(meeshop PRIVATE -static-libstdc++ -Wl,--dynamic-linker,/lib/ld-linux.so.3 -pie -rdynamic -lmdeclarativecache)
    # Copy the entire debian directory
    add_custom_target(copy_debian_dir
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/debian"
        "${CMAKE_BINARY_DIR}/debian"
        COMMENT "Copying debian directory"
    )

    install(FILES resources/meeshop_harmattan.desktop DESTINATION /usr/share/applications)
    install(FILES resources/meeshop80.png DESTINATION /usr/share/icons/hicolor/80x80/apps)
    install(FILES resources/meeshop-splash.png DESTINATION /opt/meeshop/share)
    install(FILES resources/openrepos.key DESTINATION /opt/meeshop)

    install(TARGETS meeshop DESTINATION /opt/meeshop/bin)

    add_custom_target(package
        COMMAND /home/iarchep/QtSDK/Madde/bin/mad -t harmattan_10.2011.34-1_rt1.2 dpkg-buildpackage -nc -uc -us
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building Debian package with mad..."
        DEPENDS meeshop copy_debian_dir #Ensures meeshop is built before packaging
    )
endif()
