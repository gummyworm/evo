#version 130

uniform mat4x4 ModelView, View, Projection;

in vec3 in_Position;
in vec2 in_Texco;
out vec2 ex_Texco;

void main()
{
   ex_Texco = in_Texco;
   gl_Position = ModelView * Projection * vec4(in_Position, 1.0);
}
