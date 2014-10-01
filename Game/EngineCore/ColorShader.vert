#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normalIn;
layout(location = 2) in vec3 colorIn;

out vec4 normalOut;
out vec3 colorOut;

layout(shared) uniform MatrixBlock
{
  mat4 modelToCameraMatrix;
  mat4 cameraToClipMatrix;
} matrices;

void main()
{
	gl_Position = matrices.cameraToClipMatrix * matrices.modelToCameraMatrix * vec4(position,1.0);
	normalOut = matrices.cameraToClipMatrix * matrices.modelToCameraMatrix * vec4(normalIn,1.0);

	colorOut = colorIn;
}
