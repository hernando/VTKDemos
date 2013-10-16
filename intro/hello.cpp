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

#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>

int main()
{
    /* Source data object that represents a sphere. */
    vtkSmartPointer<vtkSphereSource> sphere = vtkSphereSource::New();
    sphere->SetCenter(0, 0, 0);
    sphere->SetRadius(1);
    ///* Chaning the tessellation resolution. */
    //sphere->SetThetaResolution(64);
    //sphere->SetPhiResolution(64);

    /* Mapper object that will convert the sphere source into polygonal
       mesh. */
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(sphere->GetOutputPort());

    /* Creating the actor that will place the polygonal mesh into the
       scene and manage it's render attributes. */
    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    /* Creating the renderer and the render window */
    vtkSmartPointer<vtkRenderer> renderer = vtkRenderer::New();
    renderer->AddActor(actor);
    renderer->SetBackground(0.2, 0.3, 0.4);

    vtkSmartPointer<vtkRenderWindow> win = vtkRenderWindow::New();
    win->AddRenderer(renderer);
    win->SetSize(800, 800);

    /* Creating the interactor that handles the window events and provides
       the main rendering loop */
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
        vtkRenderWindowInteractor::New();
    interactor->SetRenderWindow(win);

    interactor->Initialize();
    interactor->Start();
}

