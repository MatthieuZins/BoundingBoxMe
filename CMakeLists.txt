#=========================================================================
#
# Copyright 2019 Kitware, Inc.
# Author: Matthieu Zins
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#=========================================================================

cmake_minimum_required(VERSION 3.10)

project(BoundingBoxMe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)


# Find current commit hash
include(${CMAKE_CURRENT_SOURCE_DIR}/git.cmake)
determine_current_commit_hash(${CMAKE_CURRENT_SOURCE_DIR} ${GIT_EXECUTABLE})
message("Current build commit hash is ${BUILD_COMMIT_HASH}")
add_definitions(-DBUILD_COMMIT_HASH="${BUILD_COMMIT_HASH}")

# Eigen
find_package(Eigen3 REQUIRED)

# Qt dependancy
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# VTK dependancy
find_package(VTK 8.1.1 REQUIRED)
include(${VTK_USE_FILE})

# YAML-cpp
find_package(yaml-cpp REQUIRED)


# The main application
add_executable(BoundingBoxMe
    assets/resources.qrc
    boundingBox.h
    boundingBox.cxx
    boundingBoxInformation_ui.ui
    boundingBoxInformation_ui.cxx
    boundingBoxInformation_ui.h
    boundingBoxManager.h
    boundingBoxManager.cxx
    cameraFrame.cxx
    cameraFrame.h
    cameraFramesManager.cxx
    cameraFramesManager.h
    classesManager.cxx
    classesManager.h
    datasetHelper.cxx
    datasetHelper.h
    displayManager_ui.cxx
    displayManager_ui.h
    displayManager_ui.ui
    keyPressInteractorStyle.cxx
    keyPressInteractorStyle.h
    lidarFrame.cxx
    lidarFrame.h
    lidarFrameManager.cxx
    lidarFrameManager.h
    logger.cxx
    logger.h
    main.cxx
    MainWindow.cxx
    MainWindow.h
    MainWindow.ui
    sideViewsInteractorStyle.cxx
    sideViewsInteractorStyle.h
    timeStepsManager.h
    timeStepsManager.cxx
    timeStepsManager_ui.cxx
    timeStepsManager_ui.h
    timeStepsManager_ui.ui
    vtkBoundingBoxManipulatorWidget.cxx
    vtkBoundingBoxManipulatorWidget.h
    vtkEigenUtils.cxx
    vtkEigenUtils.h
    vtkBoundingBoxSource.h
    vtkBoundingBoxSource.cxx
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
  stdc++fs          # for std::filesystem
  )

