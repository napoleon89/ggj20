#version 450 core
layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_tex;

out vec2 frag_uv;




layout(std140, binding = 0) uniform PerScene {
	mat4 projection;
};

layout(std140, binding = 1) uniform PerObject {
	mat4 model;
	vec4 color;
	vec2 uv_offset;
	vec2 uv_scale;
};

 

uniform sampler2D u_texture; 


void main() {

	frag_uv = in_tex * uv_scale + uv_offset;
	gl_Position = vec4(in_pos, 0.0f, 1.0f) * model * projection;
}