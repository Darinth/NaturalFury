#version 330
#extension GL_EXT_texture_array: enable

in vec3 colorCoord;
in vec4 normal;

uniform sampler2DArray imageArray;

out vec4 outputColor;

getColor

void main()
{
//	outputColor = vec4(colorCoord,1);
//	float textureNum = round(colorCoord.x);
	outputColor = texture2DArray(cubeTexture, colorCoord.yzx);
}
