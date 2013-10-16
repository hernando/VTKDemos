set(VTKDEMOS_PACKAGE_VERSION 1.0.0)
#set(VTKDEMOS_REPO_URL https://github.com/hernando/VTKDemos.git)
set(VTKDEMOS_REPO_URL ssh://git@bb4.cesvima.upm.es/VTKDemos.git)
set(VTKDEMOS_DEPENDS REQUIRED Equalizer Boost VTK)
set(VTKDEMOS_FORCE_BUILD {CI_BUILD})



