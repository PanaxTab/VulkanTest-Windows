#version 450

layout(location=0) in vec2 position;
layout(location=1) in vec3 color;

layout(push_constant) uniform Push{
	vec2 offset; //Order must match
	vec3 color;	//the order of struct
} push;

void main() {
	//4d vector
	//{-1,-1} = top left, {0,0} = center
	gl_Position = vec4(position + push.offset,0.0,1.0);	
}