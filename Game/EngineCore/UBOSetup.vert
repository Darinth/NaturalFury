#version 420

//Output color
out vec4 outputColor;

//Matrices
layout(shared, binding = 1) uniform MatrixBlock
{
	mat4 modelToCameraMatrix;
	mat4 cameraToClipMatrix;
	vec2 screenDimensions;
} matrices;

const int NUM_POINT_LIGHTS = #PointLightCount#;
const int NUM_SPOT_LIGHTS = #SpotLightCount#;

//Struct for a directional light, such as the sun
struct DirectionalLight
{
	vec3 color;
	vec3 invDirection;
	mat4 toLightClip;
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

//Lights
layout(shared, binding = 2) uniform LightBlock
{
	vec3 eyeDirection;
	vec3 ambientLight;
	DirectionalLight sun;
	PointLight pointLights[NUM_POINT_LIGHTS];
	SpotLight spotLights[NUM_SPOT_LIGHTS];
} lights;

void main()
{
	gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}
