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

# YAML-cpp
find_package(yaml-cpp REQUIRED)


# The main application
add_executable(BoundingBoxMe
    assets/resources.qrc
    boundingBox.h
    boundingBox.cxx
    boundingBoxManager.h
    boundingBoxManager.cxx
    classesManager.cxx
    classesManager.h
    keyPressInteractorStyle.cxx
    keyPressInteractorStyle.h
    lidarFrame.cxx
    lidarFrame.h
    lidarFrameManager.cxx
    lidarFrameManager.h
    logger.cxx
    logger.h
    MainWindow.cxx
    MainWindow.h
    MainWindow.ui
    timeStepsManager.h
    timeStepsManager.cxx
    timeStepsManager_ui.cxx
    timeStepsManager_ui.h
    timeStepsManager_ui.ui
    vtkBoundingBoxManipulatorWidget.cxx
    vtkBoundingBoxManipulatorWidget.h
    main.cxx
)
target_include_directories(BoundingBoxMe SYSTEM PRIVATE
    ${VTK_INCLUDE_DIRS}
    ${YAML_CPP_INCLUDE_DIR}
    .
    )
target_link_libraries(BoundingBoxMe PRIVATE
  Qt5::Widgets
  ${VTK_LIBRARIES}
  Eigen3::Eigen
  ${YAML_CPP_LIBRARIES}
  )
