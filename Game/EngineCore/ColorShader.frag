#version 330

in vec3 colorOut;
in vec4 normalOut;

out vec4 outputColor;

void main()
{
	outputColor = vec4(colorOut,1);
}
