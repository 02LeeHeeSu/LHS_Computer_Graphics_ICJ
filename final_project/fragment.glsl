#version 330 core

in vec3 Out_Color;

out vec4 FragColor;  

void main()
{
	FragColor = vec4(Out_Color, 1.0f);
}
