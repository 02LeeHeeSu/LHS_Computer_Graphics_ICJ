#version 330 core

layout (location = 0) in vec3 aPos;   // ��ġ ������ attribute position 0�� �����ϴ�.
layout (location = 1) in vec3 in_Color;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

out vec3 Out_Color;

void main()
{
	gl_Position = p * v * m *vec4 (aPos, 1.0);

	Out_Color = in_Color;
}  
