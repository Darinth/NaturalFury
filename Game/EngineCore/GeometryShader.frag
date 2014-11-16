#version 420

//Texture array for drawing
layout(binding=0) uniform sampler2DArray textureArray;

//Inputs from vertex shader
in vec3 cameraPositionFrag;
in vec3 cameraNormalFrag;
in vec3 colorCoordFrag;
in float diffuseFrag;
in float specularFrag;

layout (location = 0) out vec3 cameraPositionGBuffer;
layout (location = 1) out vec3 cameraNormalGBuffer;
layout (location = 2) out vec3 colorGBuffer;
layout (location = 3) out float diffuseGBuffer;
layout (location = 4) out float specularGBuffer;

void main()
{
	cameraPositionGBuffer = cameraPositionFrag;
	cameraNormalGBuffer = cameraNormalFrag;
	colorGBuffer = texture(textureArray, colorCoordFrag.yzx).xyz;
	diffuseGBuffer = diffuseFrag;
	specularGBuffer = specularFrag;
}
