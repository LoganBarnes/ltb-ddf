// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Geometry Visualization Server
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ///////////////////////////////////////////////////////////////////////////////////////

// version will be inserted automagically

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

layout(location = 0) in vec4 center_and_radius[];

uniform mat4 projection_from_world = mat4(1.f);

layout(location = 0) out vec3 world_position_out;
layout(location = 1) out vec3 world_normal_out;
layout(location = 2) out vec2 texture_coordinates_out;
layout(location = 3) out vec3 vertex_color_out;

out gl_PerVertex
{
    vec4 gl_Position;
};

void add_face(in vec3 p0, in vec3 p1, in vec3 p2, in vec3 p3, in vec3 n, in vec3 c)
{
    world_position_out      = p0;
    world_normal_out        = n;
    texture_coordinates_out = vec2(0.0, 0.0);
    vertex_color_out        = c;
    gl_Position             = projection_from_world * vec4(world_position_out, 1.0);
    EmitVertex();

    world_position_out      = p1;
    world_normal_out        = n;
    texture_coordinates_out = vec2(1.0, 0.0);
    vertex_color_out        = c;
    gl_Position             = projection_from_world * vec4(world_position_out, 1.0);
    EmitVertex();

    world_position_out      = p2;
    world_normal_out        = n;
    texture_coordinates_out = vec2(0.0, 1.0);
    vertex_color_out        = c;
    gl_Position             = projection_from_world * vec4(world_position_out, 1.0);
    EmitVertex();

    world_position_out      = p3;
    world_normal_out        = n;
    texture_coordinates_out = vec2(1.0, 1.0);
    vertex_color_out        = c;
    gl_Position             = projection_from_world * vec4(world_position_out, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    vec3  center = center_and_radius[0].xyz;
    float radius = center_and_radius[0].w;
    vec3  color  = vec3(0.8);

    add_face(
        center + vec3(-1, -1, 1.f) * radius,
        center + vec3(+1, -1, 1.f) * radius,
        center + vec3(-1, +1, 1.f) * radius,
        center + vec3(+1, +1, 1.f) * radius,
        vec3(0.f, 0.f, 1.f),
        color
    );
}
