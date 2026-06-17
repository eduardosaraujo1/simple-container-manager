cmake_minimum_required(VERSION 3.16)

project(workspace LANGUAGES CXX)

include(FetchContent)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Test LinguistTools)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.9.0
)

FetchContent_MakeAvailable(yaml-cpp)

set(TS_FILES workspace_pt_BR.ts)

add_executable(workspace
  main.cpp
  ${TS_FILES}
  model/dockercli.h model/dockercli.cpp
  model/data-objects/containerinfo.h model/data-objects/containerinfo.cpp

  model/dockereventstream.h model/dockereventstream.cpp
  model/data-objects/dockerevent.h model/data-objects/dockerevent.cpp
  model/apppreferences.h model/apppreferences.cpp


)
target_link_libraries(workspace PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(workspace PRIVATE yaml-cpp)

target_include_directories(workspace PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

include(GNUInstallDirs)
install(TARGETS workspace
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
