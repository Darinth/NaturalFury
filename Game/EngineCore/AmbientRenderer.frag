#version 420

//Texture array for drawing
layout(binding=0) uniform sampler2D gPositionMap;

//Texture array for drawing
layout(binding=1) uniform sampler2D gNormalMap;

//Texture array for drawing
layout(binding=2) uniform sampler2D gColorMap;

//Texture array for drawing
layout(binding=3) uniform sampler2D gDiffuseMap;

//Texture array for drawing
layout(binding=4) uniform sampler2D gSpecularMap;

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
   	vec2 texCoord = gl_FragCoord.xy / matrices.screenDimensions;
   	vec3 cameraPos = texture(gPositionMap, texCoord).xyz;
   	vec3 color = texture(gColorMap, texCoord).xyz;
   	vec3 normal = normalize(texture(gNormalMap, texCoord).xyz);
	float diffuse = texture(gDiffuseMap, texCoord).x;
	float specular = texture(gSpecularMap, texCoord).x;

	//Calculating sunlight
	//Calculate diffuse light
	vec3 diffuseSun = max(0,dot(lights.sun.invDirection, normal)) * diffuse * lights.sun.color;
	//Calculate specular light
	//Reflect the sun light over the fragment normal, which will align the maximum specular light with the direction of the eye
	vec3 reflection = reflect(lights.sun.invDirection, normal);
	//Compare direction of the eye with the reflected sun light to generate specular light.
	vec3 specularSun = pow(clamp( dot( normalize(cameraPos), reflection ), 0, 1 ), 5) * specular * lights.sun.color;

   	//outputColor = vec4(diffuseSun, 1.0);
	outputColor = vec4(color * (lights.ambientLight + diffuseSun + specularSun), 1.0);
}
