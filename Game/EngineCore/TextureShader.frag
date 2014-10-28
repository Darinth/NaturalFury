#version 330
#extension GL_EXT_texture_array: enable

//Inputs from vertex shader
in vec3 normalFrag;
in vec3 colorCoordFrag;
in float diffuseFrag;
in float specularFrag;
in vec3 eyeDirecFrag;

//Texture array for drawing
uniform sampler2DArray textureArray;

//Output color
out vec4 outputColor;

//Struct for a directional light, such as the sun
struct DirectionalLight
{
	vec3 color;
	vec3 invDirection;
};

//Struct for point lights
struct PointLight
{
	bool enabled;
	vec3 color;
	vec3 location;
};

//Matrices
layout(shared) uniform MatrixBlock
{
	mat4 modelToCameraMatrix;
	mat4 cameraToClipMatrix;
} matrices;

const int NUM_POINT_LIGHTS = #PointLightCount#;

//Lights
layout(shared) uniform LightBlock
{
	vec3 eyeDirection;
	vec3 ambientLight;
	DirectionalLight sun;
	PointLight pointLights[NUM_POINT_LIGHTS];
} lights;

void main()
{
	//Calculate diffuse light
	vec3 diffuse = max(0,dot(lights.sun.invDirection, normalFrag)) * diffuseFrag * lights.sun.color;
	
	//Calculate specular light
	//Reflect the sun light over the fragment normal, which will align the maximum specular light with the direction of the eye
	vec3 reflection = reflect(lights.sun.invDirection, normalFrag);
	//Compare direction of the eye with the reflected sun light to generate specular light.
	vec3 specular = pow(clamp( dot( normalize(eyeDirecFrag), reflection ), 0, 1 ), 5) * specularFrag * lights.sun.color;
	//Generate final color output as texture fragment multiplied by light components.
	outputColor = texture2DArray(textureArray, colorCoordFrag.yzx) * vec4((lights.ambientLight + diffuse + specular), 1.0);
}
