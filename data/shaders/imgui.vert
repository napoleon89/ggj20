#version 450 core
layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_color;

out vec4 frag_color;
out vec2 frag_uv;




layout(std140, binding = 0) uniform PerScene {
	mat4 projection;
};




uniform sampler2D u_texture;

void main() {

	frag_color = in_color;
	frag_uv = in_uv; 
	gl_Position = projection * vec4(in_pos, 0.0f, 1.0f);
}