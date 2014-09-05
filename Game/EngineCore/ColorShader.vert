#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colorIn;

out vec3 colorOut;

uniform mat4 cameraToClipMatrix;
uniform mat4 modelToCameraMatrix;

void main()
{
	gl_Position = cameraToClipMatrix * modelToCameraMatrix * vec4(position,1.0);

	colorOut = colorIn;
}
