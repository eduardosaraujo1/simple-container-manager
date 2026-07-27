cmake_minimum_required(VERSION 3.19)
project(simple-container-manager LANGUAGES CXX)
include(FetchContent)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Svg)

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

    data/data-objects/containerstate.h data/data-objects/containerstate.cpp
    data/data-objects/dockerevent.h data/data-objects/dockerevent.cpp
    data/data-objects/containerdefinition.h data/data-objects/containerdefinition.cpp
    data/data-objects/configuredcontainers.h data/data-objects/configuredcontainers.cpp
    data/dockercli.h data/dockercli.cpp
    data/dockereventstream.h data/dockereventstream.cpp
    data/appconfigreader.h data/appconfigreader.cpp


    domain/containerservice.h domain/containerservice.cpp

    containerrowwidget.h containerrowwidget.cpp containerrowwidget.ui
    containerlistwidget.h containerlistwidget.cpp containerlistwidget.ui

)

qt_add_resources(simple-container-manager "app_images"
    PREFIX "/"
    FILES
        images/refresh.svg
        images/terminal.svg
)

target_link_libraries(simple-container-manager
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Svg
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
