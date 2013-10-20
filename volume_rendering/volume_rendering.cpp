/*
 * VTKDemos
 * Copyright (C) 2013 Juan Hernando jhernando@fi.upm.es
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2.1 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "common/paths.h"

#include <vtkActor.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataSetReader.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>

vtkSmartPointer<vtkActor> doOutline(vtkDataSetReader *reader);

int main()
{
    vtkSmartPointer<vtkDataSetReader> reader = vtkDataSetReader::New();
    reader->SetFileName((common::dataPath() + "ironProt.vtk").c_str());

    /* No data transformations, connecting the reader directly to the data
       mapper for 3D texture based volume rendering. */
    vtkSmartPointer<vtkVolumeTextureMapper3D> mapper =
        vtkVolumeTextureMapper3D::New();
    mapper->SetInputConnection(reader->GetOutputPort());

    /* Creating the opacity and color transfer functions */
    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction =
        vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint(0, 0.0);
    opacityTransferFunction->AddPoint(255, 0.2);

    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
        vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(0, 1, 0, 0);
    colorTransferFunction->AddRGBPoint(255, 0, 0, 1);

    /* The volume rendering actor is special. It also inherit from vtkProp3D,
       but has nothing to do with vtkActor. */
    vtkSmartPointer<vtkVolume> volume = vtkVolume::New();
    volume->SetMapper(mapper);

    /* The transfer functions go into a vtkVolumeProperty, which is the object
       that holds rendering attributes for vtkVolume, as vtkProperty
       does for vtkActor */
    vtkSmartPointer<vtkVolumeProperty> volumeProperty =
        vtkVolumeProperty::New();
    volumeProperty->SetColor(colorTransferFunction);
    volumeProperty->SetScalarOpacity(opacityTransferFunction);
    volumeProperty->SetInterpolationTypeToLinear();
    volume->SetProperty(volumeProperty);

    /* Renderer */
    vtkSmartPointer<vtkRenderer> renderer = vtkRenderer::New();
    renderer->SetBackground(1, 1, 1);
    renderer->AddVolume(volume);
    renderer->AddActor(doOutline(reader));

    /* Interactor */
    vtkSmartPointer<vtkRenderWindow> window = vtkRenderWindow::New();
    window->AddRenderer(renderer);
    window->SetSize(800, 800);

    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
        vtkRenderWindowInteractor::New();
    interactor->SetRenderWindow(window);
    vtkSmartPointer<vtkInteractorStyleSwitch> interactorStyle =
        vtkInteractorStyleSwitch::New();
    interactor->SetInteractorStyle(interactorStyle);
    interactorStyle->SetCurrentStyleToTrackballCamera();

    interactor->Initialize();
    interactor->Start();
}

vtkSmartPointer<vtkActor> doOutline(vtkDataSetReader *reader)
{
    vtkSmartPointer<vtkOutlineFilter> outline =
        vtkSmartPointer<vtkOutlineFilter>::New();
    outline->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(outline->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 0, 0);

    return actor;
}
