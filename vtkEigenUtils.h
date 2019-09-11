#ifndef VTKEIGENUTILS_H
#define VTKEIGENUTILS_H

#include <Eigen/Dense>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

vtkSmartPointer<vtkMatrix4x4> eigenIsometry3dToVtkMatrix4x4(const Eigen::Isometry3d& isometry);

Eigen::Isometry3d eigenIsometry3dFromVtkMatrix4x4(vtkMatrix4x4* matrix, Eigen::Vector3d* scaling=nullptr);

#endif // VTKEIGENUTILS_H
