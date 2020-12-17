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

uniform float ndc_depth = 0.5f;

layout(location = 0) out vec2 texture_coordinates;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    switch (gl_VertexID)
    {
        case 0:
        texture_coordinates = vec2(0.f, 0.f);
        gl_Position         = vec4(-1.f, -1.f, ndc_depth, 1.f);
        break;

        case 1:
        texture_coordinates = vec2(1.f, 0.f);
        gl_Position         = vec4(1.f, -1.f, ndc_depth, 1.f);
        break;

        case 2:
        texture_coordinates = vec2(0.f, 1.f);
        gl_Position         = vec4(-1.f, 1.f, ndc_depth, 1.f);
        break;

        case 3:
        texture_coordinates = vec2(1.f, 1.f);
        gl_Position         = vec4(1.f, 1.f, ndc_depth, 1.f);
        break;

        default :
        texture_coordinates = vec2(-1.f);
        gl_Position         = vec4(0.f);
        break;
    }
}
