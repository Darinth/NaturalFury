#version 330

layout(location = 0) in vec3 position;

//Matrices
layout(shared) uniform MatrixBlock
{
	mat4 modelToCameraMatrix;
	mat4 cameraToClipMatrix;
} matrices;

void main()
{
	//Calculate final point for vertex
	gl_Position = matrices.cameraToClipMatrix * matrices.modelToCameraMatrix * vec4(position,1.0);
}
