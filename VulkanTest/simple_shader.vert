#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 color;
layout(location=2) in vec3 normal;
layout(location=3) in vec2 UV;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push{
	mat4 transform;
	mat4 normalMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0,-3.0,-1.0));
const float AMBIENT = 0.02;
void main() {
	//4d vector
	//{-1,-1} = top left, {0,0} = center
	gl_Position = push.transform * vec4(position,1.0);
	//Only works if scale is uniform (sx==sy==sz)
	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) *normal);
	//mat3 normalMatrix = transpose(inverse(mat3(push.modelMatrix)));
	float lightIntensity = AMBIENT + max(dot(normalWorldSpace,DIRECTION_TO_LIGHT),0);
	fragColor=lightIntensity*color;
}
