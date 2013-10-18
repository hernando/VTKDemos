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

#version 120
#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

flat varying vec4 sphere;
flat varying vec4 sphereColor;
varying vec3 eye;

vec4 phong(const vec4 baseColor,
           const vec3 normal, const vec3 eye, const vec3 light)
{
    vec4 color = baseColor;

    if (color.a == 0.0)
        discard;

    float lambertTerm = dot(normal, light);
    lambertTerm = abs(lambertTerm);

    /* Adding diffuse color */
    color.rgb =
        gl_LightSource[0].diffuse.rgb * color.rgb * lambertTerm;

    /* Adding specular reflection */
    const vec3 specular = vec3(0.2, 0.2, 0.1);
    vec3 r = reflect(-light, normal);
    color.rgb += specular * pow(max(dot(r, eye) , 0.0), 8.0);

    return color;
}


float sphereLineTest(vec3 c, float r, vec3 d)
{
    /* Real Time Rendering pg. 571 */
    float s = dot(c, d);
    float c2 = dot(c, c);
    float r2 = r * r;
    if (s < 0 && c2 > r2)
        return -1;
    float m2 = c2 - s * s;
    if (m2 > r2)
        return -1;
    float q = sqrt(r2 - m2);

    return (c2 > r2 ? s - q : s + q);
}

void propFuncFS()
{
    vec3 l = vec3(0.0, 0.0, 1.0);

    vec3 norm_eye = normalize(eye);
    float t = sphereLineTest(sphere.xyz, sphere.w, norm_eye);
    if (t == -1.0)
        discard;
    vec3 normal = normalize(norm_eye * t - sphere.xyz);
    float depth = 0.5 * (1.0 - gl_ProjectionMatrix[2][2] -
                               gl_ProjectionMatrix[3][2] / (norm_eye.z * t));

    vec4 color = phong(sphereColor, normal, -norm_eye, l);
    /* For approximate two sided transparency */
    color.a = color.a * (2.0 - color.a);

    gl_FragColor = color;
    gl_FragDepth = depth;
}
