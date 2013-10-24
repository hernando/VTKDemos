#!/usr/bin/python
#
# VTKDemos
# Copyright (C) 2013 Juan Hernando jhernando@fi.upm.es
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2.1 as published
# by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

import paths
from vtk import *

reader = vtkDataSetReader()
reader.SetFileName(paths.data_path + 'ironProt.vtk')

mapper = vtkVolumeTextureMapper3D()
mapper.SetInputConnection(reader.GetOutputPort())

# Creating the opacity and color transfer functions
opacityTransferFunction = vtkPiecewiseFunction()
opacityTransferFunction.AddPoint(0, 0.0)
opacityTransferFunction.AddPoint(255, 0.2)

colorTransferFunction = vtkColorTransferFunction();
colorTransferFunction.AddRGBPoint(0, 1, 0, 0)
colorTransferFunction.AddRGBPoint(255, 0, 0, 1)

# The volume rendering actor is special. It also inherit from vtkProp3D,
#  but has nothing to do with vtkActor.
volume = vtkVolume()
volume.SetMapper(mapper)

# The transfer functions go into a vtkVolumeProperty, which is the object
# that holds rendering attributes for vtkVolume, as vtkProperty
# does for vtkActor
volumeProperty = vtkVolumeProperty()
volumeProperty.SetColor(colorTransferFunction)
volumeProperty.SetScalarOpacity(opacityTransferFunction)
volumeProperty.SetInterpolationTypeToLinear()
volume.SetProperty(volumeProperty)


renderer = vtkRenderer()
renderer.SetBackground(1, 1, 1)
renderer.AddVolume(volume)

outline = vtkOutlineFilter()
outline.SetInputConnection(reader.GetOutputPort())
mapper = vtkPolyDataMapper()
mapper.SetInputConnection(outline.GetOutputPort())
actor = vtkActor()
actor.SetMapper(mapper)
actor.GetProperty().SetColor(0, 0, 0)
renderer.AddActor(actor)

window = vtkRenderWindow()
window.AddRenderer(renderer)
window.SetSize(800, 800)

interactor = vtkRenderWindowInteractor()
interactor.SetRenderWindow(window)
interactorStyle = vtkInteractorStyleSwitch()
interactor.SetInteractorStyle(interactorStyle)
interactorStyle.SetCurrentStyleToTrackballCamera()

interactor.Initialize()
interactor.Start()

