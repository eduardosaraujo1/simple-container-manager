cmake_minimum_required(VERSION 3.16)

project(workspace LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core LinguistTools)

set(TS_FILES workspace_pt_BR.ts)

add_executable(workspace
  main.cpp
  ${TS_FILES}
  model/dockercli.h model/dockercli.cpp
  model/data-objects/containerinfo.h model/data-objects/containerinfo.cpp


  model/dockereventstream.h model/dockereventstream.cpp
  model/data-objects/dockerevent.h model/data-objects/dockerevent.cpp



)
target_link_libraries(workspace PRIVATE Qt${QT_VERSION_MAJOR}::Core)
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
