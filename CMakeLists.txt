#cmake_minimum_required(VERSION 3.5)

#project(miniz LANGUAGES CXX C)

#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include_directories(/home/wzy/untitled)
#add_executable(app
#    main.cpp
#    mainwindow.cpp
#    mainwindow.h
#    mainwindow.ui
#    buttonedit.cpp
##    #example1.c
#    miniz.c miniz_zip.c miniz_tinfl.c miniz_tdef.c
#    )

#set(MINIZ_API_VERSION 2)
#set(MINIZ_MINOR_VERSION 1)
#set(MINIZ_PATCH_VERSION 0)
#set(MINIZ_VERSION ${MINIZ_API_VERSION}.${MINIZ_MINOR_VERSION}.${MINIZ_PATCH_VERSION})

#option(BUILD_EXAMPLES "Build examples" ON)
#option(AMALGAMATE_SOURCES "Amalgamate sources into miniz.h/c" OFF)
#option(BUILD_HEADER_ONLY "Build a header-only version" OFF)
#option(BUILD_SHARED_LIBS "Build shared library instead of static" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
include_directories(/home/wzy/untitled)

cmake_minimum_required(VERSION 3.5)

project(miniz LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(app
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  buttonedit.cpp
  miniz.c miniz_zip.c miniz_tinfl.c miniz_tdef.c
)

target_link_libraries(app PRIVATE Qt5::Widgets)


