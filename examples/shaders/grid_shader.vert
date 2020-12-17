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

in int gl_VertexID;

uniform mat4  projection_from_world = mat4(1.f);
uniform float grid_width            = 10.f;
uniform int   grid_divisions        = 0;
uniform float time_secs             = 0.f;

out gl_PerVertex
{
    vec4 gl_Position;
};

ivec2 grid_index(in int index_1d)
{
    int width = grid_divisions + 2;

    ivec2 index_2d;
    index_2d.y = index_1d / width;
    index_2d.x = index_1d - (width * index_2d.y);
    return index_2d;
}

void main()
{
    ivec2 index_2d = grid_index(gl_VertexID);

    vec2 tex_coords  = vec2(index_2d) / float(grid_divisions + 1);

    vec3 world_position;
    world_position.xy  = tex_coords * grid_width - grid_width * 0.5f;
    world_position.z   = -10.f;
    world_position.z  += cos(time_secs + tex_coords.x * 10.f);
    world_position.z  += sin(time_secs + tex_coords.y * 10.f);
    world_position.z  *= 0.2f;

    gl_Position = projection_from_world * vec4(world_position, 1.f);
}
