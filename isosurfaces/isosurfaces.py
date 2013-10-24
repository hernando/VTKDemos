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

def createOutline(reader) :
    outline = vtkOutlineFilter()
    outline.SetInputConnection(reader.GetOutputPort())

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(outline.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)

    return actor

def createBoundaryWithColorMap(reader) :
    reader.Update()
    range = (reader.GetOutput().GetScalarRange()[0],
             reader.GetOutput().GetScalarRange()[1])

    transferFunction = vtkColorTransferFunction()
    transferFunction.AddRGBPoint(range[0], 1, 0, 0)
    transferFunction.AddRGBPoint(range[1], 0, 0, 1)

    mapper = vtkDataSetMapper()
    mapper.SetInputConnection(reader.GetOutputPort())
    mapper.SetScalarRange(range)
    mapper.SetLookupTable(transferFunction)

    actor = vtkActor()
    actor.SetMapper(mapper)

    return actor

def createIsosurfaces(reader) :
    contour = vtkContourFilter()
    contour.SetInputConnection(reader.GetOutputPort())
    contour.SetNumberOfContours(3)
    contour.SetValue(0, 1.5)
    contour.SetValue(1, 3.0)
    contour.SetValue(2, 4.5)

    reader.Update()
    range = (reader.GetOutput().GetScalarRange()[0],
             reader.GetOutput().GetScalarRange()[1])

    mapper = vtkDataSetMapper()
    mapper.SetInputConnection(contour.GetOutputPort())
    mapper.SetScalarRange(range)

    actor = vtkActor()
    actor.SetMapper(mapper)

    return actor

def createCutPlane(reader) :
    cutter = vtkCutter()
    plane = vtkPlane()
    plane.SetNormal(1, 1, 1)
    plane.SetOrigin(0, 0, 0)
    cutter.SetCutFunction(plane)
    cutter.SetInputConnection(reader.GetOutputPort())

    reader.Update()
    range = (reader.GetOutput().GetScalarRange()[0],
             reader.GetOutput().GetScalarRange()[1])
    mapper = vtkDataSetMapper()
    mapper.SetScalarRange(range)
    mapper.SetInputConnection(cutter.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)

    return actor

reader = vtkDataSetReader()
reader.SetFileName(paths.data_path + "noise.vtk")

renderer = vtkRenderer()
renderer.SetBackground(0.2, 0.3, 0.4)

renderer.AddActor(createOutline(reader))
#renderer.AddActor(createBoundaryWithColorMap(reader))
renderer.AddActor(createIsosurfaces(reader))
renderer.AddActor(createCutPlane(reader))

window = vtkRenderWindow()
window.AddRenderer(renderer)
window.SetSize(800, 800)

interactor =vtkRenderWindowInteractor()
interactor.SetRenderWindow(window)
interactorStyle = vtkInteractorStyleSwitch()
interactor.SetInteractorStyle(interactorStyle)
interactorStyle.SetCurrentStyleToTrackballCamera()
interactor.Initialize()
interactor.Start()


