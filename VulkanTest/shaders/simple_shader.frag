#version 450

layout(location=0) out	vec4 outColor;

layout(push_constant) uniform Push{
	mat2 transform;
	vec2 offset; //Order must match
	vec3 color;	//the order of struct
} push;

void main(){
	outColor = vec4(push.color,1.0);
}