cmake_minimum_required(VERSION 3.16)

project(simple-container-manager LANGUAGES CXX)

include(FetchContent)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Test LinguistTools)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.9.0
)

FetchContent_MakeAvailable(yaml-cpp)

set(TS_FILES workspace_pt_BR.ts)

add_executable(containermanager
  main.cpp
  ${TS_FILES}
  data/dockercli.h data/dockercli.cpp
  data/data-objects/containerinfo.h data/data-objects/containerinfo.cpp

  data/dockereventstream.h data/dockereventstream.cpp
  data/data-objects/dockerevent.h data/data-objects/dockerevent.cpp
  data/apppreferences.h data/apppreferences.cpp


)
target_link_libraries(containermanager PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(containermanager PRIVATE yaml-cpp)

target_include_directories(containermanager PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

include(GNUInstallDirs)
install(TARGETS containermanager
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
