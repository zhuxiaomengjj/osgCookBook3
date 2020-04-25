cmake_minimum_required(VERSION 2.8.11)
project(osgcookbook1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON)

#find_package(Qt5 REQUIRED COMPONENTS Core Gui OpenGL)
find_package(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer osgText)

include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    )

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    ${OPENSCENEGRAPH_LIBRARIES}
    )
