#version 330
#extension GL_EXT_texture_array: enable

//Output color
out vec4 outputColor;

void main()
{
	outputColor = vec4(gl_FragCoord.zzz, 1.0);
}
