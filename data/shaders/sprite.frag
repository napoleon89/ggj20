#version 450 core
in vec2 frag_uv;




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


out vec4 out_color; 
void main() {

	vec4 texel = texture(u_texture, frag_uv);
	if(texel.a == 0.0) discard;
	vec4 diffuse = color * texel;
	out_color = diffuse;
}