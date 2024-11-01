#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push{
	mat4 transform;
	vec3 color;	//the order of struct
} push;

void main() {
	//4d vector
	//{-1,-1} = top left, {0,0} = center
	gl_Position = push.transform * vec4(position,1.0);
	fragColor = color;
}