cmake_minimum_required(VERSION 3.10)

project(BoundingBoxMe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Eigen
find_package(Eigen3 REQUIRED)

# Qt dependancy
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# VTK dependancy
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})



# The main application
add_executable(BoundingBoxMe
    assets/resources.qrc
    lidarFrame.cxx
    lidarFrame.h
    lidarFrameManager.cxx
    lidarFrameManager.h
    MainWindow.cxx
    MainWindow.h
    MainWindow.ui
    main.cxx
)
target_include_directories(BoundingBoxMe SYSTEM PRIVATE ${VTK_INCLUDE_DIRS})
target_link_libraries(BoundingBoxMe PRIVATE
  Qt5::Widgets
  ${VTK_LIBRARIES}
  Eigen3::Eigen
  )
