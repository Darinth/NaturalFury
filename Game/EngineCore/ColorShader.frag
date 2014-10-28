#version 330

in vec4 colorOut;
in vec3 normalOut;
in float diffuseOut;
in float specularOut;

out vec4 outputColor;

struct DirectionalLight
{
	vec3 color;
	vec3 invDirection;
};

layout(shared) uniform MatrixBlock
{
	mat4 modelToCameraMatrix;
	mat4 cameraToClipMatrix;
} matrices;

layout(shared) uniform LightBlock
{
	vec3 eyeDirection;
	vec3 ambientLight;
	DirectionalLight sun;
} lights;

void main()
{
	vec3 diffuse = max(0,dot(lights.sun.invDirection, normalOut) * diffuseOut) * lights.sun.color;
	//float diffuse = dot(vec4(lights.sun.invDirection, 1.0), normalOut);
	//outputColor = vec4(diffuse, 0, 0, 0);
	//outputColor = normalOut;
	//outputColor = matrices.cameraToClipMatrix * matrices.modelToCameraMatrix * vec4(lights.sun.invDirection, 1.0);
	outputColor = colorOut * vec4((lights.ambientLight + diffuse), 1.0);
}
