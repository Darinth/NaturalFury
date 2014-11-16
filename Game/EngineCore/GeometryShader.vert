#version 420

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normalIn;
layout(location = 2) in float texNum;
layout(location = 3) in vec2 texCoord;
layout(location = 4) in float diffuseIn;
layout(location = 5) in float specularIn;

//Outputs sent to fragment shader
out vec3 cameraPositionFrag;
out vec3 cameraNormalFrag;
out vec3 colorCoordFrag;
out float diffuseFrag;
out float specularFrag;

//Matrices
layout(shared, binding = 1) uniform MatrixBlock
{
	mat4 modelToCameraMatrix;
	mat4 cameraToClipMatrix;
	vec2 screenDimensions;
} matrices;

void main()
{
	//Calculate final point for vertex
	gl_Position = matrices.cameraToClipMatrix * matrices.modelToCameraMatrix * vec4(position,1.0);

	//Calculate point of coordinate in camera space. Once normalized, this can be used in the fragment shader to calculate the direction of the point.
	cameraPositionFrag = (matrices.modelToCameraMatrix * vec4(position,1.0)).xyz;
	//Calculate camera view normal
	cameraNormalFrag = normalize(matrices.modelToCameraMatrix * vec4(normalIn, 0.0)).xyz;
	//Calculate texture coordinate
	colorCoordFrag = vec3(texNum,texCoord);
	//Pass diffuse and specular colors
	diffuseFrag = diffuseIn;
	specularFrag = specularIn;
}
