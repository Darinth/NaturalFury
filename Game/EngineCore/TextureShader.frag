#version 330
#extension GL_EXT_texture_array: enable

//Inputs from vertex shader
in vec3 cameraPositionFrag;
in vec3 cameraNormalFrag;
in vec3 colorCoordFrag;
in float diffuseFrag;
in float specularFrag;

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
	//Calculating sunlight
	//Calculate diffuse light
	vec3 diffuseSun = max(0,dot(lights.sun.invDirection, cameraNormalFrag)) * diffuseFrag * lights.sun.color;
	//Calculate specular light
	//Reflect the sun light over the fragment normal, which will align the maximum specular light with the direction of the eye
	vec3 reflection = reflect(lights.sun.invDirection, cameraNormalFrag);
	//Compare direction of the eye with the reflected sun light to generate specular light.
	vec3 specularSun = pow(clamp( dot( normalize(cameraPositionFrag), reflection ), 0, 1 ), 5) * specularFrag * lights.sun.color;
	
	//Calculate point lights
	vec3 diffusePoint = vec3(0);
	vec3 specularPoint = vec3(0);
	//For each enabled light...
	for(int I = 0;I < NUM_POINT_LIGHTS;I++)
	{
		if(lights.pointLights[I].enabled)
		{
			vec3 pointToLight = lights.pointLights[I].location - cameraPositionFrag;
			//calculate attenuation
			float pointToLightDistance = length(pointToLight);
			float attenuation = lights.pointLights[I].attenuation.x + lights.pointLights[I].attenuation.y * pointToLightDistance + lights.pointLights[I].attenuation.y * pointToLightDistance * pointToLightDistance;
			
			//calculate diffuse light value
			diffusePoint = diffusePoint + max(0,dot(normalize(pointToLight), cameraNormalFrag)) * diffuseFrag / attenuation * lights.pointLights[I].color;
			
			//Calculate specular light value
			vec3 reflection = reflect(normalize(pointToLight), cameraNormalFrag);
			specularPoint = specularPoint + pow(clamp( dot( normalize(cameraPositionFrag), reflection ), 0, 1 ), 5) * specularFrag / attenuation * lights.pointLights[I].color;
		}
	}

	//Calculate spot lights
	vec3 diffuseSpot = vec3(0);
	vec3 specularSpot = vec3(0);
	//For each enabled light...
	for(int I = 0;I < NUM_SPOT_LIGHTS;I++)
	{
		if(lights.spotLights[I].enabled)
		{
			vec3 pointToLight = lights.spotLights[I].location - cameraPositionFrag;
			
			//Calculate the visibility of the light
			float obscurementDot = max(0,dot(normalize(pointToLight), lights.spotLights[I].direction));
			float visibility = clamp( (obscurementDot - lights.spotLights[I].minDot) / (lights.spotLights[I].fullDot - lights.spotLights[I].minDot), 0, 1);
				
			//calculate attenuation
			float pointToLightDistance = length(pointToLight);
			float attenuation = lights.spotLights[I].attenuation.x + lights.spotLights[I].attenuation.y * pointToLightDistance + lights.spotLights[I].attenuation.y * pointToLightDistance * pointToLightDistance;
			
			//calculate diffuse light value
			diffuseSpot = diffuseSpot + max(0,dot(normalize(pointToLight), cameraNormalFrag)) * diffuseFrag * visibility / attenuation * lights.spotLights[I].color;
			
			//Calculate specular light value
			vec3 reflection = reflect(normalize(pointToLight), cameraNormalFrag);
			specularSpot = specularSpot + pow(clamp( dot( normalize(cameraPositionFrag), reflection ), 0, 1 ), 5) * specularFrag * visibility / attenuation * lights.spotLights[I].color;
		}
	}

	//Calculate the final lighting value
	vec3 finalLighting = lights.ambientLight + diffuseSun + specularSun + diffusePoint + specularPoint + diffuseSpot + specularSpot;
	
	//Generate final color output as texture fragment multiplied by light components.
	outputColor = texture2DArray(textureArray, colorCoordFrag.yzx) * vec4((finalLighting), 1.0);//vec4(gl_FragCoord.zzz, 1.0);//
}
