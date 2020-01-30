#version 450 core
in vec4 frag_color;
in vec2 frag_uv;




layout(std140, binding = 0) uniform PerScene {
	mat4 projection;
};




uniform sampler2D u_texture;

out vec4 out_color; 
void main() {

	out_color = frag_color * texture(u_texture, frag_uv); 
	// out_color = frag_color;	
}