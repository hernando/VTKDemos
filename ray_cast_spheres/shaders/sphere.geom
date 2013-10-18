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
#extension GL_ARB_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

varying in vec4 color[];
varying in float radius[];
varying flat out vec4 sphere;
varying flat out vec4 sphereColor;
varying out vec3 eye;

float cathetus(float hypot, float cat)
{
    return sqrt(hypot * hypot - cat * cat);
}

/**
   Returns the coordinates of the tangency point of a line passing by
   a point located at (d, 0) with d > 0 and tangent to a circle
   centered at origin and radius r.
 */
vec2 tangencyPoint(float radius, float distance)
{
    float a = (radius * radius) / distance;
    return vec2(a, cathetus(radius, a));
}

void main()
{
    if (radius[0] < 0)
       return;
    sphere = vec4(gl_PositionIn[0].xyz, radius[0]);

    /* Left and right tangent lines */
    float quadDepth = sphere.z + radius[0];
    float d_floor = length(sphere.xz);
    /* t_floor and ortho_t_floor are the canocical 2D base for the floor
       projection (xz plane) in which the origin and the sphere center are
       on the horizontal axis. */
    vec2 t_floor = sphere.xz / d_floor;
    vec2 ortho_t_floor = vec2(t_floor.y, -t_floor.x);
    vec2 point = tangencyPoint(radius[0], d_floor);
    /* Transforming the tangency point from the sphere centered reference
       frame to the camera reference frame */
    vec2 leftTangent = sphere.xz - t_floor * point.x + ortho_t_floor * point.y;
    vec2 rightTangent = sphere.xz - t_floor * point.x - ortho_t_floor * point.y;

    /* Top and bottom tangent lines */
    float d_side = length(sphere.yz);
    /* Same as above for the yz plane */
    vec2 t_side = sphere.yz / d_side;
    vec2 ortho_t_side = vec2(t_side.y, -t_side.x);
    point = tangencyPoint(radius[0], d_side);
    vec2 topTangent = sphere.yz - t_side * point.x + ortho_t_side * point.y;
    vec2 bottomTangent = sphere.yz - t_side * point.x - ortho_t_side * point.y;

    /* Projecting the tangent points to the screen aligned plane tangent
       to the sphere on the front */
    vec4 corners[4];
    corners[0].x = corners[1].x = leftTangent.x * quadDepth / leftTangent.y;
    corners[2].x = corners[3].x = rightTangent.x * quadDepth / rightTangent.y;

    corners[0].y = corners[2].y = bottomTangent.x * quadDepth / bottomTangent.y;
    corners[1].y = corners[3].y = topTangent.x * quadDepth / topTangent.y;

    corners[0].zw = corners[1].zw = vec2(quadDepth, 1);
    corners[2].zw = corners[3].zw = vec2(quadDepth, 1);

    sphereColor = color[0];

#define VERTEX(i) \
    eye = corners[i].xyz; \
    gl_Position = gl_ProjectionMatrix * corners[i]; \
    EmitVertex();

    VERTEX(0);
    VERTEX(1);
    VERTEX(2);
    VERTEX(3);
    EndPrimitive();
}
