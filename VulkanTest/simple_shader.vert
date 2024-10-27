#version 450

layout(location=0) in vec2 position;

void main() {
	//4d vector
	//{-1,-1} = top left, {0,0} = center
	gl_Position = vec4(position,0.0,1.0);
}