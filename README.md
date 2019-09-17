# Bounding Box Me

This is an annotation tools


## How to build ?


### Dependencies:

Bounding Box Me has the following dependencies:

- Eigen3 (tested with version 3.3.4)
- VTK (>= 8.1.1)
- yaml-cpp
- Qt (tested with 5.9, 5.11 and 5.12)

 **WARNING**:  You need to enable Qt in VTK (VTK_Group_Qt should be set)


 ## Building VTK
 ```
 git clone git@gitlab.kitware.com:vtk/vtk.git
 cd vtk
 git checkout v8.2.0
 mkdir build
 cd build
 cmake .. -DCMAKE_BUILD_TYPE=Release -DVTK_Group_Qt=ON
 make -j8
 ```
 

 ## Building Bounding Box Me

 ```
 git clone git@gitlab.kitware.io:ComputerVision/BoundingBoxMe.git
 cd BoundingBoxMe
 mkdir build
 cd build
 cmake .. -DCMAKE_BUILD_TYPE=Release -DVTK_DIR=path/to/VTK/build/folder
 make -j8
 ./BoundingBoxMe
 ```

## Usage

With this software, you can load lidar point clouds (in .vtp format), bounding boxes and camera frames

Each dataset should have a .series file which list the items that it contains.

Sample data of lidar and bounding boxes are availble in:
```
annotation_example/
lidar_data_example/
```

You can add a new bounding box using 'q' (for a static BBox) or 'd' (for a dynamic one).
To create a BBox around a specific point, double-click on it before pressing 'q' or 'd'.


### Toolbar
![Toolbar](doc/Toolbar_help.png)

### Display tab

![Display tab](doc/display_tab.png)

You can enable the "Fast interaction" mode. This can be useful when the lidar dataset that you are displaying is very large. It just skip some frames during rendering for a faster interaction.

You can also filter which type of bounding boxes you want to display.
