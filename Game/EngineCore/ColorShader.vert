#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normalIn;
layout(location = 2) in vec4 colorIn;
layout(location = 3) in float diffuseIn;
layout(location = 4) in float specularIn;

out vec3 normalOut;
out vec4 colorOut;
out float diffuseOut;
out float specularOut;

layout(shared) uniform MatrixBlock
{
  mat4 modelToCameraMatrix;
  mat4 cameraToClipMatrix;
} matrices;

void main()
{
	gl_Position = matrices.cameraToClipMatrix * matrices.modelToCameraMatrix * vec4(position,1.0);
	normalOut = normalize(normalIn);
	diffuseOut = diffuseIn;
	specularOut = specularIn;

	colorOut = colorIn;
}
