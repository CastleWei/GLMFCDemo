#version 330 core
in vec3 ourColor;

out vec3 color;

void main()
{
	float factor = gl_FrontFacing ? 1.0 : 0.5;
	color = ourColor * factor;
}