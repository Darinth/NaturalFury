#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in float texNum;
layout(location = 2) in vec2 texCoord;

out vec3 colorCoord;

layout(shared) uniform MatrixBlock
{
  mat4 cameraToClipMatrix;
  mat4 modelToCameraMatrix;
} matrixBlock;

void main()
{
	gl_Position = matrixBlock.cameraToClipMatrix * matrixBlock.modelToCameraMatrix * vec4(position,1.0);

	colorCoord = vec3(texNum,texCoord);
}
