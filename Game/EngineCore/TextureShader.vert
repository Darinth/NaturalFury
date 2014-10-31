#version 330

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
out vec3 eyeDirecFrag;

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
	vec3 attenuation;
};

struct SpotLight
{
	bool enabled;
	vec3 color;
	vec3 location;
	vec3 attenuation;
	vec3 direction;
	float fullDot;
	float minDot;
};

//Matrices
layout(shared) uniform MatrixBlock
{
	mat4 modelToCameraMatrix;
	mat4 cameraToClipMatrix;
} matrices;

const int NUM_POINT_LIGHTS = #PointLightCount#;
const int NUM_SPOT_LIGHTS = #SpotLightCount#;

//Lights
layout(shared) uniform LightBlock
{
	vec3 eyeDirection;
	vec3 ambientLight;
	DirectionalLight sun;
	PointLight pointLights[NUM_POINT_LIGHTS];
	SpotLight spotLights[NUM_SPOT_LIGHTS];
} lights;

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
