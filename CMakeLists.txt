cmake_minimum_required(VERSION 3.19)
project(simple-container-manager LANGUAGES CXX)
include(FetchContent)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.9.0
)

FetchContent_MakeAvailable(yaml-cpp)

qt_standard_project_setup()

qt_add_executable(simple-container-manager
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    model/dockercli.h model/dockercli.cpp
    model/data-objects/containerinfo.h model/data-objects/containerinfo.cpp

    model/dockereventstream.h model/dockereventstream.cpp
    model/data-objects/dockerevent.h model/data-objects/dockerevent.cpp
    model/apppreferences.h model/apppreferences.cpp
)

target_link_libraries(simple-container-manager
    PRIVATE
        Qt::Core
        Qt::Widgets
)
target_link_libraries(simple-container-manager PRIVATE yaml-cpp)

target_include_directories(simple-container-manager PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

install(TARGETS simple-container-manager
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET simple-container-manager
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
