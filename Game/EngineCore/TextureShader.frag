#version 330
#extension GL_EXT_texture_array: enable

in vec3 colorCoord;
in vec4 normal;

uniform sampler2DArray cubeTexture;

out vec4 outputColor;

void main()
{
//	outputColor = vec4(colorCoord,1);
//	float textureNum = round(colorCoord.x);
	outputColor = texture2DArray(cubeTexture, colorCoord.yzx);
}
